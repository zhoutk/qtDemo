#ifndef QFTPCOMMAND_H
#define QFTPCOMMAND_H

#include "qftp.h"

class QFtpCommand
{
public:
	QFtpCommand(QFtp::Command cmd, QStringList raw, const QByteArray& ba);
	QFtpCommand(QFtp::Command cmd, QStringList raw, QIODevice* dev = 0);
	~QFtpCommand();

	int id;
	QFtp::Command command;
	QStringList rawCmds;

	// If is_ba is true, ba is used; ba is never 0.
	// Otherwise dev is used; dev can be 0 or not.
	union {
		QByteArray* ba;
		QIODevice* dev;
	} data;
	bool is_ba;

	static QBasicAtomicInt idCounter;
};

#endif // QFTPCOMMAND_H
