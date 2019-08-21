#include "RtmpServer.h"
#include "RtmpSession.h"
#include "RtmpConnection.h"

#include <iostream>

event_base* RtmpServer::base = NULL;
ThreadPool* RtmpServer::thread_pool = NULL;

RtmpServer::RtmpServer(std::string ip, uint16_t port)
	:ser(new TcpListener(ip, port))
{
	base = event_base_new();
	
	m_arg= {this,ser};
	struct event* listen_event = event_new(base, ser->getlistenfd(), EV_READ | EV_PERSIST, RtmpServer::LisCb, (void*)&m_arg);
	if (NULL == listen_event)
	{
		std::cout << "new listen event fail" << std::endl;
		return;
	}
	event_add(listen_event, NULL);
	thread_pool = new ThreadPool(RtmpServer::base);

}


void RtmpServer::loop() {
	event_base_dispatch(RtmpServer::base);
}

//�������׽������¼�����,�ص�
void RtmpServer::LisCb(int fd, short event, void* arg) {
	Arg* para = (Arg*)(arg);
	TcpListener *ser = para->s;
	RtmpServer * p = para->th;
	SOCKET clientfd = ser->handleAccept();
	if (-1 == clientfd)
	{
		std::cout << "accept fail" << std::endl;
		return;
	}
	p->newConn = new RtmpConnection(p,clientfd);
	Parameter link(p->newConn, clientfd);
	thread_pool->sendto_workthread(link);
	
}


void RtmpServer::addSession(std::string streamPath)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_rtmpSessions.find(streamPath) == m_rtmpSessions.end())
	{
		m_rtmpSessions[streamPath] = std::make_shared<RtmpSession>();
	}
}

void RtmpServer::removeSession(std::string streamPath)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_rtmpSessions.erase(streamPath);
}

bool RtmpServer::hasSession(std::string streamPath)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return (m_rtmpSessions.find(streamPath) != m_rtmpSessions.end());
}

//����ֱ���������ҵ���Ӧsession���Ҳ�������һ��
std::shared_ptr<RtmpSession> RtmpServer::getSession(std::string streamPath)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_rtmpSessions.find(streamPath) == m_rtmpSessions.end())
	{
		m_rtmpSessions[streamPath] = std::make_shared<RtmpSession>();
	}

	return m_rtmpSessions[streamPath];
}

//session�м�¼��publish״̬����RtmpConnection����֮���ı����ڲ�״̬��Ȼ�����session�ķŰ��޸�session״̬
bool RtmpServer::hasPublisher(std::string streamPath)
{
	auto sessionPtr = getSession(streamPath);
	if (sessionPtr == nullptr)
	{
		return false;
	}

	return sessionPtr->isPublishing();
}
