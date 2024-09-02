#pragma once

//#include "../net/inet.h"

//声明一个叫INet的类，先使用，稍后就会编译到这个类
class INet;
class IMediator
{
public:
	IMediator() {}
	virtual ~IMediator() {}

	//1.打开网络
	virtual bool openNet() = 0;
	//2.发送数据
	virtual bool sendData(char* data, int len, long Send) = 0;
	//3.传递数据 （把net接收到的数据传给处理类）
	virtual void transmitData(char* data, int len, long lFrom) = 0;
	//4.关闭网络 
	virtual void closeNet() = 0;

protected:
	INet* m_pNet;
};