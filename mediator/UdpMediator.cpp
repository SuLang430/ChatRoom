#include "UdpMediator.h"


UdpMediator::UdpMediator()
{

}

UdpMediator::~UdpMediator()
{

}

//1.打开网络
bool UdpMediator::openNet()
{
	return true;
}
//2.发送数据
bool UdpMediator::sendData(char* data, int len, long Send)
{
	return true;
}
//3.传递数据 （把net接收到的数据传给处理类）
void UdpMediator::transmitData(char* data, int len, long lFrom)
{

}
//4.关闭网络 
void UdpMediator::closeNet()
{

}