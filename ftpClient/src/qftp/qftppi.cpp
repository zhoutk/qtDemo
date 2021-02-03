#include "qftppi.h"

QFtpPI::QFtpPI(QObject* parent) :
    QObject(parent),
    rawCommand(false),
    transferConnectionExtended(true),
    dtp(this),
    commandSocket(0),
    state(Begin), abortState(None),
    currentCmd(QString()),
    waitForDtpToConnect(false),
    waitForDtpToClose(false)
{
    commandSocket.setObjectName(QLatin1String("QFtpPI_socket"));
    connect(&commandSocket, SIGNAL(hostFound()),
        SLOT(hostFound()));
    connect(&commandSocket, SIGNAL(connected()),
        SLOT(connected()));
    connect(&commandSocket, SIGNAL(disconnected()),
        SLOT(connectionClosed()));
    connect(&commandSocket, SIGNAL(readyRead()),
        SLOT(readyRead()));
    connect(&commandSocket, SIGNAL(error(QAbstractSocket::SocketError)),
        SLOT(error(QAbstractSocket::SocketError)));

    connect(&dtp, SIGNAL(connectState(int)),
        SLOT(dtpConnectState(int)));
}

void QFtpPI::connectToHost(const QString& host, quint16 port)
{
    emit connectState(QFtp::HostLookup);
#ifndef QT_NO_BEARERMANAGEMENT
    //copy network session down to the socket & DTP
    commandSocket.setProperty("_q_networksession", property("_q_networksession"));
    dtp.setProperty("_q_networksession", property("_q_networksession"));
#endif
    commandSocket.connectToHost(host, port);
}

/*
  Sends the sequence of commands \a cmds to the FTP server. When the commands
  are all done the finished() signal is emitted. When an error occurs, the
  error() signal is emitted.

  If there are pending commands in the queue this functions returns false and
  the \a cmds are not added to the queue; otherwise it returns true.
*/
bool QFtpPI::sendCommands(const QStringList& cmds)
{
    if (!pendingCommands.isEmpty())
        return false;

    if (commandSocket.state() != QTcpSocket::ConnectedState || state != Idle) {
        emit error(QFtp::NotConnected, QFtp::tr("Not connected"));
        return true; // there are no pending commands
    }

    pendingCommands = cmds;
    startNextCmd();
    return true;
}

void QFtpPI::clearPendingCommands()
{
    pendingCommands.clear();
    dtp.abortConnection();
    currentCmd.clear();
    state = Idle;
}

void QFtpPI::abort()
{
    pendingCommands.clear();

    if (abortState != None)
        // ABOR already sent
        return;

    abortState = AbortStarted;
#if defined(QFTPPI_DEBUG)
    qDebug("QFtpPI send: ABOR");
#endif
    commandSocket.write("ABOR\r\n", 6);

    if (currentCmd.startsWith(QLatin1String("STOR ")))
        dtp.abortConnection();
}

void QFtpPI::hostFound()
{
    emit connectState(QFtp::Connecting);
}

void QFtpPI::connected()
{
    state = Begin;
#if defined(QFTPPI_DEBUG)
    //    qDebug("QFtpPI state: %d [connected()]", state);
#endif
    // try to improve performance by setting TCP_NODELAY
    commandSocket.setSocketOption(QAbstractSocket::LowDelayOption, 1);

    emit connectState(QFtp::Connected);
}

void QFtpPI::connectionClosed()
{
    commandSocket.close();
    emit connectState(QFtp::Unconnected);
}

void QFtpPI::delayedCloseFinished()
{
    emit connectState(QFtp::Unconnected);
}

