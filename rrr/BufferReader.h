#ifndef __BUFFERREADER_H_
#define __BUFFERREADER_H_
#include "Common.h"
#include "Socket.h"

//��Ҫ��Ҫת���ɴ�˵ģ���ȡȷ��������
uint32_t readUint32BE(char* data);
uint32_t readUint24BE(char* data);
uint16_t readUint16BE(char* data);

class BufferReader
{
public:
	static const uint32_t kInitialSize = 4096;
	BufferReader(uint32_t initialSize = kInitialSize);
	~BufferReader();


	uint32_t readableBytes()       //�ɶ��ֽ���
	{
		return _writerIndex - _readerIndex;
	}

	uint32_t writableBytes()       //ʣ���д�Ĵ�С
	{
		return _buffer->size() - _writerIndex;
	}

	char* peek()        //�ɶ�������ʼƫ��
	{
		return begin() + _readerIndex;
	}
	
	void retrieveAll();			  //�ָ���дƫ������ȫ����Ϊ0
	void retrieve(size_t len);    //��λ��ƫ�ƣ�ǰ��len������
	int readFd(int sockfd);       //��socket�������ݴ�ŵ�buffer�� 

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

	static const uint32_t MAX_BYTES_PER_READ = 4096;       //vector����ֻ����4096�ֽڣ�һ��������ô��
	static const uint32_t MAX_BUFFER_SIZE = 10240 * 100;   //����ܴ�ŵ��ֽ���

};

#endif
