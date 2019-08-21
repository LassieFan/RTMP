#ifndef __TCPSOCKET_H_
#define __TCPSOCKET_H_

#include "Common.h"
#include "Socket.h"


//��Ҫsocketͨ�Ÿ�������ķ�װ
class TcpSocket
{
public:
	TcpSocket() = default;
	virtual ~TcpSocket();

	SOCKET create();
	bool bind(std::string ip, uint16_t port);
	bool listen(int backlog);
	SOCKET accept();
	bool connect(std::string ip, uint16_t port);
	void close();
	SOCKET fd() const { return _sockfd; }

private:
	SOCKET _sockfd = -1;

};

#endif
