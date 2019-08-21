#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include "Common.h"
#include "Socket.h"

struct Parameter;
class RtmpConnection;
class WorkThread
{
public:
	WorkThread(int fd);       //fd��socketpair[0]
	static void ClientCall(int fd, short event, void* arg);
	static void SockCall(int fd, short event, void* arg);
	static void* DealEvent(void* arg);
	int readfromclient(void *arg, int fd);
private:
	struct event_base* ebase;			     //ÿ�������̶߳���libevent
	int count;
	int _fd;				//socketpair����һ���׽���
	std::map<int, struct event*> eventMap;   //������ӵ��¼�
	RtmpConnection* rtmp_conn;
	
};


class ThreadPool
{
public:
	typedef std::map<int, int> MAP_T;
	
	ThreadPool(struct event_base* base);
	void sendto_workthread(Parameter pc);

	static void SockCall(int fd, short event, void* arg);

private:
	WorkThread* work_thread[3];
	static std::map<int, int> _mmap;    //���socketpair��Զ˼������ӿͻ���

};


#endif
