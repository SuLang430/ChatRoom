#pragma once

//#include "../net/inet.h"

//����һ����INet���࣬��ʹ�ã��Ժ�ͻ���뵽�����
class INet;
class IMediator
{
public:
	IMediator() {}
	virtual ~IMediator() {}

	//1.������
	virtual bool openNet() = 0;
	//2.��������
	virtual bool sendData(char* data, int len, long Send) = 0;
	//3.�������� ����net���յ������ݴ��������ࣩ
	virtual void transmitData(char* data, int len, long lFrom) = 0;
	//4.�ر����� 
	virtual void closeNet() = 0;

protected:
	INet* m_pNet;
};