void QFtpPI::error(QAbstractSocket::SocketError e)
{
    if (e == QTcpSocket::HostNotFoundError) {
        emit connectState(QFtp::Unconnected);
        emit error(QFtp::HostNotFound,
            QFtp::tr("Host %1 not found").arg(commandSocket.peerName()));
    }
    else if (e == QTcpSocket::ConnectionRefusedError) {
        emit connectState(QFtp::Unconnected);
        emit error(QFtp::ConnectionRefused,
            QFtp::tr("Connection refused to host %1").arg(commandSocket.peerName()));
    }
    else if (e == QTcpSocket::SocketTimeoutError) {
        emit connectState(QFtp::Unconnected);
        emit error(QFtp::ConnectionRefused,
            QFtp::tr("Connection timed out to host %1").arg(commandSocket.peerName()));
    }
}

void QFtpPI::readyRead()
{
    if (waitForDtpToClose)
        return;

    while (commandSocket.canReadLine()) {
        // read line with respect to line continuation
        QString line = QString::fromLatin1(commandSocket.readLine());
        if (replyText.isEmpty()) {
            if (line.length() < 3) {
                // protocol error
                return;
            }
            const int lowerLimit[3] = { 1,0,0 };
            const int upperLimit[3] = { 5,5,9 };
            for (int i = 0; i < 3; i++) {
                replyCode[i] = line[i].digitValue();
                if (replyCode[i]<lowerLimit[i] || replyCode[i]>upperLimit[i]) {
                    // protocol error
                    return;
                }
            }
        }
        QString endOfMultiLine;
        endOfMultiLine[0] = '0' + replyCode[0];
        endOfMultiLine[1] = '0' + replyCode[1];
        endOfMultiLine[2] = '0' + replyCode[2];
        endOfMultiLine[3] = QLatin1Char(' ');
        QString lineCont(endOfMultiLine);
        lineCont[3] = QLatin1Char('-');
        QString lineLeft4 = line.left(4);

        while (lineLeft4 != endOfMultiLine) {
            if (lineLeft4 == lineCont)
                replyText += line.mid(4); // strip 'xyz-'
            else
                replyText += line;
            if (!commandSocket.canReadLine())
                return;
            line = QString::fromLatin1(commandSocket.readLine());
            lineLeft4 = line.left(4);
        }
        replyText += line.mid(4); // strip reply code 'xyz '
        if (replyText.endsWith(QLatin1String("\r\n")))
            replyText.chop(2);

        if (processReply())
            replyText = QLatin1String("");
    }
}

