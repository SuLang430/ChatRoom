#include "TcpClient.h"
#include <process.h>
#include "def.h"
#include "../mediator/TcpClientMediator.h"

TcpClient::TcpClient(IMediator* p):m_bRunning(true),m_handle(nullptr),m_sock(INVALID_SOCKET)
{
	m_pMediator = p;
}
TcpClient::~TcpClient()
{

}
//初始化网络:加载库、创建套接字、连接服务端、创建接收数据的线程
bool TcpClient::initNet()
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
		//WSACleanup();
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
		//WSACleanup();
		return false;
	}
	else
	{
		cout << "sock success " << endl;
	}
	

	//3.连接服务端
	sockaddr_in addServer;
	addServer.sin_family = AF_INET;
	addServer.sin_port = htons(_DEF_TCP_PORT);
	addServer.sin_addr.S_un.S_addr = inet_addr (_DEF_TCP_SERVER_IP);
	err = connect(m_sock, (sockaddr*)&addServer, sizeof(addServer));

	if (SOCKET_ERROR == err)
	{
		cout << "connect error" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "connect success" << endl;
	}
	//4.创建接收数据的线程 
	//CreateThread和ExitThread是一对，如果在线程中使用C++运行时的库函数（例如strcpy），库函数申请空间但是自己不释放
	//ExitThread也不会释放空间，就会有内存泄漏的问题
	//_beginthreadex和_endthreadex是一对，_endthreadex会先回收空间，再调用ExitThread
	m_handle = (HANDLE)_beginthreadex(0,0, &recvThread, this, 0, nullptr);
	
	return true;
}

//线程函数（调用接收数据的函数） 
unsigned __stdcall TcpClient::recvThread(void* lpVoid)
{
	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->recvData();
	return 1;
}
//发送数据
//UDP协议ip决定发给谁，TCP协议SOCKET决定发给谁
//ip是ulong类型,SOCKET是uint类型
bool TcpClient::sendData(char* data, int len, long Send)
{
	//1.校验参数合法性
	if (!data||len<=0)
	{
		cout << "paramater error " << endl;
		return false;
	}
	//2.发送包大小len
	if (send(m_sock,(char*)&len,sizeof(len),0) <= 0)
	{
		cout << "send len error" << WSAGetLastError() << endl;
		return false;
	}
	//3.发送包内容data
	if (send(m_sock, data, len, 0) <= 0)  
	{
		cout << "send data error" << WSAGetLastError() << endl;
		return false;
	}

	return true;
}



//接收数据
void TcpClient::recvData()
{
	int packSize = 0;
	int nRecvNum = 0;
	int offset = 0;
	while (m_bRunning)
	{
		//接收包大小
		nRecvNum = recv(m_sock, (char*)&packSize, sizeof(packSize), 0);
		if (nRecvNum > 0)
		{
			//new指定包大小的空间，用来接收数据
			char* packBuf = new char[packSize];
			//接收包内容
			offset = 0;
			while (packSize>0)
			{
				nRecvNum = recv(m_sock, packBuf + offset, packSize, 0);
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
			//把数据传给中介者类
			m_pMediator->transmitData(packBuf, offset, m_sock);
		}
		else
		{
			cout << "recv1 error" << WSAGetLastError() << endl;
			break;
		}

		 
	}
}

//关闭网络:回收线程资源、关闭套接字、卸载库
void TcpClient::unInitNet()
{
	//1.回收线程资源(创建线程的时候，线程的引用计数是2。结束线程工作，引用计数-1；关闭句柄，引用计数-1) 
	m_bRunning = false;
	if (m_handle)
	{
		//等待线程1000毫秒，WAIT_TIMEOUT代表线程还没有结束
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 1000))
		{
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
	//2.关闭套接字
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	//3.卸载库
	WSACleanup();

}