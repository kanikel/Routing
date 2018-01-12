#include"Server.h"
#include"networkData.h"
#include"NetworkService.h"
#include"dijkstra.h"
#include<string>
#include<stdlib.h>
#include <time.h>
#include<list>
#include<iostream>
using namespace std;
Server::Server(string ip_address,int s_id) {
	this->local_ip = ip_address;
	this->server_id = s_id;
	this->count = 0;
	this->lockstate = 0;
	string graph_ip[5] = { "172.18.70.150","172.18.69.160","172.18.69.40","172.18.69.135","172.18.71.69" };
	for (int i = 0; i < 5; i++) {
		this->graphIp[i] = graph_ip[i];
	}
	int temp[5][5] = { { 0, 4, -1, 2, -1 },{ -1, 0, 3, -1, -1 },{ -1, -1, 0, 5, -1 },{ -1, 3, -1, 0, 2 },{ 4, -1, 4, -1, 0 } };
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++)
		{
			this->graph[i][j] = temp[i][j];
			if (i == this->server_id) {
				this->neibor[j] = graph[i][j] > 0 ? 1 : 0;
			}
		}
	}
	//��������socket����
	// create WSADATA object
	WSADATA wsaData;

	// our sockets for the server
	ListenSocket = INVALID_SOCKET;

	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
	hints.ai_flags = AI_PASSIVE;
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) {
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}
Server::~Server() {

}
void Server::initialserver() {
	for (int i = 0; i < 5; i++) {
		if (i != server_id&&this->neibor[i]>0) {
			connectToServer(graphIp[i]);
		}
	}
}
void Server::connectToServer(string ip_address) {
	// ����������
	//int nNetTimeout = 1000;
	// create WSADATA object
	WSADATA wsaData;

	// socket
	SOCKET *ConnectSocket = new SOCKET(INVALID_SOCKET);

	// holds address info for socket to connect to
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}



	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!
									  //resolve server address and port
									  //hints.ai_canonname = 
	iResult = getaddrinfo(ip_address.c_str(), DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		*ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		//setsockopt(*ConnectSocket,SOL_SOCKET, SO_SNDTIMEO,(char *)&nNetTimeout, sizeof(int));
		if (*ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}

		// Connect to server.
		iResult = connect(*ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR)
		{
			closesocket(*ConnectSocket);
			*ConnectSocket = INVALID_SOCKET;
			printf("The server is down... did not connect");
		}
	}
	// �Ѻ��ھӵ����ӱ�������
	// pass the connectSocket to 
	this->serverSocket.insert(pair<string, SOCKET*>(ip_address, ConnectSocket));
	// no longer need address info for server
	freeaddrinfo(result);



	// if connection failed
	if (*ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		exit(1);
	}
	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;

	iResult = ioctlsocket(*ConnectSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(*ConnectSocket);
		WSACleanup();
		exit(1);
	}
	//disable nagle
	char value = 1;
	setsockopt(*ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
}