/*
  Process a reply from the FTP server.

  Returns true if the reply was processed or false if the reply has to be
  processed at a later point.
*/
bool QFtpPI::processReply()
{
#if defined(QFTPPI_DEBUG)
    //    qDebug("QFtpPI state: %d [processReply() begin]", state);
    if (replyText.length() < 400)
        qDebug("QFtpPI recv: %d %s", 100 * replyCode[0] + 10 * replyCode[1] + replyCode[2], replyText.toLatin1().constData());
    else
        qDebug("QFtpPI recv: %d (text skipped)", 100 * replyCode[0] + 10 * replyCode[1] + replyCode[2]);
#endif

    int replyCodeInt = 100 * replyCode[0] + 10 * replyCode[1] + replyCode[2];

    // process 226 replies ("Closing Data Connection") only when the data
    // connection is really closed to avoid short reads of the DTP
    if (replyCodeInt == 226 || (replyCodeInt == 250 && currentCmd.startsWith(QLatin1String("RETR")))) {
        if (dtp.state() != QTcpSocket::UnconnectedState) {
            waitForDtpToClose = true;
            return false;
        }
    }

    switch (abortState) {
    case AbortStarted:
        abortState = WaitForAbortToFinish;
        break;
    case WaitForAbortToFinish:
        abortState = None;
        return true;
    default:
        break;
    }

    // get new state
    static const State table[5] = {
        /* 1yz   2yz      3yz   4yz      5yz */
        Waiting, Success, Idle, Failure, Failure
    };
    switch (state) {
    case Begin:
        if (replyCode[0] == 1) {
            return true;
        }
        else if (replyCode[0] == 2) {
            state = Idle;
            emit finished(QFtp::tr("Connected to host %1").arg(commandSocket.peerName()));
            break;
        }
        // reply codes not starting with 1 or 2 are not handled.
        return true;
    case Waiting:
        if (static_cast<signed char>(replyCode[0]) < 0 || replyCode[0] > 5)
            state = Failure;
        else
#if defined(Q_OS_IRIX) && defined(Q_CC_GNU)
        {
            // work around a crash on 64 bit gcc IRIX
            State* t = (State*)table;
            state = t[replyCode[0] - 1];
        }
#else
            if (replyCodeInt == 202)
                state = Failure;
            else
                state = table[replyCode[0] - 1];
#endif
        break;
    default:
        // ignore unrequested message
        return true;
    }
#if defined(QFTPPI_DEBUG)
    //    qDebug("QFtpPI state: %d [processReply() intermediate]", state);
#endif

    // special actions on certain replies
    emit rawFtpReply(replyCodeInt, replyText);
    if (rawCommand) {
        rawCommand = false;
    }
    else if (replyCodeInt == 227) {
        // 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
        // rfc959 does not define this response precisely, and gives
        // both examples where the parenthesis are used, and where
        // they are missing. We need to scan for the address and host
        // info.
        QRegExp addrPortPattern(QLatin1String("(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)"));
        if (addrPortPattern.indexIn(replyText) == -1) {
#if defined(QFTPPI_DEBUG)
            qDebug("QFtp: bad 227 response -- address and port information missing");
#endif
            // this error should be reported
        }
        else {
            QStringList lst = addrPortPattern.capturedTexts();
            QString host = lst[1] + QLatin1Char('.') + lst[2] + QLatin1Char('.') + lst[3] + QLatin1Char('.') + lst[4];
            quint16 port = (lst[5].toUInt() << 8) + lst[6].toUInt();
            waitForDtpToConnect = true;
            dtp.connectToHost(host, port);
        }
    }
    else if (replyCodeInt == 229) {
        // 229 Extended Passive mode OK (|||10982|)
        int portPos = replyText.indexOf(QLatin1Char('('));
        if (portPos == -1) {
#if defined(QFTPPI_DEBUG)
            qDebug("QFtp: bad 229 response -- port information missing");
#endif
            // this error should be reported
        }
        else {
            ++portPos;
            QChar delimiter = replyText.at(portPos);
            QStringList epsvParameters = replyText.mid(portPos).split(delimiter);

            waitForDtpToConnect = true;
            dtp.connectToHost(commandSocket.peerAddress().toString(),
                epsvParameters.at(3).toInt());
        }

    }
    else if (replyCodeInt == 230) {
        if (currentCmd.startsWith(QLatin1String("USER ")) && pendingCommands.count() > 0 &&
            pendingCommands.first().startsWith(QLatin1String("PASS "))) {
            // no need to send the PASS -- we are already logged in
            pendingCommands.pop_front();
        }
        // 230 User logged in, proceed.
        emit connectState(QFtp::LoggedIn);
    }
    else if (replyCodeInt == 213) {
        // 213 File status.
        if (currentCmd.startsWith(QLatin1String("SIZE ")))
            dtp.setBytesTotal(replyText.simplified().toLongLong());
    }
    else if (replyCode[0] == 1 && currentCmd.startsWith(QLatin1String("STOR "))) {
        dtp.waitForConnection();
        dtp.writeData();
    }

    // react on new state
    switch (state) {
    case Begin:
        // should never happen
        break;
    case Success:
        // success handling
        state = Idle;
        // no break!
    case Idle:
        if (dtp.hasError()) {
            emit error(QFtp::UnknownError, dtp.errorMessage());
            dtp.clearError();
        }
        startNextCmd();
        break;
    case Waiting:
        // do nothing
        break;
    case Failure:
        // If the EPSV or EPRT commands fail, replace them with
        // the old PASV and PORT instead and try again.
        if (currentCmd.startsWith(QLatin1String("EPSV"))) {
            transferConnectionExtended = false;
            pendingCommands.prepend(QLatin1String("PASV\r\n"));
        }
        else if (currentCmd.startsWith(QLatin1String("EPRT"))) {
            transferConnectionExtended = false;
            pendingCommands.prepend(QLatin1String("PORT\r\n"));
        }
        else {
            emit error(QFtp::UnknownError, replyText);
        }
        if (state != Waiting) {
            state = Idle;
            startNextCmd();
        }
        break;
    }
#if defined(QFTPPI_DEBUG)
    //    qDebug("QFtpPI state: %d [processReply() end]", state);
#endif
    return true;
}

