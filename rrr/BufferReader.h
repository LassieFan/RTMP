#ifndef __BUFFERREADER_H_
#define __BUFFERREADER_H_
#include "Common.h"
#include "Socket.h"

//主要是要转化成大端的，获取确定的数字
uint32_t readUint32BE(char* data);
uint32_t readUint24BE(char* data);
uint16_t readUint16BE(char* data);

class BufferReader
{
public:
	static const uint32_t kInitialSize = 4096;
	BufferReader(uint32_t initialSize = kInitialSize);
	~BufferReader();


	uint32_t readableBytes()       //可读字节数
	{
		return _writerIndex - _readerIndex;
	}

	uint32_t writableBytes()       //剩余可写的大小
	{
		return _buffer->size() - _writerIndex;
	}

	char* peek()        //可读处的起始偏移
	{
		return begin() + _readerIndex;
	}
	
	void retrieveAll();			  //恢复读写偏移量，全部置为0
	void retrieve(size_t len);    //置位读偏移，前面len读过了
	int readFd(int sockfd);       //从socket读出数据存放到buffer中 

	uint32_t size() const
	{
		return _buffer->size();
	}

private:
	char* begin()
	{
		//std::vector<char>::iterator iter =_buffer->begin();
		return &*_buffer->begin();
	}

	char* beginWrite()
	{
		return begin() + _writerIndex;
	}

	std::shared_ptr<std::vector<char>> _buffer;
	size_t _readerIndex = 0;
	size_t _writerIndex = 0;

	static const uint32_t MAX_BYTES_PER_READ = 4096;       //vector构造只开辟4096字节，一次最多读这么多
	static const uint32_t MAX_BUFFER_SIZE = 10240 * 100;   //最大能存放的字节数

};

#endif
