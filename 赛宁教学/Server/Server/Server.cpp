// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>

#pragma comment (lib,"ws2_32.lib") 

SOCKET m_sock;

int Init(const char* address, int port)
{
	int rlt = 0;

	int iErrorMsg;

	WSAData wsaData;
	iErrorMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);

	if (iErrorMsg != NO_ERROR)
	{
		printf("wsastartup failed with error : %d\n", iErrorMsg);

		rlt = 1;
		return rlt;
	}

	//Create Socket for server
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)

	{
		printf("socket failed with error : %d\n", WSAGetLastError());

		rlt = 2;
		return rlt;
	}

	//info 
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = port;
	serverAddr.sin_addr.s_addr = inet_addr(address);

	//bind
	iErrorMsg = bind(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (iErrorMsg < 0)
	{
		printf("bind failed with error : %d\n", iErrorMsg);
		rlt = 3;
		return rlt;
	}



	return rlt;
}

void Run()
{
	//listen for connection 
	listen(m_sock, 5);

	sockaddr_in tcpAddr;
	int len = sizeof(sockaddr);
	SOCKET newSocket;
	char buffer[1024];
	int rval, sval = 0;
	do
	{
		//accept connection 
		newSocket = accept(m_sock, (sockaddr*)&tcpAddr, &len);


		if (newSocket == INVALID_SOCKET)
		{
			printf("Come on...\n");
		}
		else
		{
			printf("Successfully connected!\n");
			send(newSocket, "Successfully connected!\n", strlen("Successfully connected!\n"), 0);
			//message loop
			do
			{
				memset(buffer, 1024, 0);
				rval = recv(newSocket, buffer, 1024, 0);
				send(newSocket, "recieve\n", 8, 0);

			} while (true);

			closesocket(newSocket);
		}



	} while (1);

	closesocket(m_sock);
}

int main()
{
	Init("127.0.0.1", 3777);
	Run();
    return 0;
}

