#pragma once
#include "inet.h"
#include <map>
#include <list>
class TcpServer : public INet
{
public:
	TcpServer(IMediator* p);
	~TcpServer();
	//初始化网络
	bool initNet();

	//发送数据
	//UDP协议ip决定发给谁，TCP协议SOCKET决定发给谁
	//ip是ulong类型,SOCKET是uint类型
	bool sendData(char* data, int len, long send);


	//接收数据
	void recvData();

	//关闭网络
	void unInitNet();

	static unsigned __stdcall acceptThread(void* lpVoid);

	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;		//监听和接收连接
	bool m_bRunning;
	map<unsigned int, SOCKET> m_mapThreadIdToSocket;	//跟客户端通信
	list<HANDLE> m_listHandle;
};




//vector:向量，如果知道下标，查找快；不知道下标，查找慢。增加和删除麻烦
//list：增加和删除容易，查找慢
//queue: 先进先出
//stack: 先进后出
//map: 查找快，<key,value>，适合保存一对一的关系
//set: 查找快，只有一个值 ，