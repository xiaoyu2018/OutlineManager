//client
#include<stdio.h>
#include"file_client.h"
#include<QDebug>

void LoadLib(WSADATA& wd)
{
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wd) != 0)
	{
		printf("fail to load winsock lib...\n");
		exit(0);
	}
}

bool UpLoadToSERVR(SOCKET &s,const char*filePath,const char*fileName)
{
	send(s, "1", 2, 0);
	char Buf[BUFFER_SIZE]={ 0 };
	int len;
	FILE* fp = fopen(filePath, "rb");
	int sendlen=send(s, fileName, 80, 0);
	if (SOCKET_ERROR==sendlen)
	  return false;
	while ((len = fread(Buf, 1, BUFFER_SIZE, fp)) > 0) 
	{
		send(s, Buf, len, 0);
	}
	fclose(fp);
	return true;
}

bool DownLoadFromSERVR(SOCKET &s,const char*fileName)
{
	int flag = 1;
	send(s, "2", 2, 0);
	char buffer[BUFFER_SIZE];
	int nCount;
	FILE* fp = fopen(fileName, "wb");
	int sendlen=send(s, fileName, 80, 0);
	if (SOCKET_ERROR==sendlen)
	  return false;
	while (flag)
	{
		
		if ((nCount = recv(s, buffer, BUFFER_SIZE, 0)) != BUFFER_SIZE)
			flag = 0;
		if (!strcmp(buffer, "error"))
		{
			return false;
		}
		fwrite(buffer, nCount, 1, fp);
	}
	fclose(fp);
	return true;
}
bool ConnecT2Server(SOCKET & s)
{
	WSADATA wsadata;//用于返回winsock库详细信息
	LoadLib(wsadata);//加载winsock库

	//客户端套接字
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET ==
		(s = socket(AF_INET, SOCK_STREAM, 0)))
	{
		return false;
	}

	//设置监听服务端地址
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(PORT);

		addrServer.sin_addr.S_un.S_addr = inet_addr(DES_IP);
		if (SOCKET_ERROR ==
			connect(s, (sockaddr*)&addrServer, sizeof(addrServer)))
		{
			return false;
		}
	return true;
}
