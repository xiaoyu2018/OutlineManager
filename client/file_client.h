#ifndef FILE_CLIENT_H
#define FILE_CLIENT_H
#include<WinSock2.h>

#define PORT 6000 //远端服务器监听端口号
#define BUFFER_SIZE 1024 //数据缓冲区大小
#define DES_IP "127.0.0.1"//服务器地址

//上传文件至服务器
bool UpLoadToSERVR(SOCKET &s,const char*filePath,const char*fileName);
//下载文件从服务器
bool DownLoadFromSERVR(SOCKET& s,const char* fileName);
//连接至文件服务器
bool ConnecT2Server(SOCKET& s);

#endif // FILE_CLIENT_H
