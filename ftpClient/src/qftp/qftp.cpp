/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QFTPPI_DEBUG
//#define QFTPDTP_DEBUG

#include "qftp.h"
#ifndef QT_NO_FTP

#include "qcoreapplication.h"
#include "qurlinfo.h"
#include "qstringlist.h"
#include "qregexp.h"
#include "qtimer.h"
#include "qfileinfo.h"
#include "qhash.h"
#include "qlocale.h"
#include <QHostAddress>
#include <QTcpSocket>
#include <QTcpServer>
#include "qftpcommand.h"
#include "qftppi.h"
#include "qftpdtp.h"
#include "qftpprivate.h"

QT_BEGIN_NAMESPACE

/**********************************************************************
 *
 * QFtp implementation
 *
 *********************************************************************/
/*!
    \class QFtp
    \brief The QFtp class provides an implementation of the client side of FTP protocol.

    \ingroup network
    \inmodule QtNetwork


    This class provides a direct interface to FTP that allows you to
    have more control over the requests. However, for new
    applications, it is recommended to use QNetworkAccessManager and
    QNetworkReply, as those classes possess a simpler, yet more
    powerful API.

    The class works asynchronously, so there are no blocking
    functions. If an operation cannot be executed immediately, the
    function will still return straight away and the operation will be
    scheduled for later execution. The results of scheduled operations
    are reported via signals. This approach depends on the event loop
    being in operation.

    The operations that can be scheduled (they are called "commands"
    in the rest of the documentation) are the following:
    connectToHost(), login(), close(), list(), cd(), get(), put(),
    remove(), mkdir(), rmdir(), rename() and rawCommand().

    All of these commands return a unique identifier that allows you
    to keep track of the command that is currently being executed.
    When the execution of a command starts, the commandStarted()
    signal with the command's identifier is emitted. When the command
    is finished, the commandFinished() signal is emitted with the
    command's identifier and a bool that indicates whether the command
    finished with an error.

    In some cases, you might want to execute a sequence of commands,
    e.g. if you want to connect and login to a FTP server. This is
    simply achieved:

    \snippet doc/src/snippets/code/src_network_access_qftp.cpp 0

    In this case two FTP commands have been scheduled. When the last
    scheduled command has finished, a done() signal is emitted with
    a bool argument that tells you whether the sequence finished with
    an error.

    If an error occurs during the execution of one of the commands in
    a sequence of commands, all the pending commands (i.e. scheduled,
    but not yet executed commands) are cleared and no signals are
    emitted for them.

    Some commands, e.g. list(), emit additional signals to report
    their results.

    Example: If you want to download the INSTALL file from the Qt
    FTP server, you would write this:

    \snippet doc/src/snippets/code/src_network_access_qftp.cpp 1

    For this example the following sequence of signals is emitted
    (with small variations, depending on network traffic, etc.):

    \snippet doc/src/snippets/code/src_network_access_qftp.cpp 2

    The dataTransferProgress() signal in the above example is useful
    if you want to show a \link QProgressBar progress bar \endlink to
    inform the user about the progress of the download. The
    readyRead() signal tells you that there is data ready to be read.
    The amount of data can be queried then with the bytesAvailable()
    function and it can be read with the read() or readAll()
    function.

    If the login fails for the above example, the signals would look
    like this:

    \snippet doc/src/snippets/code/src_network_access_qftp.cpp 3

    You can then get details about the error with the error() and
    errorString() functions.

    For file transfer, QFtp can use both active or passive mode, and
    it uses passive file transfer mode by default; see the
    documentation for setTransferMode() for more details about this.

    Call setProxy() to make QFtp connect via an FTP proxy server.

    The functions currentId() and currentCommand() provide more
    information about the currently executing command.

    The functions hasPendingCommands() and clearPendingCommands()
    allow you to query and clear the list of pending commands.

    If you are an experienced network programmer and want to have
    complete control you can use rawCommand() to execute arbitrary FTP
    commands.

    \warning The current version of QFtp doesn't fully support
    non-Unix FTP servers.

    \sa QNetworkAccessManager, QNetworkRequest, QNetworkReply,
        {FTP Example}
*/


