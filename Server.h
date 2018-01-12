#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "NetworkService.h"
#include <ws2tcpip.h>
#include <stdio.h> 
#include <map>
// size of our buffer
#define DEFAULT_BUFLEN 512
// port to connect sockets through 
#define DEFAULT_PORT "6881"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include<string>
#include"networkData.h"
using namespace std;
class Server {
public:
	Server(string ip_address,int s_id);
	~Server();
	void initialserver();
	void connectToServer(string ip_address);
	void disConnect(string ip_address);
	bool acceptNewServer();
	int receiveData(unsigned int id, char *buf);
	void receive();
	void update();
	void sendToServer(Packet packet, string ip_address);
	void userSend();
	void broadcast(string msg);
	string changePath();
private:
	SOCKET ClientSocket;
	int count;
	string local_ip;
	int server_id;
	SOCKET ListenSocket;
	std::map<string, SOCKET*> serverSocket;
	std::map<int, SOCKET> listen_socket;
	int graph[5][5];
	string graphIp[5];
	bool neibor[5];
	bool lockstate;
};
