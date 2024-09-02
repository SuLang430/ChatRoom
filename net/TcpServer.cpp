#include "TcpServer.h"
#include "def.h"
#include <process.h>
#include "../mediator/TcpServerMediator.h"

TcpServer::TcpServer(IMediator* p):m_sock(INVALID_SOCKET),m_bRunning(true)
{
	m_pMediator = p;
}
TcpServer::~TcpServer()
{

}
//初始化网络:加载库、创建套接字、绑定ip和端口号、监听、创建接受连接的线程
bool TcpServer::initNet()
{
	//1.加载库
	WORD version = MAKEWORD(2, 2);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
	if (0 != err)
	{
		cout << "WSAStartup fail" << endl;
		return false;
	}

	if (2 != HIBYTE(data.wVersion) || 2 != LOBYTE(data.wVersion))
	{
		cout << "WSAStartup version error" << endl;
		return false;
	}
	else
	{
		cout << "WSAStartup success" << endl;
	}

	//2.创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock)
	{
		cout << "socket error " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "socket success " << endl;
	}

	//3.绑定ip和端口号
	sockaddr_in addServer;
	addServer.sin_family = AF_INET;
	addServer.sin_port = htons(_DEF_TCP_PORT);
	addServer.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addServer, sizeof(addServer));

	if (SOCKET_ERROR == err)
	{
		cout << "bind error" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "bind success" << endl;
	}
	//4.监听
	err = listen(m_sock, 10);
	if (SOCKET_ERROR == err)
	{
		cout << "listen error" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "listen success" << endl;
	}
	//5.创建接收连接的线程
	HANDLE handle = (HANDLE)_beginthreadex(0, 0, &acceptThread,this, 0, nullptr);
	if (handle)
	{
		m_listHandle.push_back(handle);
	}

	return true;
}

//接收连接的线程函数（循环等待接收连接）
unsigned __stdcall TcpServer::acceptThread(void* IpVoid)
{
	TcpServer* pThis = (TcpServer*)IpVoid; 
	sockaddr_in addClient;
	int addClientSize = sizeof(addClient);
	SOCKET sockTalk = INVALID_SOCKET;
	unsigned int threadId = 0;
	HANDLE handle = nullptr;
	while (pThis->m_bRunning)
	{
		//接收连接
		sockTalk = accept(pThis->m_sock, (sockaddr*)&addClient, &addClientSize);
		if ( INVALID_SOCKET == sockTalk)
		{
			//失败，打印错误日志，退出循环
			cout << "accept error" << WSAGetLastError() << endl;
			break;
		}
		else
		{
			//连接成功，打印出客户端ip地址
			cout << "client in:" << inet_ntoa(addClient.sin_addr) << endl;
		
			//给连接成功的客户端创建一个接收数据的线程
			handle = (HANDLE)_beginthreadex(0, 0, &recvThread, pThis, 0, &threadId);
			if (handle)
			{
				pThis->m_listHandle.push_back(handle);
			}
			//把跟当前客户端通信的socket保存起来
			pThis->m_mapThreadIdToSocket[threadId] = sockTalk;
		}
	}
	return 1;
}

//线程函数（调用接收数据的函数） 
unsigned __stdcall TcpServer::recvThread(void* IpVoid)
{
	TcpServer* pThis = (TcpServer*)IpVoid;
	pThis->recvData();
	return 1;
}

//发送数据
//UDP协议ip决定发给谁，TCP协议SOCKET决定发给谁
//ip是ulong类型,SOCKET是uint类型
bool TcpServer::sendData(char* data, int len, long Send)
{
	//1.校验参数合法性
	if (!data || len <= 0)
	{
		cout << "paramater error " << endl;
		return false;
	}
	//2.发送包大小len
	if (send(Send, (char*)&len, sizeof(len), 0) <= 0)
	{
		cout << "send len error" << WSAGetLastError() << endl;
		return false;
	}
	//3.发送包内容data
	if (send(Send, data, len, 0) <= 0)
	{
		cout << "send data error" << WSAGetLastError() << endl;
		return false;
	}

	return true;
}


//接收数据
void TcpServer::recvData()
{
	//获取当前线程的id
	//获取当前线程的socket
	SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
	if (!sock||INVALID_SOCKET == sock)
	{
		cout << "sock error" << endl;
		return;
	}

	int packSize = 0;
	int nRecvNum = 0;
	int offset = 0;
	while (m_bRunning)
	{
		//接收包大小
		nRecvNum = recv(sock, (char*)&packSize, sizeof(packSize), 0);
		if (nRecvNum > 0)
		{
			//new指定包大小的空间，用来接收数据
			char* packBuf = new char[packSize];
			//接收包内容
			offset = 0;
			while (packSize > 0)
			{
				nRecvNum = recv(sock, packBuf + offset, packSize, 0);
				if (nRecvNum > 0)
				{
					offset += nRecvNum;
					packSize -= nRecvNum;
				}
				else
				{
					cout << "recv2 error" << WSAGetLastError() << endl;
					break;
				}
			}
			//一个包接收完成了
			//TODO: 把数据传给中介者类
			m_pMediator->transmitData(packBuf,offset,sock);
		}
		else
		{
			cout << "recv1 error" << WSAGetLastError() << endl;
			break;
		}


	}
}

//关闭网络：回收线程资源、关闭套接字、卸载库
void TcpServer::unInitNet()
{
	//1.回收线程资源
	m_bRunning = false;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();)
	{
		HANDLE handle = *ite;

		if (handle)
		{
			//等待线程1000毫秒，WAIT_TIMEOUT代表线程还没有结束
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 1000))
			{
				TerminateThread(handle, -1);
			}
			CloseHandle(handle);
			handle = nullptr;
		}
		//把当前节点从list中移除
		ite = m_listHandle.erase(ite);
	}
	//2.关闭套接字
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();)
	{
		SOCKET sock = ite->second;
		if (sock && INVALID_SOCKET != sock)
		{
			closesocket(sock);
		}
		//把当前节点从map中移除
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	//3.卸载库
	WSACleanup();
}
