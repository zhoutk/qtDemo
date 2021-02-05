#ifndef QFTPDTP_H
#define QFTPDTP_H

#include <QTcpServer>
#include <QTcpSocket>
#include "qurlinfo.h"

class QFtpPI;

class QFtpDTP : public QObject
{
	Q_OBJECT

public:
	enum ConnectState {
		CsHostFound,
		CsConnected,
		CsClosed,
		CsHostNotFound,
		CsConnectionRefused
	};

	QFtpDTP(QFtpPI* p, QObject* parent = 0);

	void setData(QByteArray*);
	void setDevice(QIODevice*);
	void writeData();
	void setBytesTotal(qint64 bytes);

	inline bool hasError() const;
	inline QString errorMessage() const;
	inline void clearError();

	void connectToHost(const QString& host, quint16 port);
	int setupListener(const QHostAddress& address);
	void waitForConnection();

	QTcpSocket::SocketState state() const;
	qint64 bytesAvailable() const;
	qint64 read(char* data, qint64 maxlen);
	QByteArray readAll();

	void abortConnection();

	static bool parseDir(const QByteArray& buffer, const QString& userName, QUrlInfo* info);

signals:
	void listInfo(const QUrlInfo&);
	void listInfos(const QVector<QUrlInfo>&);
	void readyRead();
	void dataTransferProgress(qint64, qint64);

	void connectState(int);

private slots:
	void socketConnected();
	void socketReadyRead();
	void socketError(QAbstractSocket::SocketError);
	void socketConnectionClosed();
	void socketBytesWritten(qint64);
	void setupSocket();

	void dataReadyRead();

private:
	void clearData();

	QTcpSocket* socket;
	QTcpServer listener;

	QFtpPI* pi;
	QString err;
	qint64 bytesDone;
	qint64 bytesTotal;
	bool callWriteData;

	// If is_ba is true, ba is used; ba is never 0.
	// Otherwise dev is used; dev can be 0 or not.
	union {
		QByteArray* ba;
		QIODevice* dev;
	} data;
	bool is_ba;

	QByteArray bytesFromSocket;
};

bool QFtpDTP::hasError() const
{
	return !err.isNull();
}

QString QFtpDTP::errorMessage() const
{
	return err;
}

void QFtpDTP::clearError()
{
	err.clear();
}

#endif // QFTPDTP_H
