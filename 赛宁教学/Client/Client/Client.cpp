// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

SOCKET m_sock;
char inputBuf[1024];

int Connect(const char* address, int port)
{
	int rlt = 0;
	int iErrMsg;
	WSAData wsaData;
	iErrMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (iErrMsg != NO_ERROR)
		//Error
	{
		_tprintf(L"failed with wsaStartup error : %d\n", iErrMsg);

		rlt = 1;
		return rlt;
	}

	//Create ocket  
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//Create socket failed
	{
		_tprintf(L"socket failed with error : %d\n", WSAGetLastError());

		rlt = 2;
		return rlt;
	}

	//target server
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	sockaddrServer.sin_port = port;
	sockaddrServer.sin_addr.s_addr = inet_addr(address);

	//Connect 
	iErrMsg = connect(m_sock, (sockaddr*)&sockaddrServer, sizeof(sockaddrServer));
	if (iErrMsg < 0)
	{
		printf("connect failed with error : %d\n", iErrMsg);

		rlt = 3;
		return rlt;
	}

	return rlt;
}

int SendMsg(const char* msg, int len)
{
	int rlt = 0, rval;
	sockaddr_in tcpAddr;

	int length = sizeof(sockaddr);
	int iErrMsg = 0;

	//Send message 
	iErrMsg = send(m_sock, msg, len, 0);
	if (iErrMsg < 0)
		//Send failed 
	{
		printf("send msg failed with error : %d\n", iErrMsg);

		rlt = 1;
		return rlt;
	}

	return rlt;
}

void RecvMsg()
{
	char buf[1024];
	while (1)
	{
		memset(buf, 0, 1024);
		recv(m_sock, buf, 1024, 0);
		printf("%s", buf);
		memset(inputBuf, 0, 1024);
		scanf_s("%s", inputBuf, 1024);
		SendMsg(inputBuf, strlen(inputBuf));
	}
}

void Close()
{
	closesocket(m_sock);
}

int main()
{

	Connect("127.0.0.1", 3777);
	RecvMsg();
    return 0;
}

