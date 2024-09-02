#include "TcpServerMediator.h"
#include "../net/TcpServer.h"

TcpServerMediator::TcpServerMediator()
{
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator()
{
	if (m_pNet)
	{
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//1.打开网络
bool TcpServerMediator::openNet()
{
	return m_pNet->initNet();
}
//2.发送数据
bool TcpServerMediator::sendData(char* data, int len, long Send)
{
	return m_pNet->sendData(data,len,Send);
}
//3.传递数据 （把net接收到的数据传给处理类）
void TcpServerMediator::transmitData(char* data, int len, long lFrom)
{
	//TODO：把net接收到的数据传给处理类
	//测试代码：打印接收到的数据
		cout << "transmitData:" << data << endl;
	//给客户端回
		sendData((char*)"ni hao", sizeof("ni hao"), lFrom);
}
//4.关闭网络 
void TcpServerMediator::closeNet()
{
	return m_pNet->unInitNet();
}
