#pragma once
#include "inet.h"
class UdpNet: public INet
{
public:
	UdpNet();
	~UdpNet();
	//初始化网络
	 bool initNet() ;

	//发送数据
	//UDP协议ip决定发给谁，TCP协议SOCKET决定发给谁
	//ip是ulong类型,SOCKET是uint类型
	 bool sendData(char* data, int len, long send) ;


	//接收数据
	 void recvData() ;

	//关闭网络
	 void unInitNet();


};