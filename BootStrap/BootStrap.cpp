#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <map>
#include <thread>
#include <process.h>
#include <sstream>
#include "commands.h"
#pragma comment(lib,"ws2_32.lib")

struct BSPacket
{
	int command;
	SOCKADDR_IN whereTo;
	SOCKADDR_IN me;
	int width;
	int height;
};

typedef struct {
	SOCKADDR_IN whereTo;
	SOCKADDR_IN me;
	int i;
}BSPacketi;

std::map<int, SOCKADDR_IN> servents;

SOCKET sock; // this is the socket that we will use it 
SOCKET sock2[200]; // this is the sockets that will be recived from the Clients and sended to them
SOCKADDR_IN i_sock2; // this will containt informations about the clients connected to the server
SOCKADDR_IN i_sock; // this will containt some informations about our socket
WSADATA Data; // this is to save our socket version
int clients = 0; // we will use it in the accepting clients
int StartServer(int);
void AcceptClient(void *vArgs);
int width=800, height=800;

int EndSocket();
int main()
{
	std::cout << "Enter height: ";
	std::cin >> height;
	std::cout << "Enter width: ";
	std::cin >> width;
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
	BSPacketi* st = (BSPacketi*)vArgs;

	servents.insert(std::pair<int, SOCKADDR_IN>(st->i, st->me));

	BSPacket mp;
	mp.height = height;
	mp.width = width;
	if (st->i == 0)
	{
		mp.command = YOUAREZERO;
		mp.me = st->me;

	}
	else
	{
		int r = rand() % servents.size();
		std::map<int, SOCKADDR_IN>::iterator it;
		it = servents.find(r);
		while (r == st->i || it == servents.end())
		{
			r = rand() % servents.size();
			it = servents.find(r);
		}

		mp.command = GOTO;
		mp.me = st->me;
		mp.whereTo = servents[r];

	}
	//mp.myLong = r;
	//std::stringstream ss;
	//ss << "GOTO";
	//mp.command = "GOTO";
	
	Send((char*)&mp, sizeof(BSPacket), st->i);
	//clients++;


	//printf("Accepted client's port:%s\n", servents[0]);
	delete(st);
	//printf("ACCEPTED!");
	
}
void checkIfAlive(void* argv)
{
	int c = 0;
	while (1)
	{
		if (c == clients)
		{
			c = 0;
			Sleep(60000);
		}
		int err;
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
		SOCKET sock1 = socket(AF_INET, SOCK_STREAM, 0);
		if (sock1 == INVALID_SOCKET)
		{
			printf("INVALID SOCKET!!!");
		}
		SOCKADDR_IN i_sockc = servents[c];
		int ss = connect(sock1, (struct sockaddr*)&i_sockc, sizeof(i_sockc));
		if (ss != 0)
		{

			printf("Cannot connect to the servent!\n");
			servents.erase(c);
		}
		c++;
		
	}
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
	_beginthread(checkIfAlive, NULL, NULL);
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
		BSPacketi* st = new BSPacketi();
		st->i = clients;
		st->whereTo = i_sock2;
		st->me = i_sock2;
		clients++;
		_beginthread(AcceptClient, NULL, (void*)st);


		printf("a client has joined the server(IP: %d.%d.%d.%d - PORT: %d)\n", int(i_sock2.sin_addr.s_addr & 0xFF), 
																			   int((i_sock2.sin_addr.s_addr & 0xFF00) >> 8),
																			   int((i_sock2.sin_addr.s_addr & 0xFF0000) >> 16),
																			   int((i_sock2.sin_addr.s_addr & 0xFF000000) >> 24), i_sock2.sin_port);
		

		
	}
	return 1;
}




