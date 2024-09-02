#include "TcpClientMediator.h"
#include "../net/TcpClient.h"
TcpClientMediator::TcpClientMediator()
{
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator()
{
	if (m_pNet)
	{
		m_pNet -> unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//1.打开网络
bool TcpClientMediator::openNet()
{
	return m_pNet->initNet();
}
//2.发送数据
bool TcpClientMediator::sendData(char* data, int len, long Send)
{
	return m_pNet->sendData(data, len, Send);
}
//3.传递数据 （把net接收到的数据传给处理类）
void TcpClientMediator::transmitData(char* data, int len, long lFrom)
{
    //把net接收到的数据传给处理类
    Q_EMIT sig_transmitData(data, len, lFrom);
}
//4.关闭网络 
void TcpClientMediator::closeNet()
{
	return m_pNet->unInitNet();
}
