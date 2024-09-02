#include "CKernel.h"
CKernel* CKernel::m_pKernel = nullptr;
CKernel::CKernel():m_pMediator(nullptr)
{
	m_pKernel = this;
	m_pMediator = new TcpServerMediator;
}
CKernel::~CKernel()
{
	
}
//��ʼ��Э��ͷ����
void CKernel::setProtocolFun()
{
	//��ʼ������
	memset(m_pProtocolToFun, 0, sizeof(m_pProtocolToFun));

	//��Э��ͷ�ͺ���
	m_pProtocolToFun[_DEF_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_pProtocolToFun[_DEF_LOGIN_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_pProtocolToFun[_DEF_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_pProtocolToFun[_DEF_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_pProtocolToFun[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_pProtocolToFun[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
	//m_pProtocolToFun[_DEF_GROUP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealGroupChatRq;
}

//�򿪷�����
bool CKernel::startServer()
{
	//��ʼ��Э��ͷ����
	setProtocolFun();
	//��ʼ������
	if (!m_pMediator->openNet())
	{
		cout << "openNet false" << endl;
		return false;
	}
	//�������ݿ�
	char ip[] = "127.0.0.1";
	char name[] = "root";
	char pass[] = "SB_SL_312";
	char db[] = "20230912im";
	if (!m_sql.ConnectMySql(ip,name,pass,db) )
	{
		cout << "connect mysql false" << endl;
		return false;
	}
	else
	{
		cout << "connect mysql success" << endl;
		
	}
	return true;
}

//�رշ�����
void CKernel::closeServer()
{
	if (m_pMediator)
	{
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}

//3.�������н��յ�������
void CKernel::dealData(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	
	/*_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	cout << "name:" << rq->name << ", pas:" << rq->password << ", tel:" << rq->tel << endl;
	*/
	/*_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	cout << "pas:" << rq->password << ", tel:" << rq->tel << endl;*/

	//����Э��ͷ���ж��յ������ĸ��ṹ�壬���ĸ���������ͬ�����߲�ͬ��������
	PackType type = *(PackType*) data;
	//���������±�
	int index = type - _DEF_PROTOCOL_BASE - 1;
	//�ж��±��Ƿ�����Ч��Χ��
	if (index>=0&&index<_DEF_PROTOCOL_COUNT)
	{

		//ȡ��������ַ
		PFUN pf = m_pProtocolToFun[index];
		if (pf)
		{
			(this->*pf)(data, len, lFrom);
		}
		else
		{
			cout << "type2 error:" << type << endl;
		}
	}
	else
	{
		cout << "type1 error:" << type << endl;
	}

	delete[] data;
}

//����ע������
void CKernel::dealRegisterRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	// 1�����
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;

	// 2��У�����ݺϷ��ԣ�����Ϳͻ���һ�£�


	// 3�������ǳƲ�ѯ�ǳ�
	list<string> lstStr;
	char szSql[1024] = "select name from t_user where name = ";
	sprintf(szSql, "select name from t_user where name = '%s';", rq->name);
	if (!m_sql.SelectMySql(szSql,1,lstStr))
	{
		//��ѯ���ݿ�ʧ������ԭ��1.δ�������ݿ������ޱ�2.sql������
		cout << "select sql false" << szSql << endl;
		return;
	}

	// 4���жϲ�ѯ����Ƿ�Ϊ��
	_STRU_REGISTER_RS rs;
	if (lstStr.size() != 0) {
		// 5����ѯ�����Ϊ�գ�ע��ʧ�ܣ�_register_name_exists
		rs.result = _register_name_exists;
	}
	else {
		// 6�����ݵ绰�����ѯ�绰����
		sprintf(szSql, "select name from t_user where tel = '%s';", rq->tel);
		if (!m_sql.SelectMySql(szSql, 1, lstStr))
		{
			//��ѯ���ݿ�ʧ������ԭ��1.δ�������ݿ������ޱ�2.sql������
			cout << "select sql false" << szSql << endl;
			return;
		}
		// 7���жϲ�ѯ����Ƿ�Ϊ��
		if (lstStr.size() != 0) {
			// 8����ѯ�����Ϊ�գ�ע��ʧ�ܣ�_register_tel_exists
			rs.result = _register_tel_exists;
		}
		else {
			// 9�����û���ע����Ϣд�����ݿ�
			sprintf(szSql, "insert into t_user(name,tel,password,feeling,iconid) values ('%s','%s','%s','nihao','4');", rq->name,rq->tel,rq->password);
			if (!m_sql.UpdateMySql(szSql))
			{
				cout << "insert mysql false" << szSql << endl;
				return;
			}
			// 10��ע��ɹ���_register_success
			rs.result = _register_success;
		}
	}

	// 11������ע�����Ƿ�ɹ�������Ҫ���ͻ��˻ظ�
	m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);
}

//�����½����
void CKernel::dealLoginRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	//1.���
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;

	//2.У������Ϸ���(����Ϳͻ���һ��)

	//3.���ݵ绰�����ѯ����
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select password,id from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(szSql, 2, lstStr))
	{
		//��ѯ���ݿ�ʧ������ԭ��1.δ�������ݿ������ޱ�2.sql������
		cout << "select sql false" << szSql << endl;
		return;
	}

	//4.�жϲ�ѯ����Ƿ�Ϊ��
	_STRU_LOGIN_RS rs;
	if (0 == lstStr.size())
	{
		//5.��ѯ���Ϊ�գ����ֻ���δע�ᣬ��¼ʧ��
		rs.result = _login_tel_not_exists;
	}
	else
	{
		//��list��ȡ������
		string pass = lstStr.front();
		lstStr.pop_front();	//ȡ������ݴ�list��ɾ��
		
		//��list��ȡ��id
		int id = stoi(lstStr.front());
		lstStr.pop_front();
		
		//6.�Ƚϲ�ѯ��������͵�¼ʱ�������Ƿ�һ��
	
		if (0 == strcmp(pass.c_str(),rq->password))
		{
			//7.������ͬ����¼�ɹ�
			rs.result = _login_success;
			rs.userId = id; 

			//9.����ʲô�������Ҫ֪ͨ�ͻ���
			m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);

			//���浱ǰ��¼�û���id��socket
			m_mapIdToSocket[id] = lFrom;
			//��ȡ�Լ������ѵ���Ϣ
			getInfo(id);

		

		}
		else
		{
			//8.������󣬵�¼ʧ��
			rs.result = _login_password_error;
		}
	
	}

	//9.����ʲô�������Ҫ֪ͨ�ͻ���
	m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);
}

void CKernel::getInfo(int id)
{
	cout << __func__ << endl;
	//�����Լ���id��ѯ�Լ�����Ϣ
	_STRU_FRIEND_INFO userInfo;
	getUserInfo(id, &userInfo);
	//���Լ�����Ϣ�����ͻ���
	if (m_mapIdToSocket.find(id)!=m_mapIdToSocket.end())
	{
		m_pMediator->sendData((char*)&userInfo, sizeof(userInfo),m_mapIdToSocket[id]);
	}
	
	//�����Լ���id��ѯ���ѵ�id�б�
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select idA from t_friend where idB = %d;", id);
	if (!m_sql.SelectMySql(szSql, 1, lstStr))
	{
		//��ѯ���ݿ�ʧ������ԭ��1.δ�������ݿ������ޱ�2.sql������
		cout << "select sql false" << szSql << endl;
		return;
	}

	//������ѯ���
	int friendId = 0;
	_STRU_FRIEND_INFO friendInfo;
	while (lstStr.size()>0)
	{
		//ȡ������id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		cout << "friendId:" << friendId << endl;
		//���ݺ���id��ѯ������Ϣ
		getUserInfo(friendId, &friendInfo);

		//�Ѻ�����Ϣ�����ͻ���
		if (m_mapIdToSocket.count(id)>0)
		{
			m_pMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[id]);
		}

		//�жϺ����Ƿ����ߣ����߾�֪ͨ����
		if (m_mapIdToSocket.count(friendId)>0)
		{
			m_pMediator->sendData((char*)&userInfo,sizeof(userInfo) , m_mapIdToSocket[friendId]);
		}
	}
}

