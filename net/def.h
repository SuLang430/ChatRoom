#pragma once

#define _DEF_TCP_PORT  (5634)
#define _DEF_TCP_SERVER_IP ("127.0.0.1")
#define _DEF_NAME_MAX			(100)
#define _DEF_CONTENT_LENGTH		(4096)
#define _DEF_PROTOCOL_COUNT		(10)

//Э��ͷ
#define _DEF_PROTOCOL_BASE		(1000)

//ע������
#define _DEF_REGISTER_RQ		(_DEF_PROTOCOL_BASE + 1)
//ע��ظ�
#define _DEF_REGISTER_RS		(_DEF_PROTOCOL_BASE + 2)
//��¼����
#define _DEF_LOGIN_RQ			(_DEF_PROTOCOL_BASE + 3)
//��½�ظ�
#define _DEF_LOGIN_RS			(_DEF_PROTOCOL_BASE + 4)
//������Ϣ
#define _DEF_FRIEND_INFO		(_DEF_PROTOCOL_BASE + 5)
//��������
#define _DEF_CHAT_RQ			(_DEF_PROTOCOL_BASE + 6)
//����ظ�
#define _DEF_CHAT_RS			(_DEF_PROTOCOL_BASE + 7)
//��Ӻ�������
#define _DEF_ADD_FRIEND_RQ		(_DEF_PROTOCOL_BASE + 8)
//��Ӻ��ѻظ�
#define _DEF_ADD_FRIEND_RS		(_DEF_PROTOCOL_BASE + 9)
//��������
#define _DEF_OFFLINE_RQ			(_DEF_PROTOCOL_BASE + 10)
//Ⱥ������
//#define _DEF_GROUP_CHAT_RQ		(_DEF_PROTOCOL_BASE + 11)
////Ⱥ�Ļظ�
//#define _DEF_GROUP_CHAT_RS		(_DEF_PROTOCOL_BASE + 12)


//ע����
#define _register_success		(0)
#define _register_tel_exists	(1)
#define _register_name_exists	(2)

//��¼���
#define _login_success			(0)
#define _login_tel_not_exists	(1)
#define _login_password_error	(2)

//���ͽ��
#define _send_success			(0)
#define _send_fail				(1)

//��Ӻ��ѽ��
#define _add_friend_success		(0)
#define _add_friend_not_exists	(1)
#define _add_friend_refuse		(2)
#define _add_friend_offline		(3)

// �û�״̬
#define _status_online			(0)
#define _status_offline			(1)

typedef int PackType;
//ע�������ֻ��š����롢�ǳ�
typedef struct _STRU_REGISTER_RQ
{
	_STRU_REGISTER_RQ():type(_DEF_REGISTER_RQ)
	{
		memset(tel, 0, _DEF_NAME_MAX);
		memset(password, 0, _DEF_NAME_MAX);
		memset(name, 0, _DEF_NAME_MAX);
	}
	PackType type;
	char tel[_DEF_NAME_MAX];
	char password[_DEF_NAME_MAX];
	char name[_DEF_NAME_MAX];

}_STRU_REGISTER_RQ;

//ע��ظ���ע�������ɹ����ǳƱ�ע�ᣬ�ֻ��ű�ע�ᣩ
typedef struct _STRU_REGISTER_RS
{
	_STRU_REGISTER_RS() :type(_DEF_REGISTER_RS) ,result(_login_password_error)
	{}
	PackType type;
	int result;
}_STRU_REGISTER_RS;

//��¼�����ֻ��š�����
typedef struct _STRU_LOGIN_RQ
{
	_STRU_LOGIN_RQ():type(_DEF_LOGIN_RQ)
	{
		memset(tel, 0, _DEF_NAME_MAX);
		memset(password, 0, _DEF_NAME_MAX);
	}
	PackType type;
	char tel[_DEF_NAME_MAX];
	char password[_DEF_NAME_MAX];
}_STRU_LOGIN_RQ;

//��¼�ظ�����½������ɹ����ֻ��Ų����ڣ��������

