#ifndef QFTPPI_H
#define QFTPPI_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include "qftp.h"
#include "qftpdtp.h"

class QFtpPI : public QObject
{
	Q_OBJECT

public:
	QFtpPI(QObject* parent = 0);

	void connectToHost(const QString& host, quint16 port);

	bool sendCommands(const QStringList& cmds);
	bool sendCommand(const QString& cmd)
	{
		return sendCommands(QStringList(cmd));
	}

	void clearPendingCommands();
	void abort();

	QString currentCommand() const
	{
		return currentCmd;
	}

	bool rawCommand;
	bool transferConnectionExtended;

	QFtpDTP dtp; // the PI has a DTP which is not the design of RFC 959, but it
				 // makes the design simpler this way
signals:
	void connectState(int);
	void finished(const QString&);
	void error(int, const QString&);
	void rawFtpReply(int, const QString&);

private slots:
	void hostFound();
	void connected();
	void connectionClosed();
	void delayedCloseFinished();
	void readyRead();
	void error(QAbstractSocket::SocketError);

	void dtpConnectState(int);

private:
	// the states are modelled after the generalized state diagram of RFC 959,
	// page 58
	enum State {
		Begin,
		Idle,
		Waiting,
		Success,
		Failure
	};

	enum AbortState {
		None,
		AbortStarted,
		WaitForAbortToFinish
	};

	bool processReply();
	bool startNextCmd();

	QTcpSocket commandSocket;
	QString replyText;
	char replyCode[3];
	State state;
	AbortState abortState;
	QStringList pendingCommands;
	QString currentCmd;

	bool waitForDtpToConnect;
	bool waitForDtpToClose;

	QByteArray bytesFromSocket;

	friend class QFtpDTP;
};

#endif // QFTPPI_H
