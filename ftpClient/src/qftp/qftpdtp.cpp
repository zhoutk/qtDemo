#include "qftpdtp.h"
#include "qftppi.h"

static void _q_fixupDateTime(QDateTime* dateTime)
{
	// Adjust for future tolerance.
	const int futureTolerance = 86400;
	if (dateTime->secsTo(QDateTime::currentDateTime()) < -futureTolerance) {
		QDate d = dateTime->date();
		d.setDate(d.year() - 1, d.month(), d.day());
		dateTime->setDate(d);
	}
}

static void _q_parseUnixDir(const QStringList& tokens, const QString& userName, QUrlInfo* info)
{
	// Unix style, 7 + 1 entries
	// -rw-r--r--    1 ftp      ftp      17358091 Aug 10  2004 qt-x11-free-3.3.3.tar.gz
	// drwxr-xr-x    3 ftp      ftp          4096 Apr 14  2000 compiled-examples
	// lrwxrwxrwx    1 ftp      ftp             9 Oct 29  2005 qtscape -> qtmozilla
	if (tokens.size() != 8)
		return;

	char first = tokens.at(1).at(0).toLatin1();
	if (first == 'd') {
		info->setDir(true);
		info->setFile(false);
		info->setSymLink(false);
	}
	else if (first == '-') {
		info->setDir(false);
		info->setFile(true);
		info->setSymLink(false);
	}
	else if (first == 'l') {
		info->setDir(true);
		info->setFile(false);
		info->setSymLink(true);
	}

	// Resolve filename
	QString name = tokens.at(7);
	if (info->isSymLink()) {
		int linkPos = name.indexOf(QLatin1String(" ->"));
		if (linkPos != -1)
			name.resize(linkPos);
	}
	info->setName(name);

	// Resolve owner & group
	info->setOwner(tokens.at(3));
	info->setGroup(tokens.at(4));

	// Resolve size
	info->setSize(tokens.at(5).toLongLong());

	QStringList formats;
	formats << QLatin1String("MMM dd  yyyy") << QLatin1String("MMM dd hh:mm") << QLatin1String("MMM  d  yyyy")
		<< QLatin1String("MMM  d hh:mm") << QLatin1String("MMM  d yyyy") << QLatin1String("MMM dd yyyy");

	QString dateString = tokens.at(6);
	dateString[0] = dateString[0].toUpper();

	// Resolve the modification date by parsing all possible formats
	QDateTime dateTime;
	int n = 0;
#ifndef QT_NO_DATESTRING
	do {
		dateTime = QLocale::c().toDateTime(dateString, formats.at(n++));
	} while (n < formats.size() && (!dateTime.isValid()));
#endif

	if (n == 2 || n == 4) {
		// Guess the year.
		dateTime.setDate(QDate(QDate::currentDate().year(),
			dateTime.date().month(),
			dateTime.date().day()));
		_q_fixupDateTime(&dateTime);
	}
	if (dateTime.isValid())
		info->setLastModified(dateTime);

	// Resolve permissions
	int permissions = 0;
	QString p = tokens.at(2);
	permissions |= (p[0] == QLatin1Char('r') ? QUrlInfo::ReadOwner : 0);
	permissions |= (p[1] == QLatin1Char('w') ? QUrlInfo::WriteOwner : 0);
	permissions |= (p[2] == QLatin1Char('x') ? QUrlInfo::ExeOwner : 0);
	permissions |= (p[3] == QLatin1Char('r') ? QUrlInfo::ReadGroup : 0);
	permissions |= (p[4] == QLatin1Char('w') ? QUrlInfo::WriteGroup : 0);
	permissions |= (p[5] == QLatin1Char('x') ? QUrlInfo::ExeGroup : 0);
	permissions |= (p[6] == QLatin1Char('r') ? QUrlInfo::ReadOther : 0);
	permissions |= (p[7] == QLatin1Char('w') ? QUrlInfo::WriteOther : 0);
	permissions |= (p[8] == QLatin1Char('x') ? QUrlInfo::ExeOther : 0);
	info->setPermissions(permissions);

	bool isOwner = info->owner() == userName;
	info->setReadable((permissions & QUrlInfo::ReadOther) || ((permissions & QUrlInfo::ReadOwner) && isOwner));
	info->setWritable((permissions & QUrlInfo::WriteOther) || ((permissions & QUrlInfo::WriteOwner) && isOwner));
}