/*!
    Constructs a QFtp object with the given \a parent.
*/
QFtp::QFtp(QObject *parent)
    : QObject(parent), d(new QFtpPrivate(this))
{
    d->errorString = tr("Unknown error");

    connect(&d->pi, SIGNAL(connectState(int)),
            SLOT(_q_piConnectState(int)));
    connect(&d->pi, SIGNAL(finished(QString)),
            SLOT(_q_piFinished(QString)));
    connect(&d->pi, SIGNAL(error(int,QString)),
            SLOT(_q_piError(int,QString)));
    connect(&d->pi, SIGNAL(rawFtpReply(int,QString)),
            SLOT(_q_piFtpReply(int,QString)));

    connect(&d->pi.dtp, SIGNAL(readyRead()),
            SIGNAL(readyRead()));
    connect(&d->pi.dtp, SIGNAL(dataTransferProgress(qint64,qint64)),
            SIGNAL(dataTransferProgress(qint64,qint64)));
    connect(&d->pi.dtp, SIGNAL(listInfo(QUrlInfo)),
            SIGNAL(listInfo(QUrlInfo)));
}

/*!
    \enum QFtp::State

    This enum defines the connection state:

    \value Unconnected There is no connection to the host.
    \value HostLookup A host name lookup is in progress.
    \value Connecting An attempt to connect to the host is in progress.
    \value Connected Connection to the host has been achieved.
    \value LoggedIn Connection and user login have been achieved.
    \value Closing The connection is closing down, but it is not yet
    closed. (The state will be \c Unconnected when the connection is
    closed.)

    \sa stateChanged() state()
*/
/*!
    \enum QFtp::TransferMode

    FTP works with two socket connections; one for commands and
    another for transmitting data. While the command connection is
    always initiated by the client, the second connection can be
    initiated by either the client or the server.

    This enum defines whether the client (Passive mode) or the server
    (Active mode) should set up the data connection.

    \value Passive The client connects to the server to transmit its
    data.

    \value Active The server connects to the client to transmit its
    data.
*/
/*!
    \enum QFtp::TransferType

    This enum identifies the data transfer type used with get and
    put commands.

    \value Binary The data will be transferred in Binary mode.

    \value Ascii The data will be transferred in Ascii mode and new line
    characters will be converted to the local format.
*/
/*!
    \enum QFtp::Error

    This enum identifies the error that occurred.

    \value NoError No error occurred.
    \value HostNotFound The host name lookup failed.
    \value ConnectionRefused The server refused the connection.
    \value NotConnected Tried to send a command, but there is no connection to
    a server.
    \value UnknownError An error other than those specified above
    occurred.

    \sa error()
*/

/*!
    \enum QFtp::Command

    This enum is used as the return value for the currentCommand() function.
    This allows you to perform specific actions for particular
    commands, e.g. in a FTP client, you might want to clear the
    directory view when a list() command is started; in this case you
    can simply check in the slot connected to the start() signal if
    the currentCommand() is \c List.

    \value None No command is being executed.
    \value SetTransferMode set the \link TransferMode transfer\endlink mode.
    \value SetProxy switch proxying on or off.
    \value ConnectToHost connectToHost() is being executed.
    \value Login login() is being executed.
    \value Close close() is being executed.
    \value List list() is being executed.
    \value Cd cd() is being executed.
    \value Get get() is being executed.
    \value Put put() is being executed.
    \value Remove remove() is being executed.
    \value Mkdir mkdir() is being executed.
    \value Rmdir rmdir() is being executed.
    \value Rename rename() is being executed.
    \value RawCommand rawCommand() is being executed.

    \sa currentCommand()
*/

/*!
    \fn void QFtp::stateChanged(int state)

    This signal is emitted when the state of the connection changes.
    The argument \a state is the new state of the connection; it is
    one of the \l State values.

    It is usually emitted in response to a connectToHost() or close()
    command, but it can also be emitted "spontaneously", e.g. when the
    server closes the connection unexpectedly.

    \sa connectToHost() close() state() State
*/

