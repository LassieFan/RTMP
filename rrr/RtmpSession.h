#ifndef __RTMPSESSION_H_
#define __RTMPSESSION_H_

#include "amf.h"
#include "Socket.h"
#include <memory>

class RtmpConnection;

class RtmpSession
{
public:
	RtmpSession();
	~RtmpSession();

	void setMetaData(AmfObjects metaData) 
	{
		m_metaData = metaData;
	}

	AmfObjects getMetaData() const       //��ȡԪ����
	{
		return m_metaData;
	}

	void addClient(std::shared_ptr<RtmpConnection> conn);
	void removeClient(std::shared_ptr<RtmpConnection> conn);
	int  getClients();

	void sendMetaData(AmfObjects& metaData);      //��m_clients�б������Է��͵������˷���Ԫ���ݣ������������ն��ǵ���RtmpConnection�еķ���
	void sendMediaData(uint8_t type, uint32_t ts, std::shared_ptr<char> data, uint32_t size);     //��m_clients�б������Է��͵������˷���ý������

	bool isPublishing() const       //���ձ�RtmpServer���û�����RtmpConnection�е���
	{
		return m_hasPublisher;
	}

private:

	AmfObjects m_metaData;					//�����˷��͹�����Ԫ���ݴ��������
	bool m_hasPublisher = false;
	std::unordered_map<SOCKET, std::weak_ptr<RtmpConnection>> m_clients;      //���ڼ�¼�ͻ��˵����ӣ�value�ǵ��ö�̬�����RtmpConnection����
};

#endif
