#pragma once

#include "IMediator.h"

class TcpClientMediator:public IMediator
{
public:
	TcpClientMediator();
	~TcpClientMediator();

	//1.������
	 bool openNet() ;
	//2.��������
	 bool sendData(char* data, int len, long Send) ;
	//3.�������� ����net���յ������ݴ��������ࣩ
	 void transmitData(char* data, int len, long lFrom) ;
	//4.�ر����� 
	 void closeNet() ;

};