//����id��ȡ�û���Ϣ
void CKernel::getUserInfo(int userId, _STRU_FRIEND_INFO* info)
{
	cout << __func__ << endl;
	info->friendId = userId;
	if (m_mapIdToSocket.count(userId)>0)
	{
		//����
		info->status = _status_online;
	}
	else
	{
		//����
		info->status = _status_offline;
	}

	//�����ݿ��ѯ�û����ǳơ�ǩ����ͷ��id
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select name, feeling, iconid from t_user where id = '%d';", userId);
	if (!m_sql.SelectMySql(szSql, 3, lstStr))
	{
		//��ѯ���ݿ�ʧ������ԭ��1.δ�������ݿ������ޱ�2.sql������
		cout << "select sql false" << szSql << endl;
		return;
	}

	if (3 == lstStr.size())
	{
		//��ʼ���ṹ���ֵ
		string name = lstStr.front();
		lstStr.pop_front();
		strcpy_s(info->friendName, sizeof(info->friendName),name.c_str());
	
		string feeling = lstStr.front();
		lstStr.pop_front();
		strcpy_s(info->feeling, sizeof(info->feeling), feeling.c_str());

		int iconid = stoi(lstStr.front());
		lstStr.pop_front();
		info->iconId = iconid;
	}
	else
	{
		cout << "lstStr size = " << lstStr.size() << endl;
	}
}

