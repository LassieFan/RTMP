#include "Threadpool.h"
#include "RtmpConnection.h"

std::map<int, int> ThreadPool::_mmap;

ThreadPool::ThreadPool(struct event_base* base)
{
	for (int i = 0; i < THREAD_NUM; i++)
	{
		int pipefd[2];
		if (-1 == socketpair(PF_UNIX, SOCK_DGRAM, 0, pipefd))
		{
			std::cout << "create socketpair fail" << std::endl;
			return;
		}

		struct event* sock_event = event_new(base, pipefd[1], EV_READ | EV_PERSIST, ThreadPool::SockCall, NULL);
		if (NULL == sock_event)
		{
			std::cout << "add socketpair fail" << std::endl;
			return;
		}
		event_add(sock_event, NULL);
		_mmap.insert(std::pair<int, int>(pipefd[1], 0));
		work_thread[i] = new WorkThread(pipefd[0]);
	}
}

//��map���в���һ���̼߳����������ٵģ�ͨ��socketpair���׽��ִ��������߳�
void ThreadPool::sendto_workthread(Parameter pc)
{
	MAP_T::iterator min = _mmap.begin();
	MAP_T::iterator it = _mmap.begin();

	int num = it->second;
	for (; it != _mmap.end(); it++)
	{
		std::cout << it->first << "  " << it->second << std::endl;
		if (it->second <= num)
		{
			num = it->second;
			min = it;
		}
	}

	char buff[1024] = { 0 };
	memcpy(buff, &pc, sizeof(pc));
	::send(min->first, buff, strlen(buff), 0);
}

//���socketpair�Ļص�����Զ��¼�����������map��
void ThreadPool::SockCall(int fd, short event, void* arg) 
{
	char buff[128] = { 0 };
	if (-1 == ::recv(fd, buff, 127, 0))
	{
		std::cout << "recv from sockpair[1] fail" << std::endl;
		return;
	}

	int num = 0;
	sscanf(buff, "%d", &num);
	_mmap[fd] = num;
}


WorkThread::WorkThread(int fd)       //fd��socketpair[0]
{
	pthread_t id;
	_fd = fd;
	count = 0;
	pthread_create(&id, NULL, DealEvent, this);
	rtmp_conn = NULL;
}

//���߳��߼�����socketpair�󶨵�libevent�У���������
void* WorkThread::DealEvent(void* arg) 
{
	WorkThread* p = static_cast<WorkThread*>(arg);
	p->ebase = event_base_new();
	struct event* sock_event = event_new(p->ebase, p->_fd, EV_READ | EV_PERSIST, SockCall, p);
	if (NULL == sock_event)
	{
		std::cout << "create socketpair event fail" << std::endl;
		return NULL;
	}

	event_add(sock_event, NULL);
	event_base_dispatch(p->ebase);
}

//socketpair�пͻ������ӻص���ע�ύ���ص�������libevent��
void WorkThread::SockCall(int fd, short event, void* arg) {
	WorkThread* p = static_cast<WorkThread*>(arg);
	char buff[1024] = { 0 };                  //��socketpair��ȡ���ͻ��������׽���
	int n = ::recv(fd, buff, 1023, 0);
	if (n <= 0)
		return;

	struct Parameter pc;
	memset(&pc, 0, sizeof(pc));
	memcpy(&pc, buff, strlen(buff));
	int c = pc.clifd;
	p->rtmp_conn = pc.rc;        //��ֵRtmpConnection�������Ա����

	struct event* cli_event = event_new(p->ebase, c, EV_READ | EV_PERSIST, WorkThread::ClientCall, p);
	if (NULL == cli_event)
	{
		std::cout << "create client event fail" << std::endl;
		return;
	}

	event_add(cli_event, NULL);
	p->eventMap.insert(std::make_pair(c, cli_event));
	(p->count)++;

	//����ǰ ���߳���ά�����׽��ֵĸ������ظ��̳߳ص�map����
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "%d", p->count);
	if (-1 == ::send(p->_fd, buff, 127, 0))
	{
		std::cout << "send count fail" << std::endl;
	}
}


int WorkThread::readfromclient(void *arg, int fd)
{
	WorkThread* p = static_cast<WorkThread*>(arg);
	int ret = p->rtmp_conn->_readBufferPtr->readFd(fd);

	if (ret <= 0)
	{
		event_free(p->eventMap[fd]);
		p->eventMap.erase(fd);
		p->count--;
		::close(fd);
	}
	return ret;
}


//�пͻ��˽����Ļص�����
void WorkThread::ClientCall(int fd, short event, void* arg) 
{
	WorkThread* p = static_cast<WorkThread*>(arg);
	int ret = p->readfromclient(p, fd);
	if (ret != 0)
	{
		std::cout << "recv data error" << std::endl;
	}
	BufferReader *buff = p->rtmp_conn->_readBufferPtr.get();
	p->rtmp_conn->onRead(*buff);
}


