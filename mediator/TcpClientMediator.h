#pragma once

#include "IMediator.h"

class TcpClientMediator:public IMediator
{
    Q_OBJECT
signals:
    //把net接收到的数据传给处理类
    void sig_transmitData(char* data, int len, long lFrom);
public:
	TcpClientMediator();
	~TcpClientMediator();

	//1.打开网络
     bool openNet();
	//2.发送数据
     bool sendData(char* data, int len, long Send);
	//3.传递数据 （把net接收到的数据传给处理类）
     void transmitData(char* data, int len, long lFrom);
	//4.关闭网络 
     void closeNet();

};
