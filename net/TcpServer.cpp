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
//��ʼ������:���ؿ⡢�����׽��֡���ip�Ͷ˿ںš������������������ӵ��߳�
bool TcpServer::initNet()
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
		return false;
	}
	else
	{
		cout << "socket success " << endl;
	}

	//3.��ip�Ͷ˿ں�
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
	//4.����
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
	//5.�����������ӵ��߳�
	HANDLE handle = (HANDLE)_beginthreadex(0, 0, &acceptThread,this, 0, nullptr);
	if (handle)
	{
		m_listHandle.push_back(handle);
	}

	return true;
}

//�������ӵ��̺߳�����ѭ���ȴ��������ӣ�
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
		//��������
		sockTalk = accept(pThis->m_sock, (sockaddr*)&addClient, &addClientSize);
		if ( INVALID_SOCKET == sockTalk)
		{
			//ʧ�ܣ���ӡ������־���˳�ѭ��
			cout << "accept error" << WSAGetLastError() << endl;
			break;
		}
		else
		{
			//���ӳɹ�����ӡ���ͻ���ip��ַ
			cout << "client in:" << inet_ntoa(addClient.sin_addr) << endl;
		
			//�����ӳɹ��Ŀͻ��˴���һ���������ݵ��߳�
			handle = (HANDLE)_beginthreadex(0, 0, &recvThread, pThis, 0, &threadId);
			if (handle)
			{
				pThis->m_listHandle.push_back(handle);
			}
			//�Ѹ���ǰ�ͻ���ͨ�ŵ�socket��������
			pThis->m_mapThreadIdToSocket[threadId] = sockTalk;
		}
	}
	return 1;
}

//�̺߳��������ý������ݵĺ����� 
unsigned __stdcall TcpServer::recvThread(void* IpVoid)
{
	TcpServer* pThis = (TcpServer*)IpVoid;
	pThis->recvData();
	return 1;
}

//��������
//UDPЭ��ip��������˭��TCPЭ��SOCKET��������˭
//ip��ulong����,SOCKET��uint����
bool TcpServer::sendData(char* data, int len, long Send)
{
	//1.У������Ϸ���
	if (!data || len <= 0)
	{
		cout << "paramater error " << endl;
		return false;
	}
	//2.���Ͱ���Сlen
	if (send(Send, (char*)&len, sizeof(len), 0) <= 0)
	{
		cout << "send len error" << WSAGetLastError() << endl;
		return false;
	}
	//3.���Ͱ�����data
	if (send(Send, data, len, 0) <= 0)
	{
		cout << "send data error" << WSAGetLastError() << endl;
		return false;
	}

	return true;
}


//��������
void TcpServer::recvData()
{
	//��ȡ��ǰ�̵߳�id
	//��ȡ��ǰ�̵߳�socket
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
		//���հ���С
		nRecvNum = recv(sock, (char*)&packSize, sizeof(packSize), 0);
		if (nRecvNum > 0)
		{
			//newָ������С�Ŀռ䣬������������
			char* packBuf = new char[packSize];
			//���հ�����
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
			//һ�������������
			//TODO: �����ݴ����н�����
			m_pMediator->transmitData(packBuf,offset,sock);
		}
		else
		{
			cout << "recv1 error" << WSAGetLastError() << endl;
			break;
		}


	}
}

//�ر����磺�����߳���Դ���ر��׽��֡�ж�ؿ�
void TcpServer::unInitNet()
{
	//1.�����߳���Դ
	m_bRunning = false;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();)
	{
		HANDLE handle = *ite;

		if (handle)
		{
			//�ȴ��߳�1000���룬WAIT_TIMEOUT�����̻߳�û�н���
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 1000))
			{
				TerminateThread(handle, -1);
			}
			CloseHandle(handle);
			handle = nullptr;
		}
		//�ѵ�ǰ�ڵ��list���Ƴ�
		ite = m_listHandle.erase(ite);
	}
	//2.�ر��׽���
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
		//�ѵ�ǰ�ڵ��map���Ƴ�
		ite = m_mapThreadIdToSocket.erase(ite);
	}
	//3.ж�ؿ�
	WSACleanup();
}
