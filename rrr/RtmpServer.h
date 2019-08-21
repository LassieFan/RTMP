#ifndef __RTMPSERVER_H_
#define __RTMPSERVER_H_

#include "Threadpool.h"
#include "TcpListener.h"

class RtmpSession;
class RtmpConnection;

class RtmpServer
{
public:
	struct Arg
	{
		RtmpServer *th;
		TcpListener * s;
	};

	RtmpServer(std::string ip, uint16_t port = 1935);
	~RtmpServer() {

	}   //��������һ�����������ر�
	void loop();
	static void LisCb(int fd, short event, void* arg);

	void addSession(std::string streamPath);
	void removeSession(std::string streamPath);
	std::shared_ptr<RtmpSession> getSession(std::string streamPath);
	bool hasSession(std::string streamPath);
	bool hasPublisher(std::string streamPath);


private:
	std::mutex m_mutex;             //�Խ��� ����������
	RtmpConnection* newConn;
	TcpListener* ser;
	static struct event_base *base;
	static ThreadPool* thread_pool;
	std::unordered_map<std::string, std::shared_ptr<RtmpSession> > m_rtmpSessions;   //���ֱ����ַ��session��ӳ�䣬��ȡsession�������

	Arg m_arg;
};

#endif
