#include <QTimer>
#include "qftpprivate.h"

int QFtpPrivate::addCommand(QFtpCommand* cmd)
{
	pending.append(cmd);

	if (pending.count() == 1) {
		// don't emit the commandStarted() signal before the ID is returned
		QTimer::singleShot(0, q_func(), SLOT(_q_startNextCommand()));
	}
	return cmd->id;
}

void QFtpPrivate::_q_startNextCommand()
{
    Q_Q(QFtp);
    if (pending.isEmpty())
        return;
    QFtpCommand* c = pending.first();

    error = QFtp::NoError;
    errorString = QT_TRANSLATE_NOOP(QFtp, QLatin1String("Unknown error"));

    if (q->bytesAvailable())
        q->readAll(); // clear the data
    emit q->commandStarted(c->id);

    // Proxy support, replace the Login argument in place, then fall
    // through.
    if (c->command == QFtp::Login && !proxyHost.isEmpty()) {
        QString loginString = c->rawCmds.first().trimmed();
        loginString += QLatin1Char('@') + host;
        if (port && port != 21)
            loginString += QLatin1Char(':') + QString::number(port);
        loginString += QLatin1String("\r\n");
        c->rawCmds[0] = loginString;
    }

    if (c->command == QFtp::SetTransferMode) {
        _q_piFinished(QLatin1String("Transfer mode set"));
    }
    else if (c->command == QFtp::SetProxy) {
        proxyHost = c->rawCmds[0];
        proxyPort = c->rawCmds[1].toUInt();
        c->rawCmds.clear();
        _q_piFinished(QLatin1String("Proxy set to ") + proxyHost + QLatin1Char(':') + QString::number(proxyPort));
    }
    else if (c->command == QFtp::ConnectToHost) {
#ifndef QT_NO_BEARERMANAGEMENT
        //copy network session down to the PI
        pi.setProperty("_q_networksession", q->property("_q_networksession"));
#endif
        if (!proxyHost.isEmpty()) {
            host = c->rawCmds[0];
            port = c->rawCmds[1].toUInt();
            pi.connectToHost(proxyHost, proxyPort);
        }
        else {
            pi.connectToHost(c->rawCmds[0], c->rawCmds[1].toUInt());
        }
    }
    else {
        if (c->command == QFtp::Put) {
            if (c->is_ba) {
                pi.dtp.setData(c->data.ba);
                pi.dtp.setBytesTotal(c->data.ba->size());
            }
            else if (c->data.dev && (c->data.dev->isOpen() || c->data.dev->open(QIODevice::ReadOnly))) {
                pi.dtp.setDevice(c->data.dev);
                if (c->data.dev->isSequential()) {
                    pi.dtp.setBytesTotal(0);
                    pi.dtp.connect(c->data.dev, SIGNAL(readyRead()), SLOT(dataReadyRead()));
                    pi.dtp.connect(c->data.dev, SIGNAL(readChannelFinished()), SLOT(dataReadyRead()));
                }
                else {
                    pi.dtp.setBytesTotal(c->data.dev->size());
                }
            }
        }
        else if (c->command == QFtp::Get) {
            if (!c->is_ba && c->data.dev) {
                pi.dtp.setDevice(c->data.dev);
            }
        }
        else if (c->command == QFtp::Close) {
            state = QFtp::Closing;
            emit q->stateChanged(state);
        }
        pi.sendCommands(c->rawCmds);
    }
}

/*! \internal
*/
void QFtpPrivate::_q_piFinished(const QString&)
{
    if (pending.isEmpty())
        return;
    QFtpCommand* c = pending.first();

    if (c->command == QFtp::Close) {
        // The order of in which the slots are called is arbitrary, so
        // disconnect the SIGNAL-SIGNAL temporary to make sure that we
        // don't get the commandFinished() signal before the stateChanged()
        // signal.
        if (state != QFtp::Unconnected) {
            close_waitForStateChange = true;
            return;
        }
    }
    emit q_func()->commandFinished(c->id, false);
    pending.removeFirst();

    delete c;

    if (pending.isEmpty()) {
        emit q_func()->done(false);
    }
    else {
        _q_startNextCommand();
    }
}

/*! \internal
*/
void QFtpPrivate::_q_piError(int errorCode, const QString& text)
{
    Q_Q(QFtp);

    if (pending.isEmpty()) {
        qWarning("QFtpPrivate::_q_piError was called without pending command!");
        return;
    }

    QFtpCommand* c = pending.first();

    // non-fatal errors
    if (c->command == QFtp::Get && pi.currentCommand().startsWith(QLatin1String("SIZE "))) {
        pi.dtp.setBytesTotal(0);
        return;
    }
    else if (c->command == QFtp::Put && pi.currentCommand().startsWith(QLatin1String("ALLO "))) {
        return;
    }

    error = QFtp::Error(errorCode);
    switch (q->currentCommand()) {
    case QFtp::ConnectToHost:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Connecting to host failed:\n%1"))
            .arg(text);
        break;
    case QFtp::Login:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Login failed:\n%1"))
            .arg(text);
        break;
    case QFtp::List:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Listing directory failed:\n%1"))
            .arg(text);
        break;
    case QFtp::Cd:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Changing directory failed:\n%1"))
            .arg(text);
        break;
    case QFtp::Get:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Downloading file failed:\n%1"))
            .arg(text);
        break;
    case QFtp::Put:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Uploading file failed:\n%1"))
            .arg(text);
        break;
    case QFtp::Remove:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Removing file failed:\n%1"))
            .arg(text);
        break;
    case QFtp::Mkdir:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Creating directory failed:\n%1"))
            .arg(text);
        break;
    case QFtp::Rmdir:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QFtp", "Removing directory failed:\n%1"))
            .arg(text);
        break;
    default:
        errorString = text;
        break;
    }

    pi.clearPendingCommands();
    q->clearPendingCommands();
    emit q->commandFinished(c->id, true);

    pending.removeFirst();
    delete c;
    if (pending.isEmpty())
        emit q->done(true);
    else
        _q_startNextCommand();
}

/*! \internal
*/
void QFtpPrivate::_q_piConnectState(int connectState)
{
    state = QFtp::State(connectState);
    emit q_func()->stateChanged(state);
    if (close_waitForStateChange) {
        close_waitForStateChange = false;
        _q_piFinished(QLatin1String(QT_TRANSLATE_NOOP("QFtp", "Connection closed")));
    }
}

/*! \internal
*/
void QFtpPrivate::_q_piFtpReply(int code, const QString& text)
{
    if (q_func()->currentCommand() == QFtp::RawCommand) {
        pi.rawCommand = true;
        emit q_func()->rawCommandReply(code, text);
    }
}

