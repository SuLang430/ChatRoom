#pragma once
#include "inet.h"
#include <map>
#include <list>
class TcpServer : public INet
{
public:
	TcpServer(IMediator* p);
	~TcpServer();
	//��ʼ������
	bool initNet();

	//��������
	//UDPЭ��ip��������˭��TCPЭ��SOCKET��������˭
	//ip��ulong����,SOCKET��uint����
	bool sendData(char* data, int len, long send);


	//��������
	void recvData();

	//�ر�����
	void unInitNet();

	static unsigned __stdcall acceptThread(void* lpVoid);

	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;		//�����ͽ�������
	bool m_bRunning;
	map<unsigned int, SOCKET> m_mapThreadIdToSocket;	//���ͻ���ͨ��
	list<HANDLE> m_listHandle;
};




//vector:���������֪���±꣬���ҿ죻��֪���±꣬�����������Ӻ�ɾ���鷳
//list�����Ӻ�ɾ�����ף�������
//queue: �Ƚ��ȳ�
//stack: �Ƚ����
//map: ���ҿ죬<key,value>���ʺϱ���һ��һ�Ĺ�ϵ
//set: ���ҿ죬ֻ��һ��ֵ ��