static void _q_parseDosDir(const QStringList& tokens, const QString& userName, QUrlInfo* info)
{
	// DOS style, 3 + 1 entries
	// 01-16-02  11:14AM       <DIR>          epsgroup
	// 06-05-03  03:19PM                 1973 readme.txt
	if (tokens.size() != 4)
		return;

	Q_UNUSED(userName);

	QString name = tokens.at(3);
	info->setName(name);
	info->setSymLink(name.toLower().endsWith(QLatin1String(".lnk")));

	if (tokens.at(2) == QLatin1String("<DIR>")) {
		info->setFile(false);
		info->setDir(true);
	}
	else {
		info->setFile(true);
		info->setDir(false);
		info->setSize(tokens.at(2).toLongLong());
	}

	// Note: We cannot use QFileInfo; permissions are for the server-side
	// machine, and QFileInfo's behavior depends on the local platform.
	int permissions = QUrlInfo::ReadOwner | QUrlInfo::WriteOwner
		| QUrlInfo::ReadGroup | QUrlInfo::WriteGroup
		| QUrlInfo::ReadOther | QUrlInfo::WriteOther;
	QString ext;
	int extIndex = name.lastIndexOf(QLatin1Char('.'));
	if (extIndex != -1)
		ext = name.mid(extIndex + 1);
	if (ext == QLatin1String("exe") || ext == QLatin1String("bat") || ext == QLatin1String("com"))
		permissions |= QUrlInfo::ExeOwner | QUrlInfo::ExeGroup | QUrlInfo::ExeOther;
	info->setPermissions(permissions);

	info->setReadable(true);
	info->setWritable(info->isFile());

	QDateTime dateTime;
#ifndef QT_NO_DATESTRING
	dateTime = QLocale::c().toDateTime(tokens.at(1), QLatin1String("MM-dd-yy  hh:mmAP"));
	if (dateTime.date().year() < 1971) {
		dateTime.setDate(QDate(dateTime.date().year() + 100,
			dateTime.date().month(),
			dateTime.date().day()));
	}
#endif

	info->setLastModified(dateTime);

}

QFtpDTP::QFtpDTP(QFtpPI* p, QObject* parent) :
	QObject(parent),
	socket(0),
	listener(this),
	pi(p),
	callWriteData(false)
{
	clearData();
	listener.setObjectName(QLatin1String("QFtpDTP active state server"));
	connect(&listener, SIGNAL(newConnection()), SLOT(setupSocket()));
}

void QFtpDTP::setData(QByteArray* ba)
{
	is_ba = true;
	data.ba = ba;
}

void QFtpDTP::setDevice(QIODevice* dev)
{
	is_ba = false;
	data.dev = dev;
}

void QFtpDTP::setBytesTotal(qint64 bytes)
{
	bytesTotal = bytes;
	bytesDone = 0;
	emit dataTransferProgress(bytesDone, bytesTotal);
}

void QFtpDTP::connectToHost(const QString& host, quint16 port)
{
	bytesFromSocket.clear();

	if (socket) {
		delete socket;
		socket = 0;
	}
	socket = new QTcpSocket(this);
#ifndef QT_NO_BEARERMANAGEMENT
	//copy network session down to the socket
	socket->setProperty("_q_networksession", property("_q_networksession"));
#endif
	socket->setObjectName(QLatin1String("QFtpDTP Passive state socket"));
	connect(socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(socket, SIGNAL(disconnected()), SLOT(socketConnectionClosed()));
	connect(socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));

	socket->connectToHost(host, port);
}

int QFtpDTP::setupListener(const QHostAddress& address)
{
#ifndef QT_NO_BEARERMANAGEMENT
	//copy network session down to the socket
	listener.setProperty("_q_networksession", property("_q_networksession"));
#endif
	if (!listener.isListening() && !listener.listen(address, 0))
		return -1;
	return listener.serverPort();
}

void QFtpDTP::waitForConnection()
{
	// This function is only interesting in Active transfer mode; it works
	// around a limitation in QFtp's design by blocking, waiting for an
	// incoming connection. For the default Passive mode, it does nothing.
	if (listener.isListening())
		listener.waitForNewConnection();
}

