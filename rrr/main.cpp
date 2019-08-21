#include "RtmpServer.h"


int main()
{
	RtmpServer server("127.0.0.1", 1935);
	server.loop();

	return 0;
}