/*!
    \fn void QFtp::listInfo(const QUrlInfo &i);

    This signal is emitted for each directory entry the list() command
    finds. The details of the entry are stored in \a i.

    \sa list()
*/

/*!
    \fn void QFtp::commandStarted(int id)

    This signal is emitted when processing the command identified by
    \a id starts.

    \sa commandFinished() done()
*/

/*!
    \fn void QFtp::commandFinished(int id, bool error)

    This signal is emitted when processing the command identified by
    \a id has finished. \a error is true if an error occurred during
    the processing; otherwise \a error is false.

    \sa commandStarted() done() error() errorString()
*/

/*!
    \fn void QFtp::done(bool error)

    This signal is emitted when the last pending command has finished;
    (it is emitted after the last command's commandFinished() signal).
    \a error is true if an error occurred during the processing;
    otherwise \a error is false.

    \sa commandFinished() error() errorString()
*/

/*!
    \fn void QFtp::readyRead()

    This signal is emitted in response to a get() command when there
    is new data to read.

    If you specify a device as the second argument in the get()
    command, this signal is \e not emitted; instead the data is
    written directly to the device.

    You can read the data with the readAll() or read() functions.

    This signal is useful if you want to process the data in chunks as
    soon as it becomes available. If you are only interested in the
    complete data, just connect to the commandFinished() signal and
    read the data then instead.

    \sa get() read() readAll() bytesAvailable()
*/

/*!
    \fn void QFtp::dataTransferProgress(qint64 done, qint64 total)

    This signal is emitted in response to a get() or put() request to
    indicate the current progress of the download or upload.

    \a done is the amount of data that has already been transferred
    and \a total is the total amount of data to be read or written. It
    is possible that the QFtp class is not able to determine the total
    amount of data that should be transferred, in which case \a total
    is 0. (If you connect this signal to a QProgressBar, the progress
    bar shows a busy indicator if the total is 0).

    \warning \a done and \a total are not necessarily the size in
    bytes, since for large files these values might need to be
    "scaled" to avoid overflow.

    \sa get(), put(), QProgressBar
*/

/*!
    \fn void QFtp::rawCommandReply(int replyCode, const QString &detail);

    This signal is emitted in response to the rawCommand() function.
    \a replyCode is the 3 digit reply code and \a detail is the text
    that follows the reply code.

    \sa rawCommand()
*/

/*!
    Connects to the FTP server \a host using port \a port.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c HostLookup, then \c
    Connecting, then \c Connected.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa stateChanged() commandStarted() commandFinished()
*/
int QFtp::connectToHost(const QString &host, quint16 port)
{
    QStringList cmds;
    cmds << host;
    cmds << QString::number((uint)port);
    int id = d->addCommand(new QFtpCommand(ConnectToHost, cmds));
    d->pi.transferConnectionExtended = true;
    return id;
}

/*!
    Logs in to the FTP server with the username \a user and the
    password \a password.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c LoggedIn.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int QFtp::login(const QString &user, const QString &password)
{
    QStringList cmds;
    cmds << (QLatin1String("USER ") + (user.isNull() ? QLatin1String("anonymous") : user) + QLatin1String("\r\n"));
    cmds << (QLatin1String("PASS ") + (password.isNull() ? QLatin1String("anonymous@") : password) + QLatin1String("\r\n"));
    return d->addCommand(new QFtpCommand(Login, cmds));
}

/*!
    Closes the connection to the FTP server.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c Closing, then \c
    Unconnected.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa stateChanged() commandStarted() commandFinished()
*/
int QFtp::close()
{
    return d->addCommand(new QFtpCommand(Close, QStringList(QLatin1String("QUIT\r\n"))));
}

