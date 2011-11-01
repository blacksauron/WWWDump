#include "wwwdump.h"
#include <QtNetwork>

wwwdump::wwwdump(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), _responseHeader(200, "OK", 1, 1)
{
	_list = new QListWidget();
	setCentralWidget(_list);

	resize(640, 480);

	_response = "\r\n";
	_responseHeader.setContentType("text/xml");
    _responseHeader.setContentLength(_response.size());

	_http = new QTcpServer();
	connect(_http, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    if (!_http->listen(QHostAddress::Any, 81)) 
	{
		_list->addItem("ERROR: Unable to listen on port 81");
    }
	else
	{
		_list->addItem("Listening on port 81");
	}
}

wwwdump::~wwwdump()
{

}

/////////////////////////////////////////////////////////////////////////////
// wwwdump onNewConnection

void wwwdump::onNewConnection()
{
	// Accept connection and await HTTP request
	QTcpSocket *pSocket = _http->nextPendingConnection();
	if (pSocket)
	{
		QObject::connect(pSocket, SIGNAL(readyRead()), this, SLOT(on_readClient()));
		QObject::connect(pSocket, SIGNAL(disconnected()), this, SLOT(on_disconnectedClient()));

		_list->addItem("Received connection from "+pSocket->peerAddress().toString()+":"+QString::number(pSocket->peerPort()));
	}
}

/////////////////////////////////////////////////////////////////////////////
// wwwdump on_disconnectedClient

void wwwdump::on_disconnectedClient()
{
	QTcpSocket *pSocket = (QTcpSocket*)sender();
	if (pSocket == NULL)
		return;
	_list->addItem(pSocket->peerAddress().toString()+":"+QString::number(pSocket->peerPort())+" disconnected");
}

/////////////////////////////////////////////////////////////////////////////
// wwwdump on_readClient

void wwwdump::on_readClient()
{
	QTcpSocket *pSocket = (QTcpSocket*)sender();
	if (pSocket == NULL)
		return;
	// Read HTTP Request Data
	_recvData += pSocket->readAll();
	_list->addItem(pSocket->peerAddress().toString()+":"+QString::number(pSocket->peerPort())+" recv data "+QString::number(_recvData.size())+" bytes");
	// Parse HTTP Request
	QHttpRequestHeader request(_recvData);
	// Decode and Handle HTTP Request
	const QString method = request.method();
	if (method == "POST")
	{
		const int nStart = _recvData.indexOf("\r\n\r\n");
		if ((nStart != -1) && (_recvData.size()-(nStart+4) == request.contentLength()))
		{
			QString outputfilename = request.path().mid(1, -1);
			QFile file(outputfilename);
			if (file.open(QFile::WriteOnly))
			{
				file.write(_recvData.mid(nStart, -1));
				file.close();
				_list->addItem("Outputting file '"+outputfilename+"'");
			}
			pSocket->write(_responseHeader.toString().toUtf8());
			pSocket->write(_response);
			pSocket->close();
			_recvData.clear();
		}
	}
	else
	{
		pSocket->write(_responseHeader.toString().toUtf8());
		pSocket->write(_response);
		pSocket->close();
		_recvData.clear();
	}
}
