#ifndef CKERNEL_H
#define CKERNEL_H

#include "logindialog.h"
#include "friendlist.h"
#include "mediator/TcpClientMediator.h"
#include <QObject>
#include "net/def.h"
#include "useritem.h"
#include <QMap>
#include "chatdialog.h"

//定义类的成员函数指针
class CKernel;
typedef void (CKernel::*PFUN)(char* data, int len, long lFrom);

class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();

    //utf-8转到gb2312
    void utf8ToGb2312(QString src, char* dst, int len);
    //gb2312转到utf-8
    QString gb2312ToUtf8(char* src);
    //初始化协议头数组
    void setProtocolFun();
    //处理注册请求
    void dealRegisterRq(char* data, int len, long lFrom);

    //处理登陆请求
    void dealLoginRq(char* data, int len, long lFrom);
    //处理注册回复
    void dealRegisterRs(char* data, int len, long lFrom);

    //处理登录回复
    void dealLoginRs(char* data,int len,long lFrom);

    //处理自己及好友的信息
    void dealInfo(char* data, int len, long lFrom);
    //处理下线请求
    void dealOfflineRq(char* data, int len, long lFrom);
    //处理聊天请求（B用户）
    void dealChatRq(char* data, int len, long lFrom);
    //处理聊天回复（A用户）
    void dealChatRs(char* data, int len, long lFrom);
    //处理添加好友请求（B用户）
    void dealAddFriendRq(char* data, int len, long lFrom);
    //处理添加好友回复（A用户）
    void dealAddFriendRs(char* data, int len, long lFrom);

public slots:
    //把net接收到的数据传给处理类
    void slot_transmitData(char* data, int len, long lFrom);



    //处理注册信息
    void slot_registerMessage(QString name,QString  tel,QString  password);

    //处理登录信息
    void slot_loginMessage(QString tel,QString password);

    //通知kernel关闭程序
    void slot_closeProcess();

    //通知kernel关闭程序
    void slot_exitProcess();
    //显示与该好友的聊天窗口
    void slot_showChatDialog(int id);

    //把聊天内容发给kernel
    void slot_ChatInfo(int id, QString content);

    //通知kernel添加好友
    void slot_addFriend();
signals:

private:
    int m_id;
    QString m_name;
    IMediator* m_pMediator;
    FriendList* m_pFriendList;
    LoginDialog* m_pLoginDlg;
    //定义函数指针数组
    PFUN m_pProtocolToFun[_DEF_PROTOCOL_COUNT];
    //保存useritem
    QMap<int,useritem*> m_mapIdToUseritem;
    //保存好友的聊天窗口
    QMap<int, ChatDialog*> m_mapIdToChatdlg;
};

#endif // CKERNEL_H