/*!
    Sets the current FTP transfer mode to \a mode. The default is QFtp::Passive.

    \sa QFtp::TransferMode
*/
int QFtp::setTransferMode(TransferMode mode)
{
    int id = d->addCommand(new QFtpCommand(SetTransferMode, QStringList()));
    d->pi.transferConnectionExtended = true;
    d->transferMode = mode;
    return id;
}

/*!
    Enables use of the FTP proxy on host \a host and port \a
    port. Calling this function with \a host empty disables proxying.

    QFtp does not support FTP-over-HTTP proxy servers. Use
    QNetworkAccessManager for this.
*/
int QFtp::setProxy(const QString &host, quint16 port)
{
    QStringList args;
    args << host << QString::number(port);
    return d->addCommand(new QFtpCommand(SetProxy, args));
}

/*!
    Lists the contents of directory \a dir on the FTP server. If \a
    dir is empty, it lists the contents of the current directory.

    The listInfo() signal is emitted for each directory entry found.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa listInfo() commandStarted() commandFinished()
*/
int QFtp::list(const QString &dir)
{
    QStringList cmds;
    cmds << QLatin1String("TYPE A\r\n");
    cmds << QLatin1String(d->transferMode == Passive ? "PASV\r\n" : "PORT\r\n");
    if (dir.isEmpty())
        cmds << QLatin1String("LIST\r\n");
    else
        cmds << (QLatin1String("LIST ") + dir + QLatin1String("\r\n"));
    return d->addCommand(new QFtpCommand(List, cmds));
}

/*!
    Changes the working directory of the server to \a dir.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int QFtp::cd(const QString &dir)
{
    return d->addCommand(new QFtpCommand(Cd, QStringList(QLatin1String("CWD ") + dir + QLatin1String("\r\n"))));
}

/*!
    Downloads the file \a file from the server.

    If \a dev is 0, then the readyRead() signal is emitted when there
    is data available to read. You can then read the data with the
    read() or readAll() functions.

    If \a dev is not 0, the data is written directly to the device \a
    dev. Make sure that the \a dev pointer is valid for the duration
    of the operation (it is safe to delete it when the
    commandFinished() signal is emitted). In this case the readyRead()
    signal is \e not emitted and you cannot read data with the
    read() or readAll() functions.

    If you don't read the data immediately it becomes available, i.e.
    when the readyRead() signal is emitted, it is still available
    until the next command is started.

    For example, if you want to present the data to the user as soon
    as there is something available, connect to the readyRead() signal
    and read the data immediately. On the other hand, if you only want
    to work with the complete data, you can connect to the
    commandFinished() signal and read the data when the get() command
    is finished.

    The data is transferred as Binary or Ascii depending on the value
    of \a type.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa readyRead() dataTransferProgress() commandStarted()
    commandFinished()
*/
int QFtp::get(const QString &file, QIODevice *dev, TransferType type)
{
    QStringList cmds;
    if (type == Binary)
        cmds << QLatin1String("TYPE I\r\n");
    else
        cmds << QLatin1String("TYPE A\r\n");
    cmds << QLatin1String("SIZE ") + file + QLatin1String("\r\n");
    cmds << QLatin1String(d->transferMode == Passive ? "PASV\r\n" : "PORT\r\n");
    cmds << QLatin1String("RETR ") + file + QLatin1String("\r\n");
    return d->addCommand(new QFtpCommand(Get, cmds, dev));
}

/*!
    \overload

    Writes a copy of the given \a data to the file called \a file on
    the server. The progress of the upload is reported by the
    dataTransferProgress() signal.

    The data is transferred as Binary or Ascii depending on the value
    of \a type.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    Since this function takes a copy of the \a data, you can discard
    your own copy when this function returns.

    \sa dataTransferProgress() commandStarted() commandFinished()
*/
int QFtp::put(const QByteArray &data, const QString &file, TransferType type)
{
    QStringList cmds;
    if (type == Binary)
        cmds << QLatin1String("TYPE I\r\n");
    else
        cmds << QLatin1String("TYPE A\r\n");
    cmds << QLatin1String(d->transferMode == Passive ? "PASV\r\n" : "PORT\r\n");
    cmds << QLatin1String("ALLO ") + QString::number(data.size()) + QLatin1String("\r\n");
    cmds << QLatin1String("STOR ") + file + QLatin1String("\r\n");
    return d->addCommand(new QFtpCommand(Put, cmds, data));
}

