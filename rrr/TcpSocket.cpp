#include "TcpSocket.h"


TcpSocket::~TcpSocket()
{

}

SOCKET TcpSocket::create()
{
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	return _sockfd;
}

bool TcpSocket::bind(std::string ip, uint16_t port)
{
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	if (::bind(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << " <socket=" << _sockfd << ">" << " bind <" << ip.c_str() << ":" << port <<"> failed" << std::endl;
		return false;
	}

	return true;
}

bool TcpSocket::listen(int backlog)
{
	if (::listen(_sockfd, backlog) == SOCKET_ERROR)
	{
		std::cout << "<socket=" << _sockfd << "> listen failed" << std::endl;
		return false;
	}

	return true;
}

SOCKET TcpSocket::accept()
{
	struct sockaddr_in addr = { 0 };
	socklen_t addrlen = sizeof addr;

	SOCKET clientfd = ::accept(_sockfd, (struct sockaddr*)&addr, &addrlen);

	return clientfd;
}

bool TcpSocket::connect(std::string ip, uint16_t port)
{
	bool isConnected = true;

	struct sockaddr_in addr = { 0 };
	socklen_t addrlen = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	if (::connect(_sockfd, (struct sockaddr*)&addr, addrlen) == SOCKET_ERROR)
	{
		isConnected = false;
		std::cout << "<socket=" <<_sockfd << "> connect failed" << std::endl;
	}

	return isConnected;
}

void TcpSocket::close()
{
	::close(_sockfd);
	_sockfd = 0;
}

