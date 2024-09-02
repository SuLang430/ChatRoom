#pragma once
#include "inet.h"
#include <windows.h>
class TcpClient : public INet
{
public:
	TcpClient(IMediator* p);
	~TcpClient();
	//SOCKET m_sock;
	//初始化网络
	bool initNet();

	//发送数据
	//UDP协议ip决定发给谁，TCP协议SOCKET决定发给谁
	//ip是ulong类型,SOCKET是uint类型
	bool sendData(char* data, int len, long Send);


	//接收数据
	void recvData();

	//关闭网络
	void unInitNet();

	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bRunning;
};