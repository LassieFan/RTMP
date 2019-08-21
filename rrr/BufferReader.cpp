#include "BufferReader.h"


//ת�����ȷ��������
uint32_t readUint32BE(char* data)
{
	uint8_t* p = (uint8_t*)data;
	uint32_t value = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
	return value;
}


uint32_t readUint24BE(char* data)
{
	uint8_t* p = (uint8_t*)data;
	uint32_t value = (p[0] << 16) | (p[1] << 8) | p[2];
	return value;
}


uint16_t readUint16BE(char* data)
{
	uint8_t* p = (uint8_t*)data;
	uint16_t value = (p[0] << 8) | p[1];
	return value;
}


BufferReader::BufferReader(uint32_t initialSize)
	: _buffer(new std::vector<char>(initialSize))
{
	_buffer->resize(initialSize);
}

BufferReader::~BufferReader()
{

}


void BufferReader::retrieveAll()   //�ָ���дƫ������ȫ����Ϊ0
{
	_writerIndex = 0;
	_readerIndex = 0;
}


//��λlen������ƫ��ֵ������len��ǰ��len������
void BufferReader::retrieve(size_t len)
{
	if (len <= readableBytes())
	{
		_readerIndex += len;
		if (_readerIndex == _writerIndex)
		{
			_readerIndex = 0;
			_writerIndex = 0;
		}
	}
	else
	{
		retrieveAll();
	}
}


int BufferReader::readFd(int sockfd)
{
	uint32_t size = writableBytes();
	if (size < MAX_BYTES_PER_READ) // ���µ���BufferReader��С
	{
		uint32_t bufferReaderSize = _buffer->size();
		if (bufferReaderSize > MAX_BUFFER_SIZE)
		{
			return 0; // close
		}

		_buffer->resize(bufferReaderSize + MAX_BYTES_PER_READ);
	}

	int bytesRead = ::recv(sockfd, beginWrite(), MAX_BYTES_PER_READ, 0);    //���׽����ж�ȡ���ݴ�ŵ�buffer��
	if (bytesRead > 0)
	{
		_writerIndex += bytesRead;
	}

	return bytesRead;
}


