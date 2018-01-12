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
	server = new Server("172.18.70.150",0);   //此处生成路由器对象
	_beginthread(serverLoop, 0, (void*)12);   //开一个线程用来监听新的信息
	system("PAUSE");
	//server->connectToServer("172.18.69.160");
	//server->connectToServer("172.18.70.164");
	server->initialserver();                      //建立和邻居的连接
	_beginthread(broadcastLoop, 0, (void*)12);
	clientLoop();                //用来循环用户请求 用户请求发出去一次之后可以跳回来再次发送一条请求
}
void serverLoop(void *arg) {
	while (1) {
		server->update();
		Sleep(10);                    //检查新请求的函数
	}
}
void broadcastLoop(void *arg) {
	string str;
	while (1) {
		str = server->changePath();
		server->broadcast(str);
		Sleep(10000);
	}                                //广播
}
void clientLoop() {
	while (1) {
		server->userSend();
		system("pause");
	}                               //发送数据包
} 