QTcpSocket::SocketState QFtpDTP::state() const
{
	return socket ? socket->state() : QTcpSocket::UnconnectedState;
}

qint64 QFtpDTP::bytesAvailable() const
{
	if (!socket || socket->state() != QTcpSocket::ConnectedState)
		return (qint64)bytesFromSocket.size();
	return socket->bytesAvailable();
}

qint64 QFtpDTP::read(char* data, qint64 maxlen)
{
	qint64 read;
	if (socket && socket->state() == QTcpSocket::ConnectedState) {
		read = socket->read(data, maxlen);
	}
	else {
		read = qMin(maxlen, qint64(bytesFromSocket.size()));
		memcpy(data, bytesFromSocket.data(), read);
		bytesFromSocket.remove(0, read);
	}

	bytesDone += read;
	return read;
}

QByteArray QFtpDTP::readAll()
{
	QByteArray tmp;
	if (socket && socket->state() == QTcpSocket::ConnectedState) {
		tmp = socket->readAll();
		bytesDone += tmp.size();
	}
	else {
		tmp = bytesFromSocket;
		bytesFromSocket.clear();
	}
	return tmp;
}

void QFtpDTP::writeData()
{
	if (!socket)
		return;

	if (is_ba) {
#if defined(QFTPDTP_DEBUG)
		qDebug("QFtpDTP::writeData: write %d bytes", data.ba->size());
#endif
		if (data.ba->size() == 0)
			emit dataTransferProgress(0, bytesTotal);
		else
			socket->write(data.ba->data(), data.ba->size());

		socket->close();

		clearData();
	}
	else if (data.dev) {
		callWriteData = false;
		const qint64 blockSize = 16 * 1024;
		char buf[16 * 1024];
		qint64 read = data.dev->read(buf, blockSize);
#if defined(QFTPDTP_DEBUG)
		qDebug("QFtpDTP::writeData: write() of size %lli bytes", read);
#endif
		if (read > 0) {
			socket->write(buf, read);
		}
		else if (read == -1 || (!data.dev->isSequential() && data.dev->atEnd())) {
			// error or EOF
			if (bytesDone == 0 && socket->bytesToWrite() == 0)
				emit dataTransferProgress(0, bytesTotal);
			socket->close();
			clearData();
		}

		// do we continue uploading?
		callWriteData = data.dev != 0;
	}
}

void QFtpDTP::dataReadyRead()
{
	writeData();
}

void QFtpDTP::abortConnection()
{
#if defined(QFTPDTP_DEBUG)
	qDebug("QFtpDTP::abortConnection, bytesAvailable == %lli",
		socket ? socket->bytesAvailable() : (qint64)0);
#endif
	callWriteData = false;
	clearData();

	if (socket)
		socket->abort();
}

bool QFtpDTP::parseDir(const QByteArray& buffer, const QString& userName, QUrlInfo* info)
{
	if (buffer.isEmpty())
		return false;

	QString bufferStr = QString::fromLatin1(buffer).trimmed();

	// Unix style FTP servers
	QRegExp unixPattern(QLatin1String("^([\\-dl])([a-zA-Z\\-]{9,9})\\s+\\d+\\s+(\\S*)\\s+"
		"(\\S*)\\s+(\\d+)\\s+(\\S+\\s+\\S+\\s+\\S+)\\s+(\\S.*)"));
	if (unixPattern.indexIn(bufferStr) == 0) {
		_q_parseUnixDir(unixPattern.capturedTexts(), userName, info);
		return true;
	}

	// DOS style FTP servers
	QRegExp dosPattern(QLatin1String("^(\\d\\d-\\d\\d-\\d\\d\\d?\\d?\\ \\ \\d\\d:\\d\\d[AP]M)\\s+"
		"(<DIR>|\\d+)\\s+(\\S.*)$"));
	if (dosPattern.indexIn(bufferStr) == 0) {
		_q_parseDosDir(dosPattern.capturedTexts(), userName, info);
		return true;
	}

	// Unsupported
	return false;
}

void QFtpDTP::socketConnected()
{
	bytesDone = 0;
#if defined(QFTPDTP_DEBUG)
	qDebug("QFtpDTP::connectState(CsConnected)");
#endif
	emit connectState(QFtpDTP::CsConnected);
}

