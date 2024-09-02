#pragma once

#define _DEF_TCP_PORT  (5634)
#define _DEF_TCP_SERVER_IP ("127.0.0.1")
#define _DEF_NAME_MAX			(100)
#define _DEF_CONTENT_LENGTH		(4096)
#define _DEF_PROTOCOL_COUNT		(10)

//协议头
#define _DEF_PROTOCOL_BASE		(1000)

//注册请求
#define _DEF_REGISTER_RQ		(_DEF_PROTOCOL_BASE + 1)
//注册回复
#define _DEF_REGISTER_RS		(_DEF_PROTOCOL_BASE + 2)
//登录请求
#define _DEF_LOGIN_RQ			(_DEF_PROTOCOL_BASE + 3)
//登陆回复
#define _DEF_LOGIN_RS			(_DEF_PROTOCOL_BASE + 4)
//好友信息
#define _DEF_FRIEND_INFO		(_DEF_PROTOCOL_BASE + 5)
//聊天请求
#define _DEF_CHAT_RQ			(_DEF_PROTOCOL_BASE + 6)
//聊天回复
#define _DEF_CHAT_RS			(_DEF_PROTOCOL_BASE + 7)
//添加好友请求
#define _DEF_ADD_FRIEND_RQ		(_DEF_PROTOCOL_BASE + 8)
//添加好友回复
#define _DEF_ADD_FRIEND_RS		(_DEF_PROTOCOL_BASE + 9)
//下线请求
#define _DEF_OFFLINE_RQ			(_DEF_PROTOCOL_BASE + 10)
//群聊请求
//#define _DEF_GROUP_CHAT_RQ		(_DEF_PROTOCOL_BASE + 11)
////群聊回复
//#define _DEF_GROUP_CHAT_RS		(_DEF_PROTOCOL_BASE + 12)


//注册结果
#define _register_success		(0)
#define _register_tel_exists	(1)
#define _register_name_exists	(2)

//登录结果
#define _login_success			(0)
#define _login_tel_not_exists	(1)
#define _login_password_error	(2)

//发送结果
#define _send_success			(0)
#define _send_fail				(1)

//添加好友结果
#define _add_friend_success		(0)
#define _add_friend_not_exists	(1)
#define _add_friend_refuse		(2)
#define _add_friend_offline		(3)

// 用户状态
#define _status_online			(0)
#define _status_offline			(1)

typedef int PackType;
//注册请求：手机号、密码、昵称
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

//注册回复：注册结果（成功，昵称被注册，手机号被注册）
typedef struct _STRU_REGISTER_RS
{
	_STRU_REGISTER_RS() :type(_DEF_REGISTER_RS) ,result(_login_password_error)
	{}
	PackType type;
	int result;
}_STRU_REGISTER_RS;

//登录请求：手机号、密码
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

//登录回复：登陆结果（成功，手机号不存在，密码错误）

typedef struct _STRU_LOGIN_RS
{
	_STRU_LOGIN_RS() :type(_DEF_LOGIN_RS), result(_login_password_error)
	{}
	PackType type;
	int userId;
	int result;
}_STRU_LOGIN_RS;
// 好友信息：好友id、好友的昵称、头像id、签名、状态
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


// 聊天请求：聊天内容、我的id、好友的id
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

// 聊天回复：发送结果（成功、失败）
typedef struct _STRU_CHAT_RS
{
	_STRU_CHAT_RS() :type(_DEF_CHAT_RS), result(_send_fail), friendId(0)
	{}
	PackType type;
	int friendId;
	int result;
}_STRU_CHAT_RS;

// 添加好友请求：好友昵称、我的id、我的昵称
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




//添加好友回复：添加结果（成功，好友不存在，好友拒绝，好友不在线）
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

//下线请求：我的id
typedef struct _STRU_OFFLINE_RQ
{
	_STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ),userId(0)
	{
	}
	PackType type;
	int userId;
}_STRU_OFFLINE_RQ;
//
//// 群聊请求：聊天内容、我的id
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
//// 群聊回复：发送结果（成功、失败）
//typedef struct _STRU_GROUP_CHAT_RS
//{
//	_STRU_GROUP_CHAT_RS() : type(_DEF_GROUP_CHAT_RS), result(_send_fail)
//	{}
//	PackType type;
//	int result;
//} _STRU_GROUP_CHAT_RS;