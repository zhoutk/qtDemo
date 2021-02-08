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
	connect(&d->pi.dtp, SIGNAL(listInfos(QVector<QUrlInfo>)),
		SIGNAL(listInfos(QVector<QUrlInfo>)));
}

int QFtp::connectToHost(const QString &host, quint16 port)
{
    QStringList cmds;
    cmds << host;
    cmds << QString::number((uint)port);
    int id = d->addCommand(new QFtpCommand(ConnectToHost, cmds));
    d->pi.transferConnectionExtended = true;
    return id;
}

int QFtp::login(const QString &user, const QString &password)
{
    QStringList cmds;
    cmds << (QLatin1String("USER ") + (user.isNull() ? QLatin1String("anonymous") : user) + QLatin1String("\r\n"));
    cmds << (QLatin1String("PASS ") + (password.isNull() ? QLatin1String("anonymous@") : password) + QLatin1String("\r\n"));
    return d->addCommand(new QFtpCommand(Login, cmds));
}

int QFtp::close()
{
    return d->addCommand(new QFtpCommand(Close, QStringList(QLatin1String("QUIT\r\n"))));
}

int QFtp::setTransferMode(TransferMode mode)
{
    int id = d->addCommand(new QFtpCommand(SetTransferMode, QStringList()));
    d->pi.transferConnectionExtended = true;
    d->transferMode = mode;
    return id;
}

int QFtp::setProxy(const QString &host, quint16 port)
{
    QStringList args;
    args << host << QString::number(port);
    return d->addCommand(new QFtpCommand(SetProxy, args));
}

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

int QFtp::cd(const QString &dir)
{
    return d->addCommand(new QFtpCommand(Cd, QStringList(QLatin1String("CWD ") + dir + QLatin1String("\r\n"))));
}

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

int QFtp::remove(const QString &file)
{
    return d->addCommand(new QFtpCommand(Remove, QStringList(QLatin1String("DELE ") + file + QLatin1String("\r\n"))));
}

int QFtp::mkdir(const QString &dir)
{
    return d->addCommand(new QFtpCommand(Mkdir, QStringList(QLatin1String("MKD ") + dir + QLatin1String("\r\n"))));
}

int QFtp::rmdir(const QString &dir)
{
    return d->addCommand(new QFtpCommand(Rmdir, QStringList(QLatin1String("RMD ") + dir + QLatin1String("\r\n"))));
}

int QFtp::rename(const QString &oldname, const QString &newname)
{
    QStringList cmds;
    cmds << QLatin1String("RNFR ") + oldname + QLatin1String("\r\n");
    cmds << QLatin1String("RNTO ") + newname + QLatin1String("\r\n");
    return d->addCommand(new QFtpCommand(Rename, cmds));
}

int QFtp::rawCommand(const QString &command)
{
    QString cmd = command.trimmed() + QLatin1String("\r\n");
    return d->addCommand(new QFtpCommand(RawCommand, QStringList(cmd)));
}

qint64 QFtp::bytesAvailable() const
{
    return d->pi.dtp.bytesAvailable();
}

qint64 QFtp::read(char *data, qint64 maxlen)
{
    return d->pi.dtp.read(data, maxlen);
}

QByteArray QFtp::readAll()
{
    return d->pi.dtp.readAll();
}

void QFtp::abort()
{
    if (d->pending.isEmpty())
        return;

    clearPendingCommands();
    d->pi.abort();
}

int QFtp::currentId() const
{
    if (d->pending.isEmpty())
        return 0;
    return d->pending.first()->id;
}

QFtp::Command QFtp::currentCommand() const
{
    if (d->pending.isEmpty())
        return None;
    return d->pending.first()->command;
}

QIODevice* QFtp::currentDevice() const
{
    if (d->pending.isEmpty())
        return 0;
    QFtpCommand *c = d->pending.first();
    if (c->is_ba)
        return 0;
    return c->data.dev;
}

bool QFtp::hasPendingCommands() const
{
    return d->pending.count() > 1;
}

void QFtp::clearPendingCommands()
{
    // delete all entires except the first one
    while (d->pending.count() > 1)
        delete d->pending.takeLast();
}

QFtp::State QFtp::state() const
{
    return d->state;
}

QFtp::Error QFtp::error() const
{
    return d->error;
}

QString QFtp::errorString() const
{
    return d->errorString;
}

QFtp::~QFtp()
{
    abort();
    close();
}

#include "qftp.moc"

#include "moc_qftp.cpp"

#endif // QT_NO_FTP
