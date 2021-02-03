#include "qftpcommand.h"
#include <QtCore/qbasicatomic.h>

QBasicAtomicInt QFtpCommand::idCounter = Q_BASIC_ATOMIC_INITIALIZER(1);

QFtpCommand::QFtpCommand(QFtp::Command cmd, QStringList raw, const QByteArray& ba)
	: command(cmd), rawCmds(raw), is_ba(true)
{
	id = idCounter.fetchAndAddRelaxed(1);
	data.ba = new QByteArray(ba);
}

QFtpCommand::QFtpCommand(QFtp::Command cmd, QStringList raw, QIODevice* dev)
	: command(cmd), rawCmds(raw), is_ba(false)
{
	id = idCounter.fetchAndAddRelaxed(1);
	data.dev = dev;
}

QFtpCommand::~QFtpCommand()
{
	if (is_ba)
		delete data.ba;
}
