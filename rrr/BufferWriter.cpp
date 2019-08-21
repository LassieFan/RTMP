#include "BufferWriter.h"
#include <sys/socket.h>

void writeUint32BE(char* p, uint32_t value)
{
	p[0] = value >> 24;
	p[1] = value >> 16;
	p[2] = value >> 8;
	p[3] = value & 0xff;
}

void writeUint32LE(char* p, uint32_t value)
{
	p[0] = value & 0xff;
	p[1] = value >> 8;
	p[2] = value >> 16;
	p[3] = value >> 24;
}

void writeUint24BE(char* p, uint32_t value)
{
	p[0] = value >> 16;
	p[1] = value >> 8;
	p[2] = value & 0xff;
}

void writeUint16BE(char* p, uint16_t value)
{
	p[0] = value >> 8;
	p[1] = value & 0xff;
}


BufferWriter::BufferWriter(int capacity)
	: _maxQueueLength(capacity)
	, _buffer(new std::queue<Packet>)
{

}

//��rtmp��Ҫ���͵����ݴ����һ������packet����ɶ���
bool BufferWriter::append(std::shared_ptr<char> data, uint32_t size, uint32_t index)
{
	if (size <= index)
		return false;

	if ((int)_buffer->size() >= _maxQueueLength)
		return false;

	Packet pkt = { data, size, index };
	_buffer->emplace(std::move(pkt));

	return true;
}

int BufferWriter::send(int sockfd)
{

	int ret = 0;
	int count = 5;
	//ѭ�����Ͷ����е����ݣ���ֹһ�η�����
	do
	{
		if (_buffer->empty())
			return 0;

		count -= 1;
		Packet &pkt = _buffer->front();
		ret = ::send(sockfd, pkt.data.get() + pkt.writeIndex, pkt.size - pkt.writeIndex, 0);
		if (ret > 0)
		{
			pkt.writeIndex += ret;
			if (pkt.size == pkt.writeIndex)
			{
				count += 1;
				_buffer->pop();
			}
		}
		else if (ret < 0)
		{
			if (errno == EINTR || errno == EAGAIN)
				ret = 0;
		}
	} while (count > 0 && ret > 0);

	return ret;
}

