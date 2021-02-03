#ifndef QFTPPRIVATE_H
#define QFTPPRIVATE_H
#include "qftp.h"
#include "qftpcommand.h"
#include "qftppi.h"

class QFtpPrivate
{
	Q_DECLARE_PUBLIC(QFtp)
public:

	inline QFtpPrivate(QFtp* owner) : close_waitForStateChange(false), state(QFtp::Unconnected),
		transferMode(QFtp::Passive), error(QFtp::NoError), q_ptr(owner)
	{ }

	~QFtpPrivate() { while (!pending.isEmpty()) delete pending.takeFirst(); }

	// private slots
	void _q_startNextCommand();
	void _q_piFinished(const QString&);
	void _q_piError(int, const QString&);
	void _q_piConnectState(int);
	void _q_piFtpReply(int, const QString&);

	int addCommand(QFtpCommand* cmd);

	QFtpPI pi;
	QList<QFtpCommand*> pending;
	bool close_waitForStateChange;
	QFtp::State state;
	QFtp::TransferMode transferMode;
	QFtp::Error error;
	QString errorString;

	QString host;
	quint16 port;
	QString proxyHost;
	quint16 proxyPort;
	QFtp* q_ptr;
};

#endif // QFTPPRIVATE_H
