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

//1.������
bool TcpClientMediator::openNet()
{
	return m_pNet->initNet();
}
//2.��������
bool TcpClientMediator::sendData(char* data, int len, long Send)
{
	return m_pNet->sendData(data, len, Send);
}
//3.�������� ����net���յ������ݴ��������ࣩ
void TcpClientMediator::transmitData(char* data, int len, long lFrom)
{
	cout << "transmitData:" << data << endl;
}
//4.�ر����� 
void TcpClientMediator::closeNet()
{
	return m_pNet->unInitNet();
}