#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <sys/types.h>         
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h> 
#include <netinet/ether.h>   
#include <netinet/ip.h>  
#include <netpacket/packet.h>   
#include <arpa/inet.h>
#include <net/ethernet.h>   
#include <net/route.h>  
#include <netdb.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <event.h>
#include <cstdint>
#include <cstring>

#define SOCKET int
#define INVALID_SOCKET  (-1)			//��Ч��socket
#define SOCKET_ERROR    (-1)			//socket����
#define THREAD_NUM  3                   //�̳߳��̸߳���

class RtmpConnection;
struct Parameter
{
	RtmpConnection * rc;
	SOCKET clifd;
	Parameter(){}
	Parameter(RtmpConnection * _rc, SOCKET c)
	{
		rc = _rc;
		clifd = c;
	}
};

#endif
