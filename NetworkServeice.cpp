//#include"stdafx.h"
//this is used to communicate by socket
#include"NetworkService.h"
int NetworkService::sendMessage(SOCKET curSocket, char *message, int messageSize) {
	return send(curSocket, message, messageSize, 0);
}
int NetworkService::receiveMessage(SOCKET cursocket, char *buffer, int bufSize) {
	return recv(cursocket, buffer, bufSize, 0);
}
NetworkService::NetworkService()
{
}

NetworkService::~NetworkService()
{
}