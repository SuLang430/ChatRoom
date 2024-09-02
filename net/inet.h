#pragma once
#include <iostream>
#include<WinSock2.h>
//#include "../mediator/IMediator.h"

#pragma comment(lib,"Ws2_32.lib")
using namespace std;

class IMediator;
class INet
{
public:
	INet() {}
	virtual ~INet() {}
	//初始化网络
	virtual bool initNet() = 0;

	//发送数据
	//UDP协议ip决定发给谁，TCP协议SOCKET决定发给谁
	//ip是ulong类型,SOCKET是uint类型
	virtual bool sendData(char* data, int len, long Send) = 0;


	//接收数据
	virtual void recvData() = 0;

	//关闭网络
	virtual void unInitNet() = 0;

protected:
	IMediator* m_pMediator;
};