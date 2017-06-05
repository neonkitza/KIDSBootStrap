#include "stdafx.h"
#include <Windows.h>
#include <map>
#include <thread>
#include <process.h>
#include <sstream>
#pragma comment(lib,"ws2_32.lib")

typedef struct
{
	std::string id;
	SOCKADDR_IN address;
	SOCKET socket;

}Node;
struct MyPacket
{
	std::string command;
	Node node;
};

struct AcceptPacket
{
	std::string command;
	socketStruct whereTo;
	socketStruct me;
};

typedef struct {
	SOCKADDR_IN address;
	SOCKET socket;
}socketStruct;
typedef struct {
	SOCKADDR_IN address;
	SOCKET socket;
	int i;
}socketStructi;

std::map<int, socketStruct> servents;

SOCKET sock; // this is the socket that we will use it 
SOCKET sock2[200]; // this is the sockets that will be recived from the Clients and sended to them
SOCKADDR_IN i_sock2; // this will containt informations about the clients connected to the server
SOCKADDR_IN i_sock; // this will containt some informations about our socket
WSADATA Data; // this is to save our socket version
int clients = 0; // we will use it in the accepting clients
int StartServer(int);
void AcceptClient(void *vArgs);


int EndSocket();
int main()
{
	StartServer(9999);
	EndSocket();
	return 0;
}

int Send(char *Buf, int len, int Client)
{
	int slen;
	slen = send(sock2[Client], Buf, len, 0);
	if (slen < 0)
	{
		printf("Cannot send data !");
		return 1;
	}
	return slen;
}


int Recive(char *Buf, int len, int Client)
{
	int slen;
	slen = recv(sock2[Client], Buf, len, 0);
	if (slen < 0)
	{
		printf("Cannot send data !");
		return 1;
	}
	return slen;
}

int EndSocket()
{
	closesocket(sock);
	WSACleanup();
	return 1;
}

void AcceptClient(void *vArgs)
{
	socketStructi* st = (socketStructi*)vArgs;

	socketStruct ss;
	ss.address = st->address;
	ss.socket = st->socket;
	servents.insert(std::pair<int, socketStruct>(st->i, ss));

	AcceptPacket mp;
	if (st->i == 0)
	{
		mp.command = "YOUAREZERO";
		mp.me = ss;

	}
	else
	{
		int r = rand() % servents.size();
		while (r == st->i)
			r = rand() % servents.size();

		mp.command = "GOTO";
		mp.me = ss;
		mp.whereTo = servents[r];

	}
	//mp.myLong = r;
	//std::stringstream ss;
	//ss << "GOTO";
	//mp.command = "GOTO";
	
	Send((char*)&mp, sizeof(MyPacket), st->i);
	//clients++;
	delete(st);
	//printf("ACCEPTED!");
	
}

int StartServer(int Port)
{
	int err;
	WSAStartup(MAKEWORD(2, 2), &Data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		Sleep(4000);
		exit(0);
		return 0;
	}

	memset(&i_sock, '0', sizeof(i_sock));
	memset(&i_sock2, '0', sizeof(i_sock2));
	i_sock.sin_family = AF_INET;
	i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
	i_sock.sin_port = htons(Port);
	err = bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
	if (err != 0)
	{
		return 0;
	}
	err = listen(sock, 100);
	if (err == SOCKET_ERROR)
	{

		return 0;
	}

	//bind(sock, (struct sockaddr*)&i_sock, sizeof(i_sock));

	//listen(sock, 100);
	while (1)
	{
		int so2len = sizeof(i_sock2);
		sock2[clients] = accept(sock, (sockaddr *)&i_sock2, &so2len);
		if (sock2[clients] == INVALID_SOCKET)
		{
			return 0;

		}
		//int i = clients;
		socketStructi* st = new socketStructi();
		st->i = clients;
		st->address = i_sock2;
		st->socket = sock2[clients];
		clients++;
		_beginthread(AcceptClient, NULL, (void*)st);


		printf("a client has joined the server(IP: %d.%d.%d.%d - PORT: %d)\n", int(i_sock2.sin_addr.s_addr & 0xFF), 
																			   int((i_sock2.sin_addr.s_addr & 0xFF00) >> 8),
																			   int((i_sock2.sin_addr.s_addr & 0xFF0000) >> 16),
																			   int((i_sock2.sin_addr.s_addr & 0xFF000000) >> 24), i_sock2.sin_port);
		

		
	}
	return 1;
}




