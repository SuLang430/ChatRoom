#pragma once
#include "inet.h"
class UdpNet: public INet
{
public:
	UdpNet();
	~UdpNet();
	//��ʼ������
	 bool initNet() ;

	//��������
	//UDPЭ��ip��������˭��TCPЭ��SOCKET��������˭
	//ip��ulong����,SOCKET��uint����
	 bool sendData(char* data, int len, long send) ;


	//��������
	 void recvData() ;

	//�ر�����
	 void unInitNet();


};