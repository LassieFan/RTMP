#ifndef __BUFFERWRITER_H_
#define __BUFFERWRITER_H_

#include "Common.h"

//д����Ҫ����Է�������ʱ����rtmpmsg�л�ȡ����Ӧ�����ݽ����װ��С�˻��˴�ŵ�p�У�����ת��
void writeUint32BE(char* p, uint32_t value);
void writeUint24BE(char* p, uint32_t value);
void writeUint16BE(char* p, uint16_t value);
void writeUint32LE(char* p, uint32_t value);


//��RTMPCONNECTION�д���ý�Ҫ���͵����ݣ���������ٴη�װ����һ������
class BufferWriter
{
public:
	BufferWriter(int capacity = kMaxQueueLength);
	~BufferWriter(){}
	bool append(std::shared_ptr<char> data, uint32_t size, uint32_t index = 0);
	int send(int sockfd);

	//û�д����͵�
	bool isEmpty() const
	{
		return _buffer->empty();
	}

	//�����г��ȷ���
	uint32_t size() const
	{
		return _buffer->size();   
	}

private:
	//��ֹһ�η��Ͳ��꣬���䶨���Ѷ���ƫ��ֵ
	typedef struct 
	{
		std::shared_ptr<char> data;
		uint32_t size;
		uint32_t writeIndex;
	}Packet;

	std::shared_ptr<std::queue<Packet>> _buffer;
	int _maxQueueLength = 0;

	static const int kMaxQueueLength = 30;
};




#endif // ! __BUFFERWRITER_H_