/*!
    Reads the data from the IO device \a dev, and writes it to the
    file called \a file on the server. The data is read in chunks from
    the IO device, so this overload allows you to transmit large
    amounts of data without the need to read all the data into memory
    at once.

    The data is transferred as Binary or Ascii depending on the value
    of \a type.

    Make sure that the \a dev pointer is valid for the duration of the
    operation (it is safe to delete it when the commandFinished() is
    emitted).
*/
int QFtp::put(QIODevice *dev, const QString &file, TransferType type)
{
    QStringList cmds;
    if (type == Binary)
        cmds << QLatin1String("TYPE I\r\n");
    else
        cmds << QLatin1String("TYPE A\r\n");
    cmds << QLatin1String(d->transferMode == Passive ? "PASV\r\n" : "PORT\r\n");
    if (!dev->isSequential())
        cmds << QLatin1String("ALLO ") + QString::number(dev->size()) + QLatin1String("\r\n");
    cmds << QLatin1String("STOR ") + file + QLatin1String("\r\n");
    return d->addCommand(new QFtpCommand(Put, cmds, dev));
}

/*!
    Deletes the file called \a file from the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int QFtp::remove(const QString &file)
{
    return d->addCommand(new QFtpCommand(Remove, QStringList(QLatin1String("DELE ") + file + QLatin1String("\r\n"))));
}

/*!
    Creates a directory called \a dir on the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int QFtp::mkdir(const QString &dir)
{
    return d->addCommand(new QFtpCommand(Mkdir, QStringList(QLatin1String("MKD ") + dir + QLatin1String("\r\n"))));
}

/*!
    Removes the directory called \a dir from the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int QFtp::rmdir(const QString &dir)
{
    return d->addCommand(new QFtpCommand(Rmdir, QStringList(QLatin1String("RMD ") + dir + QLatin1String("\r\n"))));
}

/*!
    Renames the file called \a oldname to \a newname on the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int QFtp::rename(const QString &oldname, const QString &newname)
{
    QStringList cmds;
    cmds << QLatin1String("RNFR ") + oldname + QLatin1String("\r\n");
    cmds << QLatin1String("RNTO ") + newname + QLatin1String("\r\n");
    return d->addCommand(new QFtpCommand(Rename, cmds));
}

/*!
    Sends the raw FTP command \a command to the FTP server. This is
    useful for low-level FTP access. If the operation you wish to
    perform has an equivalent QFtp function, we recommend using the
    function instead of raw FTP commands since the functions are
    easier and safer.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa rawCommandReply() commandStarted() commandFinished()
*/
int QFtp::rawCommand(const QString &command)
{
    QString cmd = command.trimmed() + QLatin1String("\r\n");
    return d->addCommand(new QFtpCommand(RawCommand, QStringList(cmd)));
}

/*!
    Returns the number of bytes that can be read from the data socket
    at the moment.

    \sa get() readyRead() read() readAll()
*/
qint64 QFtp::bytesAvailable() const
{
    return d->pi.dtp.bytesAvailable();
}

/*! \fn qint64 QFtp::readBlock(char *data, quint64 maxlen)

    Use read() instead.
*/

/*!
    Reads \a maxlen bytes from the data socket into \a data and
    returns the number of bytes read. Returns -1 if an error occurred.

    \sa get() readyRead() bytesAvailable() readAll()
*/
qint64 QFtp::read(char *data, qint64 maxlen)
{
    return d->pi.dtp.read(data, maxlen);
}

/*!
    Reads all the bytes available from the data socket and returns
    them.

    \sa get() readyRead() bytesAvailable() read()
*/
QByteArray QFtp::readAll()
{
    return d->pi.dtp.readAll();
}

