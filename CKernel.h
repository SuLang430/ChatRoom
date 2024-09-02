#pragma once
#include <iostream>
#include <map>
#include "mediator/TcpServerMediator.h"
#include "net/def.h"
#include "MySQL/CMySql.h"
#include <string>

using namespace std;

//������ĳ�Ա����ָ��
class CKernel;
typedef void (CKernel::*PFUN)(char* data, int len, long lFrom);

class CKernel
{
public:
	CKernel();
	~CKernel();

	//��ʼ��Э��ͷ����
	void setProtocolFun();

	//1.�򿪷�����
	bool startServer();

	//2.�رշ�����
	void closeServer();

	//3.�������н��յ�������
	void dealData(char* data, int len, long lFrom);

	//����ע������
	void dealRegisterRq(char* data, int len, long lFrom);

	//�����½����
	void dealLoginRq(char* data, int len, long lFrom);

	//������������
	void dealOfflineRq(char* data, int len, long lFrom);	
	
	//������������
	void dealChatRq(char* data, int len, long lFrom);
	//������Ӻ�������
	void dealAddFriendRq(char* data, int len, long lFrom);

	//������Ӻ��ѻظ�
	void dealAddFriendRs(char* data, int len, long lFrom);
	
	//����Ⱥ������
	//void dealGroupChatRq(char* data, int len, long lFrom);

	//��ȡ�Լ������ѵ���Ϣ
	void getInfo(int id);

	//����id��ȡ�û���Ϣ
	void getUserInfo(int userId, _STRU_FRIEND_INFO* info);
public:
	static CKernel* m_pKernel;

private:
	IMediator* m_pMediator;
	//���庯��ָ������
	PFUN m_pProtocolToFun[_DEF_PROTOCOL_COUNT];
	//�������ݿ����
	CMySql m_sql;
	//��¼�ɹ�ʱ���û���socket�����ߵ�ʱ��ɾ���û���socket
	map<int, SOCKET> m_mapIdToSocket;
};