void QFtpDTP::socketReadyRead()
{
	if (!socket)
		return;

	if (pi->currentCommand().isEmpty()) {
		socket->close();
#if defined(QFTPDTP_DEBUG)
		qDebug("QFtpDTP::connectState(CsClosed)");
#endif
		emit connectState(QFtpDTP::CsClosed);
		return;
	}

	if (pi->abortState == QFtpPI::AbortStarted) {
		// discard data
		socket->readAll();
		return;
	}

	if (pi->currentCommand().startsWith(QLatin1String("LIST"))) {
		while (socket->canReadLine()) {
			QUrlInfo i;
			QByteArray line = socket->readLine();
#if defined(QFTPDTP_DEBUG)
			qDebug("QFtpDTP read (list): '%s'", line.constData());
#endif
			if (parseDir(line, QLatin1String(""), &i)) {
				emit listInfo(i);
			}
			else {
				// some FTP servers don't return a 550 if the file or directory
				// does not exist, but rather write a text to the data socket
				// -- try to catch these cases
				if (line.endsWith("No such file or directory\r\n"))
					err = QString::fromLatin1(line);
			}
		}
	}
	else {
		if (!is_ba && data.dev) {
			do {
				QByteArray ba;
				ba.resize(socket->bytesAvailable());
				qint64 bytesRead = socket->read(ba.data(), ba.size());
				if (bytesRead < 0) {
					// a read following a readyRead() signal will
					// never fail.
					return;
				}
				ba.resize(bytesRead);
				bytesDone += bytesRead;
#if defined(QFTPDTP_DEBUG)
				qDebug("QFtpDTP read: %lli bytes (total %lli bytes)", bytesRead, bytesDone);
#endif
				if (data.dev)       // make sure it wasn't deleted in the slot
					data.dev->write(ba);
				emit dataTransferProgress(bytesRead, bytesTotal);
				//qDebug() << "----------------------------" << bytesDone;
				// Need to loop; dataTransferProgress is often connected to
				// slots that update the GUI (e.g., progress bar values), and
				// if events are processed, more data may have arrived.
			} while (socket->bytesAvailable());
		}
		else {
#if defined(QFTPDTP_DEBUG)
			qDebug("QFtpDTP readyRead: %lli bytes available (total %lli bytes read)",
				bytesAvailable(), bytesDone);
#endif
			emit dataTransferProgress(bytesDone + socket->bytesAvailable(), bytesTotal);
			emit readyRead();
		}
	}
}

void QFtpDTP::socketError(QAbstractSocket::SocketError e)
{
	if (e == QTcpSocket::HostNotFoundError) {
#if defined(QFTPDTP_DEBUG)
		qDebug("QFtpDTP::connectState(CsHostNotFound)");
#endif
		emit connectState(QFtpDTP::CsHostNotFound);
	}
	else if (e == QTcpSocket::ConnectionRefusedError) {
#if defined(QFTPDTP_DEBUG)
		qDebug("QFtpDTP::connectState(CsConnectionRefused)");
#endif
		emit connectState(QFtpDTP::CsConnectionRefused);
	}
}

void QFtpDTP::socketConnectionClosed()
{
	if (!is_ba && data.dev) {
		clearData();
	}

	bytesFromSocket = socket->readAll();
#if defined(QFTPDTP_DEBUG)
	qDebug("QFtpDTP::connectState(CsClosed)");
#endif
	emit connectState(QFtpDTP::CsClosed);
}

void QFtpDTP::socketBytesWritten(qint64 bytes)
{
	bytesDone += bytes;
#if defined(QFTPDTP_DEBUG)
	qDebug("QFtpDTP::bytesWritten(%lli)", bytesDone);
#endif
	emit dataTransferProgress(bytesDone, bytesTotal);
	if (callWriteData)
		writeData();
}

void QFtpDTP::setupSocket()
{
	socket = listener.nextPendingConnection();
	socket->setObjectName(QLatin1String("QFtpDTP Active state socket"));
	connect(socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(socket, SIGNAL(disconnected()), SLOT(socketConnectionClosed()));
	connect(socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));

	listener.close();
}

void QFtpDTP::clearData()
{
	is_ba = false;
	data.dev = 0;
}
