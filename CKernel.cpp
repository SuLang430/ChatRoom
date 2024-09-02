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
//初始化协议头数组
void CKernel::setProtocolFun()
{
	//初始化数组
	memset(m_pProtocolToFun, 0, sizeof(m_pProtocolToFun));

	//绑定协议头和函数
	m_pProtocolToFun[_DEF_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_pProtocolToFun[_DEF_LOGIN_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_pProtocolToFun[_DEF_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_pProtocolToFun[_DEF_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_pProtocolToFun[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_pProtocolToFun[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
	//m_pProtocolToFun[_DEF_GROUP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealGroupChatRq;
}

//打开服务器
bool CKernel::startServer()
{
	//初始化协议头数组
	setProtocolFun();
	//初始化网络
	if (!m_pMediator->openNet())
	{
		cout << "openNet false" << endl;
		return false;
	}
	//连接数据库
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

//关闭服务器
void CKernel::closeServer()
{
	if (m_pMediator)
	{
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}

//3.处理所有接收到的数据
void CKernel::dealData(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	
	/*_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	cout << "name:" << rq->name << ", pas:" << rq->password << ", tel:" << rq->tel << endl;
	*/
	/*_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	cout << "pas:" << rq->password << ", tel:" << rq->tel << endl;*/

	//根据协议头，判断收到的是哪个结构体，是哪个操作，不同操作走不同处理流程
	PackType type = *(PackType*) data;
	//计算数组下标
	int index = type - _DEF_PROTOCOL_BASE - 1;
	//判断下标是否在有效范围内
	if (index>=0&&index<_DEF_PROTOCOL_COUNT)
	{

		//取出函数地址
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

//处理注册请求
void CKernel::dealRegisterRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	// 1、拆包
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;

	// 2、校验数据合法性（规则和客户端一致）


	// 3、根据昵称查询昵称
	list<string> lstStr;
	char szSql[1024] = "select name from t_user where name = ";
	sprintf(szSql, "select name from t_user where name = '%s';", rq->name);
	if (!m_sql.SelectMySql(szSql,1,lstStr))
	{
		//查询数据库失败两个原因：1.未连接数据库或库中无表；2.sql语句错误
		cout << "select sql false" << szSql << endl;
		return;
	}

	// 4、判断查询结果是否为空
	_STRU_REGISTER_RS rs;
	if (lstStr.size() != 0) {
		// 5、查询结果不为空，注册失败，_register_name_exists
		rs.result = _register_name_exists;
	}
	else {
		// 6、根据电话号码查询电话号码
		sprintf(szSql, "select name from t_user where tel = '%s';", rq->tel);
		if (!m_sql.SelectMySql(szSql, 1, lstStr))
		{
			//查询数据库失败两个原因：1.未连接数据库或库中无表；2.sql语句错误
			cout << "select sql false" << szSql << endl;
			return;
		}
		// 7、判断查询结果是否为空
		if (lstStr.size() != 0) {
			// 8、查询结果不为空，注册失败，_register_tel_exists
			rs.result = _register_tel_exists;
		}
		else {
			// 9、把用户的注册信息写入数据库
			sprintf(szSql, "insert into t_user(name,tel,password,feeling,iconid) values ('%s','%s','%s','nihao','4');", rq->name,rq->tel,rq->password);
			if (!m_sql.UpdateMySql(szSql))
			{
				cout << "insert mysql false" << szSql << endl;
				return;
			}
			// 10、注册成功，_register_success
			rs.result = _register_success;
		}
	}

	// 11、无论注册结果是否成功，都需要给客户端回复
	m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);
}

//处理登陆请求
void CKernel::dealLoginRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;

	//2.校验参数合法性(规则和客户端一致)

	//3.根据电话号码查询密码
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select password,id from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(szSql, 2, lstStr))
	{
		//查询数据库失败两个原因：1.未连接数据库或库中无表；2.sql语句错误
		cout << "select sql false" << szSql << endl;
		return;
	}

	//4.判断查询结果是否为空
	_STRU_LOGIN_RS rs;
	if (0 == lstStr.size())
	{
		//5.查询结果为空，则手机号未注册，登录失败
		rs.result = _login_tel_not_exists;
	}
	else
	{
		//从list中取出密码
		string pass = lstStr.front();
		lstStr.pop_front();	//取完的数据从list中删除
		
		//从list中取出id
		int id = stoi(lstStr.front());
		lstStr.pop_front();
		
		//6.比较查询出的密码和登录时的密码是否一致
	
		if (0 == strcmp(pass.c_str(),rq->password))
		{
			//7.密码相同，登录成功
			rs.result = _login_success;
			rs.userId = id; 

			//9.无论什么结果，都要通知客户端
			m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);

			//保存当前登录用户的id和socket
			m_mapIdToSocket[id] = lFrom;
			//获取自己及好友的信息
			getInfo(id);

		

		}
		else
		{
			//8.密码错误，登录失败
			rs.result = _login_password_error;
		}
	
	}

	//9.无论什么结果，都要通知客户端
	m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);
}

void CKernel::getInfo(int id)
{
	cout << __func__ << endl;
	//根据自己的id查询自己的信息
	_STRU_FRIEND_INFO userInfo;
	getUserInfo(id, &userInfo);
	//把自己的信息发给客户端
	if (m_mapIdToSocket.find(id)!=m_mapIdToSocket.end())
	{
		m_pMediator->sendData((char*)&userInfo, sizeof(userInfo),m_mapIdToSocket[id]);
	}
	
	//根据自己的id查询好友的id列表
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select idA from t_friend where idB = %d;", id);
	if (!m_sql.SelectMySql(szSql, 1, lstStr))
	{
		//查询数据库失败两个原因：1.未连接数据库或库中无表；2.sql语句错误
		cout << "select sql false" << szSql << endl;
		return;
	}

	//遍历查询结果
	int friendId = 0;
	_STRU_FRIEND_INFO friendInfo;
	while (lstStr.size()>0)
	{
		//取出好友id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		cout << "friendId:" << friendId << endl;
		//根据好友id查询好友信息
		getUserInfo(friendId, &friendInfo);

		//把好友信息发给客户端
		if (m_mapIdToSocket.count(id)>0)
		{
			m_pMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[id]);
		}

		//判断好友是否在线，在线就通知好友
		if (m_mapIdToSocket.count(friendId)>0)
		{
			m_pMediator->sendData((char*)&userInfo,sizeof(userInfo) , m_mapIdToSocket[friendId]);
		}
	}
}

//根据id获取用户信息
void CKernel::getUserInfo(int userId, _STRU_FRIEND_INFO* info)
{
	cout << __func__ << endl;
	info->friendId = userId;
	if (m_mapIdToSocket.count(userId)>0)
	{
		//在线
		info->status = _status_online;
	}
	else
	{
		//离线
		info->status = _status_offline;
	}

	//从数据库查询用户的昵称、签名、头像id
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select name, feeling, iconid from t_user where id = '%d';", userId);
	if (!m_sql.SelectMySql(szSql, 3, lstStr))
	{
		//查询数据库失败两个原因：1.未连接数据库或库中无表；2.sql语句错误
		cout << "select sql false" << szSql << endl;
		return;
	}

	if (3 == lstStr.size())
	{
		//开始往结构体存值
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

//处理下线请求
void CKernel::dealOfflineRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;

	//1.拆包
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

	//2.根据用户id查询好友id列表
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select idA from t_friend where idB = %d;", rq->userId);
	if (!m_sql.SelectMySql(szSql, 1, lstStr))
	{
		//查询数据库失败两个原因：1.未连接数据库或库中无表；2.sql语句错误
		cout << "select sql false" << szSql << endl;
		return;
	}
	
	//3.遍历=好友id列表
	int friendId = 0;
	while (lstStr.size() > 0)
	{
		//4.取出好友id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		//5.判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			//6.通知在线好友（转发下线请求给好友）
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}

	//7.回收socket、从map中移除
	if (m_mapIdToSocket.count(rq->userId) > 0)
	{
		SOCKET sock = m_mapIdToSocket[rq->userId];
		closesocket(sock);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//处理聊天请求
void CKernel::dealChatRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	
	//1.拆包
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

	//2.判断好友是否在线
	if (m_mapIdToSocket.count(rq->friendId) > 0)
	{
		//3.好友在线，把聊天请求转发给好友B用户
		m_pMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//4.好友不在线，告诉A用户发送失败
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		rs.result = _send_fail;
		m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);

	}
}

//处理添加好友请求
void CKernel::dealAddFriendRq(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2.根据好友的昵称查询好友的id
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf(szSql, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(szSql, 1, lstStr) ){
		//查询数据库失败两个原因：1.没连接数据库或者库里没有表；2.sql语句写错了
		cout<<"查询数据库失败"<<szSql<<endl;
			return;
	}

	//3.判断查询结果是否为空
	_STRU_ADD_FRIEND_RS rs;
	if (0 == lstStr.size()) {
		//4.如果查询结果为空，添加好友失败，_add_friend_not_exists
		rs.result = _add_friend_not_exists;
		strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
		//把结果发回给客户端
		m_pMediator->sendData((char*)&rs,sizeof(rs),lFrom);
	}
	else {
		//5.查询结果不为空，从查询结果中取出好友的id
		int friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//6.判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0) {
			//7.好友在线，把添加好友请求转发给好友
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//8.好友不在线，添加好友失败，_add_friend_offline
			rs.result = _add_friend_offline;
			strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
			//把结果发回给客户端
			m_pMediator->sendData((char*)&rs, sizeof(rs), lFrom);
		}
	}
}

//处理添加好友回复
void CKernel::dealAddFriendRs(char* data, int len, long lFrom)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;

	//2.判断添加结果是否成功
	if (_add_friend_success == rs->result)
	{
		//3.把两个人的好友关系写入数据库
		char sqlBuf[1024] = "";
		sprintf(sqlBuf, "insert into t_friend values (%d, %d);", rs->friendId, rs->userId );
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "插入数据库失败：" << sqlBuf << endl;
			return;
		}

		sprintf(sqlBuf, "insert into t_friend values (%d, %d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "插入数据库失败：" << sqlBuf << endl;
			return;
		}
		//4.更新客户端好友列表
		getInfo(rs->friendId);

	}
	//不管成功是否都要转发给A客户端
	if (m_mapIdToSocket.count(rs->userId) > 0) {
		m_pMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
	}
}
