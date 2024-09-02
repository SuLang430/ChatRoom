#include "UdpNet.h"

UdpNet::UdpNet()
{

}
UdpNet::~UdpNet()
{

}
	//初始化网络
bool UdpNet::initNet()
{
	return true;
}

	//发送数据
	//UDP协议ip决定发给谁，TCP协议SOCKET决定发给谁
	//ip是ulong类型,SOCKET是uint类型
bool UdpNet::sendData(char* data, int len, long send)
{
	return true;
}


	//接收数据
void UdpNet::recvData()
{

}

	//关闭网络
void UdpNet::unInitNet()
{

}


