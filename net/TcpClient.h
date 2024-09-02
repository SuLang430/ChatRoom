#pragma once
#include "inet.h"
#include <windows.h>
class TcpClient : public INet
{
public:
	TcpClient(IMediator* p);
	~TcpClient();
	//SOCKET m_sock;
	//��ʼ������
	bool initNet();

	//��������
	//UDPЭ��ip��������˭��TCPЭ��SOCKET��������˭
	//ip��ulong����,SOCKET��uint����
	bool sendData(char* data, int len, long Send);


	//��������
	void recvData();

	//�ر�����
	void unInitNet();

	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bRunning;
};