/*
  Starts next pending command. Returns false if there are no pending commands,
  otherwise it returns true.
*/
bool QFtpPI::startNextCmd()
{
    if (waitForDtpToConnect)
        // don't process any new commands until we are connected
        return true;

#if defined(QFTPPI_DEBUG)
    if (state != Idle)
        qDebug("QFtpPI startNextCmd: Internal error! QFtpPI called in non-Idle state %d", state);
#endif
    if (pendingCommands.isEmpty()) {
        currentCmd.clear();
        emit finished(replyText);
        return false;
    }
    currentCmd = pendingCommands.first();

    // PORT and PASV are edited in-place, depending on whether we
    // should try the extended transfer connection commands EPRT and
    // EPSV. The PORT command also triggers setting up a listener, and
    // the address/port arguments are edited in.
    QHostAddress address = commandSocket.localAddress();
    if (currentCmd.startsWith(QLatin1String("PORT"))) {
        if ((address.protocol() == QTcpSocket::IPv6Protocol) && transferConnectionExtended) {
            int port = dtp.setupListener(address);
            currentCmd = QLatin1String("EPRT |");
            currentCmd += (address.protocol() == QTcpSocket::IPv4Protocol) ? QLatin1Char('1') : QLatin1Char('2');
            currentCmd += QLatin1Char('|') + address.toString() + QLatin1Char('|') + QString::number(port);
            currentCmd += QLatin1Char('|');
        }
        else if (address.protocol() == QTcpSocket::IPv4Protocol) {
            int port = dtp.setupListener(address);
            QString portArg;
            quint32 ip = address.toIPv4Address();
            portArg += QString::number((ip & 0xff000000) >> 24);
            portArg += QLatin1Char(',') + QString::number((ip & 0xff0000) >> 16);
            portArg += QLatin1Char(',') + QString::number((ip & 0xff00) >> 8);
            portArg += QLatin1Char(',') + QString::number(ip & 0xff);
            portArg += QLatin1Char(',') + QString::number((port & 0xff00) >> 8);
            portArg += QLatin1Char(',') + QString::number(port & 0xff);

            currentCmd = QLatin1String("PORT ");
            currentCmd += portArg;
        }
        else {
            // No IPv6 connection can be set up with the PORT
            // command.
            return false;
        }

        currentCmd += QLatin1String("\r\n");
    }
    else if (currentCmd.startsWith(QLatin1String("PASV"))) {
        if ((address.protocol() == QTcpSocket::IPv6Protocol) && transferConnectionExtended)
            currentCmd = QLatin1String("EPSV\r\n");
    }

    pendingCommands.pop_front();
#if defined(QFTPPI_DEBUG)
    qDebug("QFtpPI send: %s", currentCmd.left(currentCmd.length() - 2).toLatin1().constData());
#endif
    state = Waiting;
    commandSocket.write(currentCmd.toLatin1());
    return true;
}

void QFtpPI::dtpConnectState(int s)
{
    switch (s) {
    case QFtpDTP::CsClosed:
        if (waitForDtpToClose) {
            // there is an unprocessed reply
            if (processReply())
                replyText = QLatin1String("");
            else
                return;
        }
        waitForDtpToClose = false;
        readyRead();
        return;
    case QFtpDTP::CsConnected:
        waitForDtpToConnect = false;
        startNextCmd();
        return;
    case QFtpDTP::CsHostNotFound:
    case QFtpDTP::CsConnectionRefused:
        emit error(QFtp::ConnectionRefused,
            QFtp::tr("Connection refused for data connection"));
        startNextCmd();
        return;
    default:
        return;
    }
}
