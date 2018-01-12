#include<iostream>
#include<fstream>
#include<list>
#include"Server.h"
#include<process.h>
#include<stdlib.h>
#include"dijkstra.h"
using namespace std;
void serverLoop(void *);
void clientLoop();
void broadcastLoop(void *);
Server *server;
int main() {
	server = new Server("172.18.70.150",0);   //�˴�����·��������
	_beginthread(serverLoop, 0, (void*)12);   //��һ���߳����������µ���Ϣ
	system("PAUSE");
	//server->connectToServer("172.18.69.160");
	//server->connectToServer("172.18.70.164");
	server->initialserver();                      //�������ھӵ�����
	_beginthread(broadcastLoop, 0, (void*)12);
	clientLoop();                //����ѭ���û����� �û����󷢳�ȥһ��֮������������ٴη���һ������
}
void serverLoop(void *arg) {
	while (1) {
		server->update();
		Sleep(10);                    //���������ĺ���
	}
}
void broadcastLoop(void *arg) {
	string str;
	while (1) {
		str = server->changePath();
		server->broadcast(str);
		Sleep(10000);
	}                                //�㲥
}
void clientLoop() {
	while (1) {
		server->userSend();
		system("pause");
	}                               //�������ݰ�
} 