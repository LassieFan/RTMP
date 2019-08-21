#ifndef __TCPLISTENER_H_
#define __TCPLISTENER_H_

#include "Common.h"
#include "TcpSocket.h"
#include "Socket.h"

class TcpListener
{
public:
	TcpListener(std::string ip, uint16_t port);
	~TcpListener() {}
	int listen();
	int getlistenfd();
	SOCKET handleAccept();
private:
	int listenfd;
	std::shared_ptr<TcpSocket> _tcpSocket;
};


#endif
