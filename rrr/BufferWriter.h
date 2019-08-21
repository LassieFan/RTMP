#ifndef __BUFFERWRITER_H_
#define __BUFFERWRITER_H_

#include "Common.h"

//写的主要是针对发送数据时，从rtmpmsg中获取到对应的数据将其封装成小端或大端存放到p中，最终转发
void writeUint32BE(char* p, uint32_t value);
void writeUint24BE(char* p, uint32_t value);
void writeUint16BE(char* p, uint16_t value);
void writeUint32LE(char* p, uint32_t value);


//从RTMPCONNECTION中打包好将要发送的数据，再这个类再次封装，成一个队列
class BufferWriter
{
public:
	BufferWriter(int capacity = kMaxQueueLength);
	~BufferWriter(){}
	bool append(std::shared_ptr<char> data, uint32_t size, uint32_t index = 0);
	int send(int sockfd);

	//没有待发送的
	bool isEmpty() const
	{
		return _buffer->empty();
	}

	//将队列长度返回
	uint32_t size() const
	{
		return _buffer->size();   
	}

private:
	//防止一次发送不完，给其定义已读量偏移值
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