// �Ͽ�����
void Server::disConnect(string ip_address) {
	std::map<string, SOCKET *>::iterator ppp;
	ppp = this->serverSocket.find(ip_address);
	SOCKET *soc = ppp->second;
	closesocket(*soc);
	this->serverSocket.erase(ppp);
	delete soc;
}
// ����Ƿ����µ�����socket
bool Server::acceptNewServer() {
	// if client waiting, accept the connection and save the socket
	//int nNetTimeout = 1000;
	//setsockopt(ListenSocket,SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket != INVALID_SOCKET)
	{
		//disable nagle on the client's socket
		char value = 1;
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		// insert new client into session id table
		listen_socket.insert(pair<unsigned int, SOCKET>(this->count, ClientSocket));
		this->count += 1;
		return true;
	}

	return false;
}
// ��鷢�͹���������
int Server::receiveData(unsigned int id, char *buf) {
	if (listen_socket.find(id) != listen_socket.end())
	{
		SOCKET currentSocket = listen_socket[id];
		int iResult = NetworkService::receiveMessage(currentSocket, buf, MAX_PACKET_SIZE);
		if (iResult == 0)
		{
			printf("Connection closed\n");
			closesocket(currentSocket);
		}
		return iResult;
	}
	return 0;
}
// ��鷢�͹���������
void Server::receive() {
	Packet packet;
	char *network_data = new char[1000000];
	std::map<int, SOCKET>::iterator iter;
	for (iter = this->listen_socket.begin(); iter != listen_socket.end(); iter++) {
		int data_length = receiveData(iter->first, network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		int i = 0;
		while (i < (unsigned int)data_length)
		{
			packet.deserialize(&(network_data[i]));
			i += sizeof(Packet);
			int count1 = 0;
			int fromServer;
			int toServer;
			string toIp, fromIp;
			char *msg;
			int change[16];
			dijkstra *ddd = new dijkstra(this->graph, 5, 8);
			list<int> pathlist;
			//routeCentral = new dijkstra(*(this->graph), 5, 8);
			switch (packet.packet_type) {//�˴������ݰ��ֳ���������

			case INIT_CONNECTION://��ʼ����ʱ��鿴�Ƿ�������

				printf("host received init packet from %d host %d\n", packet.fromServer, packet.fromServer);

				//sendActionPackets();
				//sendToClient(99, packet.client_id);
				break;

			case ACTION_EVENT://���������ݰ���������Ϣ��
				toIp = packet.toIp;
				fromIp = packet.fromIp;
				//path = routeCentral->routing(fromClientId, toClientId);
				if (toIp == this->local_ip) {
					cout <<"host "<< this->local_ip<< " receive packet from host " << fromIp << endl;
					cout << "the message received is " << packet.message << endl;
				}
				else {
					//this->sendToServer(packet, toIp);
					cout << endl << "host ";
					cout << "receive packet from host " << fromIp << " and send it to host " << toIp << endl;
					cout << "the shortest path is ";
					//sendActionPackets();
					pathlist = ddd->routing(this->server_id, packet.toSercer);
					list<int>::iterator pathiter;
					cout << pathlist.front();
					for (pathiter = ++pathlist.begin(); pathiter != pathlist.end(); pathiter++) {
						cout << "->" << *pathiter;
					}
					cout << endl;
					pathlist.pop_front();
					sendToServer(packet, this->graphIp[pathlist.front()]);
				}
				break;
			case SERVER_EVENT://�㲥����·��  ���յ�֮�������и������� �����յ���֮�󣬼��������ǲ�������֮ǰ�������ģ��ǵĻ�������������ǣ����²��Ұ��������Լ����ھӽڵ㣩
				if (packet.toSercer + 3 > time(NULL))
					break;
				else {
					msg = packet.message;
					while (*(msg + count1)!='\0') {
						change[count1] = *(msg + count1);
						change[++count1] = *(msg + count1);
						change[++count1] = *(msg + count1);
						count1++;
					}
					while (1) {
						if (this->lockstate) {
							Sleep(500);
							continue;
						}
						else {
							lockstate = 1;
							for (int j = 0; j < 15; j++) {
								if (*(change + j) != '\0') {
									int row = *(change + j);
									int col = *(change + j + 1);
									int path = *(change + j + 2);
									this->graph[row][col] = path;
									j += 3;
								}
							}
							lockstate = 0;
							broadcast(packet.message);//�㲥
							break;
						}
					}
					break;
				}
			default:

				printf("error in packet types\n");

				break;
			}
			delete ddd;
		}
	}
	delete network_data;
}

void Server::update() {
	this->acceptNewServer();//֮ǰ������ һ���Ǽ���µ�����accept  һ���Ǽ���µ���Ϣreceive  �˴���������Ϣ����һ����
	this->receive();
}

void Server::sendToServer(Packet packet, string toIp) {//�Ѱ�������Ӧ��ip��ַ
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];
	packet.serialize(packet_data);
	std::map<string, SOCKET*>::iterator iter;
	iter = this->serverSocket.find(toIp);
	int iSendResult;
	iSendResult = NetworkService::sendMessage(*iter->second, packet_data, packet_size);
	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(*iter->second);
	}
}
void Server::userSend() {//�����û�����Ϣ����
	string msg, toIp; 
	int toServer_id;
	cout << "please input the ip of the server you want to send: "<<endl;
	cin >> toIp;
	cout << "please input the message you want to send:"<<endl;
	cin >> msg;
	cout << endl;
	//cout << "please input the server_id you want to send";   //
	//cin >> toServer_id;
	std::map<string, SOCKET*>::iterator iter;// ����ǳ�ʼ����ʱ���initial server
	iter = this->serverSocket.find(toIp);
	if (iter != this->serverSocket.end()) { //���ֻ�Ƿ����Լ����ھ� 
		Packet packet;
		packet.packet_type = ACTION_EVENT;
		packet.fromServer = this->server_id;
		strcpy_s(packet.fromIp,this->local_ip.c_str());
		strcpy_s(packet.toIp,toIp.c_str());
		strcpy_s(packet.message, msg.c_str());
		this->sendToServer(packet, toIp);
		cout << "succeed to send the packet from host " << local_ip << " to host " << toIp << endl;
	}
	else {  //������Ƿ����ھӽڵ� ����ls�㷨������·�� ����·���ϵĵڶ����ڵ�
		Packet packet;
		packet.packet_type = ACTION_EVENT;
		packet.fromServer = this->server_id;
		strcpy_s(packet.fromIp, this->local_ip.c_str());
		strcpy_s(packet.toIp, toIp.c_str());
		strcpy_s(packet.message, msg.c_str());
		this->sendToServer(packet, toIp);
		cout << "succeed to send the packet from host " << local_ip << " to host " << toIp << endl;
		dijkstra *ddd = new dijkstra(this->graph, 5, 8);
		for (toServer_id = 0; toServer_id < 5; toServer_id++) {
			if (this->graphIp[toServer_id] == toIp) {
				break;
			}
		}
		list<int> pathlist = ddd->routing(server_id,toServer_id);
		pathlist.pop_front();
		int nextNode = pathlist.front();
		for (int i = 0; i < 5; i++) {
			if (i==nextNode) {
				this->sendToServer(packet, this->graphIp[i]);
			}
		}
	}
}

void Server::broadcast(string msg) {   //�㲥
	for (int i = 0; i < 5; i++) {
		if (this->neibor[i]) {
			Packet packet;
			packet.packet_type = 2;
			packet.fromServer = this->server_id;
			packet.toSercer = time(NULL);
			strcpy_s(packet.message, msg.c_str());
			strcpy_s(packet.fromIp, this->local_ip.c_str());
			strcpy_s(packet.toIp, this->graphIp[i].c_str());
			this->sendToServer(packet, this->graphIp[i]);
		}
	}
}
string Server::changePath() {  //�ı�Ȩֵ���������
	string str;
	this->lockstate = 1;
	srand((unsigned)time(NULL));
	for (int i = 0; i < 5; i++) {
		if (this->neibor[i]) {
			int path = rand() % 7 + 3;
			this->graph[server_id][i] = path;
			str = str + (char)this->server_id + "|" + (char)i + "|" + (char)path + "|";
		}
	}
	for (int i = 0; i < 5; i++) {
		cout << "             " << this->graphIp[i];
	}
	cout << endl;
	cout << this->server_id;
	for (int i = 0; i < 5; i++) {
		cout << "        " << this->graph[server_id][i];
	}
	cout << endl;
	return str;
}
