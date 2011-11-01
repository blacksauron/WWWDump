#ifndef WWWDUMP_H
#define WWWDUMP_H

#include <QtNetwork>
#include <QtGui>

class wwwdump : public QMainWindow
{
	Q_OBJECT

public:
	wwwdump(QWidget *parent = 0, Qt::WFlags flags = 0);
	~wwwdump();

public slots:
	void onNewConnection();
	void on_readClient();
	void on_disconnectedClient();

private:
	QListWidget *_list;

	QTcpServer* _http;
	QByteArray _recvData;
	QByteArray _response;
	QHttpResponseHeader _responseHeader;
};

#endif // WWWDUMP_H
