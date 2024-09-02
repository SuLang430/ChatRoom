#pragma once
#include <iostream>
#include<WinSock2.h>
//#include "../mediator/IMediator.h"

#pragma comment(lib,"Ws2_32.lib")
using namespace std;

class IMediator;
class INet
{
public:
	INet() {}
	virtual ~INet() {}
	//��ʼ������
	virtual bool initNet() = 0;

	//��������
	//UDPЭ��ip��������˭��TCPЭ��SOCKET��������˭
	//ip��ulong����,SOCKET��uint����
	virtual bool sendData(char* data, int len, long Send) = 0;


	//��������
	virtual void recvData() = 0;

	//�ر�����
	virtual void unInitNet() = 0;

protected:
	IMediator* m_pMediator;
};