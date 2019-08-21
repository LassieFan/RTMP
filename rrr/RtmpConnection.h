#ifndef __RTMP_CONNECTION_H_
#define __RTMP_CONNECTION_H_

#include "Common.h"
#include "Socket.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "amf.h"
#include <memory>

class RtmpServer;
class RtmpSession;

// chunk header: basic_header + rtmp_message_header 
struct RtmpMessageHeader
{
	uint8_t timestamp[3];
	uint8_t length[3];
	uint8_t typeId;
	uint8_t streamId[4]; //С�˸�ʽ
};

struct RtmpMessage
{
	uint32_t timestamp = 0;
	uint32_t timestampDelta = 0; //ʱ������������˵�һ�����Ǿ���ʱ�䣬ʣ�µĶ���ʱ�������
	uint32_t length = 0;		 //type0����type1������������ݳ��ȱ��棬���ں��������ݰ�
	uint8_t  typeId = 0;
	uint32_t streamId = 0;		 //message stream idֻ��type 0����һ���һ���������Ա�������
	uint32_t extTimestamp = 0;

	uint8_t  csid = 0;			 //chunk stream id
	uint64_t clock = 0;
	uint32_t index = 0;			 //��¼�ṩdata�е�����λ�ã�֮������λ�����������
	std::shared_ptr<char> data = nullptr;	//����

	void reset()
	{
		index = 0;
	}
};

class RtmpConnection: public std::enable_shared_from_this<RtmpConnection>
{
public:
	enum ConnectionStatus
	{
		HANDSHAKE_C0C1,
		HANDSHAKE_C2,
		HANDSHAKE_COMPLETE,
		START_PLAY,
		START_PUBLISH
	};

	//ͷ��typeId����
	enum RtmpMessagType
	{
		RTMP_SET_CHUNK_SIZE = 0x1, //���ÿ��С
		RTMP_AOBRT_MESSAGE = 0X2, //��ֹ��Ϣ
		RTMP_ACK = 0x3, //ȷ��
		RTMP_USER_EVENT = 0x4, //�û�������Ϣ
		RTMP_ACK_SIZE = 0x5, //���ڴ�Сȷ��
		RTMP_BANDWIDTH_SIZE = 0x6, //���öԶ˴���
		RTMP_AUDIO = 0x08,
		RTMP_VIDEO = 0x09,
		RTMP_FLEX_MESSAGE = 0x11, //amf3
		RTMP_NOTIFY = 0x12, //amf0 data
		RTMP_INVOKE = 0x14, //amf0 command
		RTMP_FLASH_VIDEO = 0x16,
	};

	//ͷ��chunk stream id����stream id��ͬ
	enum ChunkSreamId
	{
		CHUNK_CONTROL_ID = 2, // ������Ϣ
		CHUNK_INVOKE_ID = 3,
		CHUNK_AUDIO_ID = 4,
		CHUNK_VIDEO_ID = 5,
		CHUNK_DATA_ID = 6,
	};


	std::string getStreamPath() const
	{
		return m_streamPath;
	}

	std::string getStreamName() const
	{
		return m_streamName;
	}

	std::string getApp() const
	{
		return m_app;
	}

	AmfObjects getMetaData() const
	{
		return m_metaData;
	}

	bool isPlayer() const
	{
		return m_connStatus == START_PLAY;
	}

	bool isPublisher() const
	{
		return m_connStatus == START_PUBLISH;
	}

	RtmpConnection() {}
	RtmpConnection(RtmpServer* rtmpServer, SOCKET sockfd);
	~RtmpConnection();
	std::shared_ptr<BufferReader> _readBufferPtr;     //��socket�л�ȡbuffer�����ݴ�
	bool onRead(BufferReader& buffer);
	bool sendMetaData(AmfObjects& metaData);
	bool sendMediaData(uint8_t type, uint32_t ts, std::shared_ptr<char> payload, uint32_t payloadSize);

	SOCKET _fd;

private:
	bool handleHandshake(BufferReader& buffer);  // ����
	bool handleChunk(BufferReader& buffer);      //chunk�������ְ���һ������Ϣ
	bool handleMessage(RtmpMessage& rtmpMsg);    //����һ������Ϣ����Ҫ���ж����͵�����Ӧ�ӿ�
	bool handleInvoke(RtmpMessage& rtmpMsg);	 //����amf0 command��
	bool handleNotify(RtmpMessage& rtmpMsg);
	bool handleVideo(RtmpMessage rtmpMsg);
	bool handleAudio(RtmpMessage rtmpMsg);

	bool handleConnect();
	bool handleCreateStream();
	bool handlePublish();
	bool handlePlay();
	bool handDeleteStream();      //ɾ��session��map�е���Ч������

	void setPeerBandwidth();
	void sendAcknowledgement();
	void setChunkSize();

	bool sendInvokeMessage(uint32_t csid, std::shared_ptr<char> payload, uint32_t payloadSize);
	bool sendNotifyMessage(uint32_t csid, std::shared_ptr<char> payload, uint32_t payloadSize);
	void sendRtmpChunks(uint32_t csid, RtmpMessage& rtmpMsg);
	int createChunkBasicHeader(uint8_t fmt, uint32_t csid, char* buf);
	int createChunkMessageHeader(uint8_t fmt, RtmpMessage& rtmpMsg, char* buf);

	RtmpServer *m_rtmpServer;

	//std::shared_ptr<BufferReader> _readBufferPtr;
	std::shared_ptr<BufferWriter> _writeBufferPtr;
	void send(std::shared_ptr<char> data, uint32_t size , SOCKET fd);
	
	ConnectionStatus m_connStatus = HANDSHAKE_C0C1;
	const int kRtmpVersion = 0x03;				//C0S0�汾�ţ�Ĭ��Ϊ3
	const int kChunkMessageLen[4] = { 11, 7, 3, 0 };  //ChunkMessageHeader�ĳ���
	uint32_t m_inChunkSize = 128;				//Ĭ�ϵ�chunk(��ͷ��data)�Ĵ�С  ��chunk
	uint32_t m_outChunkSize = 128;				//�ظ�chunk��ʱҲ��128ΪĬ�����ݴ�С����������ʱ���¸�ֵ
	uint32_t m_streamId = 0;
	bool hasKeyFrame = false;
	AmfDecoder m_amfDec;				//amf0����
	AmfEncoder m_amfEnc;				//amf0����
	std::string m_app;
	std::string m_streamName;
	std::string m_streamPath;				//ͨ������ַ��ȡsession
	AmfObjects m_metaData;                 //Ԫ���ݣ�����Ƶ�ı����ʽ����ز�����
	std::map<int, RtmpMessage> m_rtmpMsgs;   //csid��rtmpmessage��ȷ���´λ�ȡ���Ծ���ͬһ����

	//�����ͨ�ò��֣�������Ӧ��ͻ��˵�connect��Ŀ��netconnection.success�ھ��崦���߼���д
	const uint32_t kPeerBandwidth = 5000000;            //��������
	const uint32_t kAcknowledgementSize = 5000000;      //ָ��Window Acknowledgement Size
	const uint32_t kMaxChunkSize = 60000;			    //ָ��ý�����ݲ�ֳɿ�ʱ�Ŀ��С					 
	const uint32_t kStreamId = 1;					    //����ʱ��Ҫ�����streamid����Ĭ��ֵ
};
#endif
