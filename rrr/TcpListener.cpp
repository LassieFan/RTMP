#include "TcpListener.h"

TcpListener::TcpListener(std::string ip, uint16_t port) : _tcpSocket(std::make_shared<TcpSocket>())
{
	listenfd = _tcpSocket->create();        //��ȡ��һ���׽��ִ�ŵ�_tcpSocket��Ա������
	_tcpSocket->bind(ip, port);

	if (-1 == listen())
	{
		return;
	}

}

int TcpListener::listen()
{
	if (!_tcpSocket->listen(1024))
	{
		return -1;
	}
	return 0;
}

SOCKET TcpListener::handleAccept()
{
	int connfd = _tcpSocket->accept();
	if (connfd > 0)
	{
		return connfd;
	}
	return -1;
}

SOCKET TcpListener::getlistenfd()
{
	return listenfd;
}