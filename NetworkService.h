#pragma once
#include<winsock2.h>
#include<Windows.h>
// this is used for communication by socket
class NetworkService
{
public:
	NetworkService();
	~NetworkService();
	static int sendMessage(SOCKET curSocket, char *message, int messageSize);
	static int receiveMessage(SOCKET curSocket, char *buffer, int bufSize);
private:

};
