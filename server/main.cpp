#include<QDebug>
#include<stdio.h>
#include <stdlib.h>
#include<WinSock2.h>
#include <windows.h>
#define PORT 6000 //本服务器监听端口号
#define BUFFER_SIZE 1024 //数据缓冲区大小
#define MAX_LISTEN_NUM 20//最大监听客户端数

//载入winsock库
void LoadLib(WSADATA& wd)
{
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wd) != 0)
	{
		printf("fail to load winsock lib...\n");
		exit(0);
	}
}
void Record(int m, sockaddr_in addr, char* filename)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	FILE* f = fopen("log.txt", "a+");
	fprintf(f, "%4d/%02d/%02d %02d:%02d:%02d\n",
		sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);

	m == 1 ? fprintf(f, "上传文件：") : fprintf(f, "下载文件：");

	fprintf(f, "%s\n", filename);
	fprintf(f, "操作ip:%s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	fprintf(f, "************************************************\n");
	fclose(f);
}

//上传文件
void UpLoad(SOCKET s, sockaddr_in addr)
{
	printf("%s:%d is uploading...\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	int flag = 1;
	char filename[80];
	recv(s, filename, 80, 0);
	FILE* fp = fopen(filename, "wb");
	char buffer[BUFFER_SIZE] = { 0 };  //文件缓冲区
	int nCount;
	//实现大文件分段
	while (flag)
	{
		if ((nCount = recv(s, buffer, BUFFER_SIZE, 0)) != BUFFER_SIZE)
			flag = 0;
		fwrite(buffer, nCount, 1, fp);
	}
	fclose(fp);
	printf("%s 上传完毕\n", filename);
	Record(1, addr, filename);

}
//下载文件
void DownLoad(SOCKET s, sockaddr_in addr)
{

        printf("%s:%d is downloading...\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        char Buf[BUFFER_SIZE] = { 0 };
        int len;
        char filename[80];
        recv(s, filename, 80, 0);
        FILE* fp = fopen(filename, "rb");
        if (fp == NULL)
        {
                sprintf(Buf, "error");
                send(s, Buf, 10, 0);
                return;
        }
        while ((len = fread(Buf, 1, BUFFER_SIZE, fp)) > 0)
        {
                send(s, Buf, len, 0);
        }
        fclose(fp);
        printf("%s 下载完毕\n", filename);
        Record(2, addr, filename);
}
struct servparam
{
	SOCKET sconnect;
	sockaddr_in addrClient;
};
//服务线程
DWORD WINAPI ServerThread(LPVOID lpParameter)
{
	servparam* p = (servparam*)lpParameter;
	while (true)
	{
		char recvBuf[2] = { '9' };//接收缓冲区
		int i = recv(p->sconnect, recvBuf, sizeof(recvBuf), 0);
		if (i == 0 || (recvBuf[0] != '1' && recvBuf[0] != '2'))
		{
			printf("已断开与 %s:%d 的连接！\n", inet_ntoa((p->addrClient).sin_addr), ntohs((p->addrClient).sin_port));
			closesocket(p->sconnect);
			free(p);
			return 0;
		}
		else if (recvBuf[0] == '1')
		{
			UpLoad(p->sconnect, p->addrClient);
		}
		else
		{
			DownLoad(p->sconnect, p->addrClient);
		}
	}
}

int main()
{
        WSADATA wsadata;//用于返回winsock库详细信息
        LoadLib(wsadata);//加载winsock库

	//监听套接字
	SOCKET sListen;
	if (INVALID_SOCKET ==
		(sListen = socket(AF_INET, SOCK_STREAM, 0)))
	{
		printf("fail to create sock：%d...\n", WSAGetLastError());
		return 0;
	}
	//设置监听服务端地址
	sockaddr_in addrListen;
	addrListen.sin_family = AF_INET;
	addrListen.sin_port = htons(PORT);
	addrListen.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//绑定至监听套接字
	if (SOCKET_ERROR ==
		bind(sListen,
			(sockaddr*)&addrListen, sizeof(addrListen)))
	{
		printf("attach failed...\n");
		return 0;
	}
	//进入监听状态
	if (SOCKET_ERROR == listen(sListen, MAX_LISTEN_NUM))
	{
		printf("set listen failed...\n");
		return 0;
	}
	printf("Server is running...\n\n\n");


	//监听线程
	while (true)
	{
		//待传递参数
		servparam* myparam;
		myparam = (servparam*)malloc(sizeof(servparam));
		int clientAddrLen = sizeof(myparam->addrClient);//初始化客户端地址长度

		myparam->sconnect = accept(sListen, (sockaddr*)&(myparam->addrClient), &clientAddrLen);
		if (INVALID_SOCKET == myparam->sconnect)
		{
			printf("connect failed...\n");
			continue;
		}
		printf("主机: %s:%d 已连接至服务器\n", inet_ntoa((myparam->addrClient).sin_addr), ntohs((myparam->addrClient).sin_port));
		CreateThread(NULL, 0, &ServerThread, myparam, 0, NULL);
	}
}
