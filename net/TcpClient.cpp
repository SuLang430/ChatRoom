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
//��ʼ������:���ؿ⡢�����׽��֡����ӷ���ˡ������������ݵ��߳�
bool TcpClient::initNet()
{
	//1.���ؿ� 
	
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

	//2.�����׽���
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
	

	//3.���ӷ����
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
	//4.�����������ݵ��߳� 
	//CreateThread��ExitThread��һ�ԣ�������߳���ʹ��C++����ʱ�Ŀ⺯��������strcpy�����⺯������ռ䵫���Լ����ͷ�
	//ExitThreadҲ�����ͷſռ䣬�ͻ����ڴ�й©������
	//_beginthreadex��_endthreadex��һ�ԣ�_endthreadex���Ȼ��տռ䣬�ٵ���ExitThread
	m_handle = (HANDLE)_beginthreadex(0,0, &recvThread, this, 0, nullptr);
	
	return true;
}

//�̺߳��������ý������ݵĺ����� 
unsigned __stdcall TcpClient::recvThread(void* lpVoid)
{
	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->recvData();
	return 1;
}
//��������
//UDPЭ��ip��������˭��TCPЭ��SOCKET��������˭
//ip��ulong����,SOCKET��uint����
bool TcpClient::sendData(char* data, int len, long Send)
{
	//1.У������Ϸ���
	if (!data||len<=0)
	{
		cout << "paramater error " << endl;
		return false;
	}
	//2.���Ͱ���Сlen
	if (send(m_sock,(char*)&len,sizeof(len),0) <= 0)
	{
		cout << "send len error" << WSAGetLastError() << endl;
		return false;
	}
	//3.���Ͱ�����data
	if (send(m_sock, data, len, 0) <= 0)  
	{
		cout << "send data error" << WSAGetLastError() << endl;
		return false;
	}

	return true;
}



//��������
void TcpClient::recvData()
{
	int packSize = 0;
	int nRecvNum = 0;
	int offset = 0;
	while (m_bRunning)
	{
		//���հ���С
		nRecvNum = recv(m_sock, (char*)&packSize, sizeof(packSize), 0);
		if (nRecvNum > 0)
		{
			//newָ������С�Ŀռ䣬������������
			char* packBuf = new char[packSize];
			//���հ�����
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
			//һ�������������
			//�����ݴ����н�����
			m_pMediator->transmitData(packBuf, offset, m_sock);
		}
		else
		{
			cout << "recv1 error" << WSAGetLastError() << endl;
			break;
		}

		 
	}
}

//�ر�����:�����߳���Դ���ر��׽��֡�ж�ؿ�
void TcpClient::unInitNet()
{
	//1.�����߳���Դ(�����̵߳�ʱ���̵߳����ü�����2�������̹߳��������ü���-1���رվ�������ü���-1) 
	m_bRunning = false;
	if (m_handle)
	{
		//�ȴ��߳�1000���룬WAIT_TIMEOUT�����̻߳�û�н���
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 1000))
		{
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
	//2.�ر��׽���
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	//3.ж�ؿ�
	WSACleanup();

}