#pragma once
//this is the defintion of the user-defined data type
#include <string.h>
#include<string>
#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

	INIT_CONNECTION = 0,

	ACTION_EVENT = 1,

	SERVER_EVENT = 2,

};
struct Packet {

	unsigned int packet_type;
	// server to receive message
	unsigned int toSercer;
	// server sending the message
	unsigned int fromServer;
	char toIp[15];
	char fromIp[15];
	// the message
	char message[100];
	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
	}
};

struct socket_info {
	unsigned int id;
	unsigned int to_server;
	unsigned int to_client;
};