#include "RtmpSession.h"
#include "RtmpConnection.h"

RtmpSession::RtmpSession()
{

}

RtmpSession::~RtmpSession()
{

}

//����map����������play�Ŀͻ��˷���Ԫ���ݣ�����������
void RtmpSession::sendMetaData(AmfObjects& metaData)
{
	if (m_clients.size() == 0)
	{
		return;
	}

	for (auto iter = m_clients.begin(); iter != m_clients.end(); iter++)
	{
		auto conn = iter->second.lock();     //ת��Ϊǿ����ָ��
		if (conn == nullptr) // conn disconect ������
		{
			m_clients.erase(iter++);
		}
		else
		{
			RtmpConnection* player = conn.get();
			if (player->isPlayer())                 //״̬ȷ���ǿ��Է��͵�״̬
			{
				player->sendMetaData(metaData);     //�Ǵ�RtmpConnection����session��sendMetaData�б𷢸�˭�������߼�������RtmpConnection������sendMetaData
				iter++;
			}
		}
	}
}

//�뷢��Ԫ�����߼���ͬ
void RtmpSession::sendMediaData(uint8_t type, uint32_t ts, std::shared_ptr<char> data, uint32_t size)
{
	
	if (m_clients.size() <= 1)
	{
		return;
	}

	for (auto iter = m_clients.begin(); iter != m_clients.end(); iter++)
	{
		auto conn = iter->second.lock();
		if (conn == nullptr) // conn disconect
		{
			m_clients.erase(iter++);
		}
		else
		{
			RtmpConnection* player = conn.get();
			if (player->isPlayer())
			{
				player->sendMediaData(type, ts, data, size);
				iter++;
			}
		}
	}
}

//RtmpConnection��publish��play����ã������Ӷ������������ı�session�е�����״̬
void RtmpSession::addClient(std::shared_ptr<RtmpConnection> conn)
{
	m_clients[conn->_fd] = conn;
	if (conn.get()->isPublisher())
	{
		m_hasPublisher = true;
	}
}

void RtmpSession::removeClient(std::shared_ptr<RtmpConnection> conn)
{
	m_clients.erase(conn->_fd);
	if (conn.get()->isPublisher())
	{
		m_hasPublisher = false;
	}
}

int RtmpSession::getClients()
{
	return m_clients.size();
}