//������������
void CKernel::dealOfflineRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;

	//1.���
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

	//2.�����û�id��ѯ����id�б�
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select idA from t_friend where idB = %d;", rq->userId);
	if (!m_sql.SelectMySql(szSql, 1, lstStr))
	{
		//��ѯ���ݿ�ʧ������ԭ��1.δ�������ݿ������ޱ�2.sql������
		cout << "select sql false" << szSql << endl;
		return;
	}
	
	//3.����=����id�б�
	int friendId = 0;
	while (lstStr.size() > 0)
	{
		//4.ȡ������id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		//5.�жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			//6.֪ͨ���ߺ��ѣ�ת��������������ѣ�
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}

	//7.����socket����map���Ƴ�
	if (m_mapIdToSocket.count(rq->userId) > 0)
	{
		SOCKET sock = m_mapIdToSocket[rq->userId];
		closesocket(sock);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//������������
void CKernel::dealChatRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	
	//1.���
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

	//2.�жϺ����Ƿ�����
	if (m_mapIdToSocket.count(rq->friendId) > 0)
	{
		//3.�������ߣ�����������ת��������B�û�
		m_pMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//4.���Ѳ����ߣ�����A�û�����ʧ��
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		rs.result = _send_fail;
		m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);

	}
}

//������Ӻ�������
void CKernel::dealAddFriendRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	//1.���
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2.���ݺ��ѵ��ǳƲ�ѯ���ѵ�id
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(szSql, 1, lstStr) ){
		//��ѯ���ݿ�ʧ������ԭ��1.û�������ݿ���߿���û�б�2.sql���д����
		cout<<"��ѯ���ݿ�ʧ��"<<szSql<<endl;
			return;
	}

	//3.�жϲ�ѯ����Ƿ�Ϊ��
	_STRU_ADD_FRIEND_RS rs;
	if (0 == lstStr.size()) {
		//4.�����ѯ���Ϊ�գ���Ӻ���ʧ�ܣ�_add_friend_not_exists
		rs.result = _add_friend_not_exists;
		strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
		//�ѽ�����ظ��ͻ���
		m_pMediator->sendData((char*)&rs,sizeof(rs),lFrom);
	}
	else {
		//5.��ѯ�����Ϊ�գ��Ӳ�ѯ�����ȡ�����ѵ�id
		int friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//6.�жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0) {
			//7.�������ߣ�����Ӻ�������ת��������
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//8.���Ѳ����ߣ���Ӻ���ʧ�ܣ�_add_friend_offline
			rs.result = _add_friend_offline;
			strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
			//�ѽ�����ظ��ͻ���
			m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);
		}
	}
}

//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	//1.���
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;

	//2.�ж���ӽ���Ƿ�ɹ�
	if (_add_friend_success == rs->result)
	{
		//3.�������˵ĺ��ѹ�ϵд�����ݿ�
		char sqlBuf[1024] = "";
		sprintf(sqlBuf, "insert into t_friend values (%d, %d);", rs->friendId, rs->userId );
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "�������ݿ�ʧ�ܣ�" << sqlBuf << endl;
			return;
		}

		sprintf(sqlBuf, "insert into t_friend values (%d, %d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "�������ݿ�ʧ�ܣ�" << sqlBuf << endl;
			return;
		}
		//4.���¿ͻ��˺����б�
		getInfo(rs->friendId);

	}
	//���ܳɹ��Ƿ�Ҫת����A�ͻ���
	if (m_mapIdToSocket.count(rs->userId) > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
	}
}
