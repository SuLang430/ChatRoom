#include "UdpMediator.h"


UdpMediator::UdpMediator()
{

}

UdpMediator::~UdpMediator()
{

}

//1.������
bool UdpMediator::openNet()
{
	return true;
}
//2.��������
bool UdpMediator::sendData(char* data, int len, long Send)
{
	return true;
}
//3.�������� ����net���յ������ݴ��������ࣩ
void UdpMediator::transmitData(char* data, int len, long lFrom)
{

}
//4.�ر����� 
void UdpMediator::closeNet()
{

}