typedef struct _STRU_LOGIN_RS
{
	_STRU_LOGIN_RS() :type(_DEF_LOGIN_RS), result(_login_password_error)
	{}
	PackType type;
	int userId;
	int result;
}_STRU_LOGIN_RS;
// ������Ϣ������id�����ѵ��ǳơ�ͷ��id��ǩ����״̬
typedef struct _STRU_FRIEND_INFO
{
	_STRU_FRIEND_INFO() :type(_DEF_FRIEND_INFO),friendId(0), iconId(0), status(_status_offline)
	{
		memset(friendName, 0, _DEF_NAME_MAX);
		memset(feeling, 0, _DEF_NAME_MAX);
	}
	PackType type;
	int friendId;
	int iconId;
	int status;
	char friendName[_DEF_NAME_MAX];
	char feeling[_DEF_NAME_MAX];
}_STRU_FRIEND_INFO;


// ���������������ݡ��ҵ�id�����ѵ�id
typedef struct _STRU_CHAT_RQ
{
	_STRU_CHAT_RQ() :type(_DEF_CHAT_RQ),userId(0), friendId(0)
	{
		memset(content, 0, _DEF_CONTENT_LENGTH);
	}
	PackType type;
	char content[_DEF_CONTENT_LENGTH];
	int userId;
	int friendId;
};

// ����ظ������ͽ�����ɹ���ʧ�ܣ�
typedef struct _STRU_CHAT_RS
{
	_STRU_CHAT_RS() :type(_DEF_CHAT_RS), result(_send_fail), friendId(0)
	{}
	PackType type;
	int friendId;
	int result;
}_STRU_CHAT_RS;

// ��Ӻ������󣺺����ǳơ��ҵ�id���ҵ��ǳ�
typedef struct _STRU_ADD_FRIEND_RQ
{
	_STRU_ADD_FRIEND_RQ():type(_DEF_ADD_FRIEND_RQ),userId(0)
	{
		memset(friendName, 0, _DEF_NAME_MAX);
		memset(userName, 0, _DEF_NAME_MAX);
	}
	PackType type;
	char friendName[_DEF_NAME_MAX];
	char userName[_DEF_NAME_MAX];
	int userId;
}_STRU_ADD_FRIEND_RQ;




//��Ӻ��ѻظ�����ӽ�����ɹ������Ѳ����ڣ����Ѿܾ������Ѳ����ߣ�
typedef struct _STRU_ADD_FRIEND_RS
{
	_STRU_ADD_FRIEND_RS():type(_DEF_ADD_FRIEND_RS),result(_add_friend_offline),userId(0),friendId(0)
	{
		memset(friendName, 0, _DEF_NAME_MAX);
	}
	PackType type;
	int userId;
	int friendId;
	char friendName[_DEF_NAME_MAX]; 
	int result;
}_STRU_ADD_FRIEND_RS;

//���������ҵ�id
typedef struct _STRU_OFFLINE_RQ
{
	_STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ),userId(0)
	{
	}
	PackType type;
	int userId;
}_STRU_OFFLINE_RQ;
//
//// Ⱥ�������������ݡ��ҵ�id
//typedef struct _STRU_GROUP_CHAT_RQ
//{
//	_STRU_GROUP_CHAT_RQ() : type(_DEF_GROUP_CHAT_RQ), userId(0)
//	{
//		memset(content, 0, _DEF_CONTENT_LENGTH);
//	}
//	PackType type;
//	char content[_DEF_CONTENT_LENGTH];
//	int userId;
//} _STRU_GROUP_CHAT_RQ;
//
//// Ⱥ�Ļظ������ͽ�����ɹ���ʧ�ܣ�
//typedef struct _STRU_GROUP_CHAT_RS
//{
//	_STRU_GROUP_CHAT_RS() : type(_DEF_GROUP_CHAT_RS), result(_send_fail)
//	{}
//	PackType type;
//	int result;
//} _STRU_GROUP_CHAT_RS;