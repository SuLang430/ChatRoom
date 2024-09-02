#include "TcpServerMediator.h"
#include "../net/TcpServer.h"
#include "../CKernel.h"

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

//1.������
bool TcpServerMediator::openNet()
{
	return m_pNet->initNet();
}
//2.��������
bool TcpServerMediator::sendData(char* data, int len, long Send)
{
	return m_pNet->sendData(data,len,Send);
}
//3.�������� ����net���յ������ݴ��������ࣩ
void TcpServerMediator::transmitData(char* data, int len, long lFrom)
{
	//��net���յ������ݴ���������
	CKernel::m_pKernel->dealData(data, len, lFrom);
	
}
//4.�ر����� 
void TcpServerMediator::closeNet()
{
	return m_pNet->unInitNet();
}
