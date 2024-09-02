#pragma once
#include <iostream>
#include <map>
#include "mediator/TcpServerMediator.h"
#include "net/def.h"
#include "MySQL/CMySql.h"
#include <string>

using namespace std;

//定义类的成员函数指针
class CKernel;
typedef void (CKernel::*PFUN)(char* data, int len, long lFrom);

class CKernel
{
public:
	CKernel();
	~CKernel();

	//初始化协议头数组
	void setProtocolFun();

	//1.打开服务器
	bool startServer();

	//2.关闭服务器
	void closeServer();

	//3.处理所有接收到的数据
	void dealData(char* data, int len, long lFrom);

	//处理注册请求
	void dealRegisterRq(char* data, int len, long lFrom);

	//处理登陆请求
	void dealLoginRq(char* data, int len, long lFrom);

	//处理下线请求
	void dealOfflineRq(char* data, int len, long lFrom);	
	
	//处理聊天请求
	void dealChatRq(char* data, int len, long lFrom);
	//处理添加好友请求
	void dealAddFriendRq(char* data, int len, long lFrom);

	//处理添加好友回复
	void dealAddFriendRs(char* data, int len, long lFrom);
	
	//处理群聊请求
	//void dealGroupChatRq(char* data, int len, long lFrom);

	//获取自己及好友的信息
	void getInfo(int id);

	//根据id获取用户信息
	void getUserInfo(int userId, _STRU_FRIEND_INFO* info);
public:
	static CKernel* m_pKernel;

private:
	IMediator* m_pMediator;
	//定义函数指针数组
	PFUN m_pProtocolToFun[_DEF_PROTOCOL_COUNT];
	//定义数据库对象
	CMySql m_sql;
	//登录成功时存用户的socket，下线的时候删除用户的socket
	map<int, SOCKET> m_mapIdToSocket;
};