/*!
    Aborts the current command and deletes all scheduled commands.

    If there is an unfinished command (i.e. a command for which the
    commandStarted() signal has been emitted, but for which the
    commandFinished() signal has not been emitted), this function
    sends an \c ABORT command to the server. When the server replies
    that the command is aborted, the commandFinished() signal with the
    \c error argument set to \c true is emitted for the command. Due
    to timing issues, it is possible that the command had already
    finished before the abort request reached the server, in which
    case, the commandFinished() signal is emitted with the \c error
    argument set to \c false.

    For all other commands that are affected by the abort(), no
    signals are emitted.

    If you don't start further FTP commands directly after the
    abort(), there won't be any scheduled commands and the done()
    signal is emitted.

    \warning Some FTP servers, for example the BSD FTP daemon (version
    0.3), wrongly return a positive reply even when an abort has
    occurred. For these servers the commandFinished() signal has its
    error flag set to \c false, even though the command did not
    complete successfully.

    \sa clearPendingCommands()
*/
void QFtp::abort()
{
    if (d->pending.isEmpty())
        return;

    clearPendingCommands();
    d->pi.abort();
}

/*!
    Returns the identifier of the FTP command that is being executed
    or 0 if there is no command being executed.

    \sa currentCommand()
*/
int QFtp::currentId() const
{
    if (d->pending.isEmpty())
        return 0;
    return d->pending.first()->id;
}

/*!
    Returns the command type of the FTP command being executed or \c
    None if there is no command being executed.

    \sa currentId()
*/
QFtp::Command QFtp::currentCommand() const
{
    if (d->pending.isEmpty())
        return None;
    return d->pending.first()->command;
}

/*!
    Returns the QIODevice pointer that is used by the FTP command to read data
    from or store data to. If there is no current FTP command being executed or
    if the command does not use an IO device, this function returns 0.

    This function can be used to delete the QIODevice in the slot connected to
    the commandFinished() signal.

    \sa get() put()
*/
QIODevice* QFtp::currentDevice() const
{
    if (d->pending.isEmpty())
        return 0;
    QFtpCommand *c = d->pending.first();
    if (c->is_ba)
        return 0;
    return c->data.dev;
}

/*!
    Returns true if there are any commands scheduled that have not yet
    been executed; otherwise returns false.

    The command that is being executed is \e not considered as a
    scheduled command.

    \sa clearPendingCommands() currentId() currentCommand()
*/
bool QFtp::hasPendingCommands() const
{
    return d->pending.count() > 1;
}

/*!
    Deletes all pending commands from the list of scheduled commands.
    This does not affect the command that is being executed. If you
    want to stop this as well, use abort().

    \sa hasPendingCommands() abort()
*/
void QFtp::clearPendingCommands()
{
    // delete all entires except the first one
    while (d->pending.count() > 1)
        delete d->pending.takeLast();
}

/*!
    Returns the current state of the object. When the state changes,
    the stateChanged() signal is emitted.

    \sa State stateChanged()
*/
QFtp::State QFtp::state() const
{
    return d->state;
}

/*!
    Returns the last error that occurred. This is useful to find out
    what went wrong when receiving a commandFinished() or a done()
    signal with the \c error argument set to \c true.

    If you start a new command, the error status is reset to \c NoError.
*/
QFtp::Error QFtp::error() const
{
    return d->error;
}

/*!
    Returns a human-readable description of the last error that
    occurred. This is useful for presenting a error message to the
    user when receiving a commandFinished() or a done() signal with
    the \c error argument set to \c true.

    The error string is often (but not always) the reply from the
    server, so it is not always possible to translate the string. If
    the message comes from Qt, the string has already passed through
    tr().
*/
QString QFtp::errorString() const
{
    return d->errorString;
}

/*!
    Destructor.
*/
QFtp::~QFtp()
{
    abort();
    close();
}

QT_END_NAMESPACE

#include "qftp.moc"

#include "moc_qftp.cpp"

#endif // QT_NO_FTP
