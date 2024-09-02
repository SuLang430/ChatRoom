#include "ckernel.h"
#include <QMessageBox>
#include <QDebug>
#include <QTextCodec>
#include <QInputDialog>
CKernel::CKernel(QObject *parent) : QObject(parent),m_id(0)
{
    qDebug()<<__func__;
    setProtocolFun();
    //给登录窗口new个对象
    m_pLoginDlg = new LoginDialog;
    m_pLoginDlg -> show();

    //绑定发送注册信息的信号和槽函数
    connect(m_pLoginDlg,&LoginDialog::sig_registerMessage,
            this,&CKernel::slot_registerMessage);
    //绑定发送登录信息的信号和槽函数
    connect(m_pLoginDlg,&LoginDialog::sig_loginMessage,
            this,&CKernel::slot_loginMessage);
    //绑定关闭程序的信号和槽函数
    connect(m_pLoginDlg,&LoginDialog::sig_closeProcess,
           this,&CKernel::slot_closeProcess);
    //new一个好友列表的窗口对象
    m_pFriendList = new FriendList;
   // m_pFriendList->show();

    //绑定退出程序的信号和槽函数
    connect(m_pFriendList, &FriendList::sig_exitProcess,
            this,&CKernel::slot_exitProcess);

    //绑定添加好友的信号和槽函数
    connect(m_pFriendList, &FriendList::sig_addFriend,
            this, &CKernel::slot_addFriend);

    //new一个中介者类的对象
    m_pMediator = new TcpClientMediator;

    //打开客户端网络
    if(!m_pMediator->openNet())
    {
        QMessageBox::about(m_pLoginDlg,"提示","打开网络失败");
        exit(1);
    }
    //绑定把net接收到的数据传给处理类的信号和槽函数
    connect(m_pMediator,SIGNAL(sig_transmitData(char*, int, long)),
            this,SLOT(slot_transmitData(char*, int, long)));

}

CKernel::~CKernel()
{
    if(m_pFriendList)
    {
        m_pFriendList->hide();
        delete m_pFriendList;
        m_pFriendList = nullptr;
    }
    if(m_pMediator)
    {
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }
    if(m_pLoginDlg){
        m_pLoginDlg->hide();
        delete m_pLoginDlg;
        m_pLoginDlg = nullptr;
    }
    for(auto ite = m_mapIdToChatdlg.begin();ite != m_mapIdToChatdlg.end();){
        ChatDialog* chat = ite.value();
        if(chat){
            chat->hide();
            delete chat;
            chat = nullptr;
        }
        //把无效的节点从map中移除
        ite = m_mapIdToChatdlg.erase(ite);
    }
}

//utf-8转到gb2312
void CKernel::utf8ToGb2312(QString src, char *dst, int len)
{
    QTextCodec* cd = QTextCodec::codecForName("gb2312");
    QByteArray ba = cd->fromUnicode(src);
    strcpy_s(dst, len, ba.data());
}

//gb2312转到utf-8
QString CKernel::gb2312ToUtf8(char *src)
{
    QTextCodec* cd = QTextCodec::codecForName("gb2312");
   return cd->toUnicode(src);
}

//处理注册回复
void CKernel::dealRegisterRs(char* data, int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_REGISTER_RS* rs = (_STRU_REGISTER_RS*)data;

    //2.显示注册结果
    switch (rs->result) {
    case _register_success:
        QMessageBox::about(m_pLoginDlg,"提示","注册成功");
        break;
    case _register_tel_exists:
         QMessageBox::about(m_pLoginDlg,"提示","注册失败：电话号码已被注册");
        break;
    case _register_name_exists:
        QMessageBox::about(m_pLoginDlg,"提示","注册失败：昵称已被使用");
        break;
default:
        break;
    }
}

//处理登录回复
void CKernel::dealLoginRs(char* data,int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_LOGIN_RS* rs = (_STRU_LOGIN_RS*)data;

    //2.根据结果显示
    switch (rs->result) {
    case _login_success:
    {

        //页面跳转:隐藏登录界面，显示好友列表界面
        m_pLoginDlg->hide();
        m_pFriendList->show();
        //保存自己的id
        m_id = rs->userId;
    }
        break;
    case _login_tel_not_exists:
        QMessageBox::about(m_pLoginDlg,"提示","登录失败，电话号码未注册");
        break;
    case _login_password_error:
        QMessageBox::about(m_pLoginDlg,"提示","登录失败，密码错误");
        break;
    default:
        break;
    }

}

    //处理自己及好友的信息
void CKernel::dealInfo(char *data, int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_FRIEND_INFO* info = (_STRU_FRIEND_INFO*)data;
    QString name = gb2312ToUtf8(info->friendName);
    QString feeling = gb2312ToUtf8(info->feeling);

    //2.判断是不是自己的信息
    if(info->friendId == m_id)
    {
        //保存自己的昵称
        m_name = name;
        //3.把自己的信息设置到界面上
        m_pFriendList->setInfo(info->friendId,name,feeling,info->iconId);
        return;
    }

    //4.好友的信息，判断是否都已经在列表上了
    if(m_mapIdToUseritem.count(info->friendId)>0)
    {
        //5.好友在列表上，从列表上取出好友只需要更新好友的信息
        useritem* item = m_mapIdToUseritem[info->friendId];
        item->setFriendInfo(info->friendId,name,feeling,info->iconId,info->status);
    }
    else
    {
        //6.好友不在列表上，先new一个好友
        useritem* item = new useritem;
        //7.设置好友的信息
        item->setFriendInfo(info->friendId,name,feeling,info->iconId,info->status);
        //8.把好友添加到列表上
        m_pFriendList->addFriend(item);

        //绑定显示聊天窗口的信号和槽函数
        connect(item, &useritem::sig_showChatDialog,
                this, &CKernel::slot_showChatDialog);
        //9.保存useritem
        m_mapIdToUseritem[info->friendId] = item;

        //10.new一个跟这个好友的聊天窗口
        ChatDialog* chat = new ChatDialog;
        //设置聊天信息
        chat->setDialogInfo(info->friendId,name);
        //11.把聊天窗口保存在map中
        m_mapIdToChatdlg[info->friendId] = chat;

        //12.绑定发送聊天内容的信号和槽函数
        connect(chat, &ChatDialog::sig_ChatInfo,
                this, &CKernel::slot_ChatInfo);
    }
}

//处理下线请求
void CKernel::dealOfflineRq(char *data, int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

    //2.找到好友的useritem
    if(m_mapIdToUseritem.count(rq->userId)>0){
        //取出好友的useritem
        useritem* item = m_mapIdToUseritem[rq->userId];
        //修改好友的状态和头像
        item->setFriendOffline();
    }
}

//处理聊天请求（B用户）
void CKernel::dealChatRq(char *data, int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

    //2.找到跟用户A的聊天窗口，把聊天内容显示到窗口上
    if(m_mapIdToChatdlg.count(rq->userId)>0){
        ChatDialog* chat = m_mapIdToChatdlg[rq->userId];
        chat->setChatContent(rq->content);
        chat ->showNormal();
    }
}

//处理聊天回复（A用户）
void CKernel::dealChatRs(char *data, int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_CHAT_RS* rs = (_STRU_CHAT_RS*)data;

    //2.找到跟用户B的聊天窗口，显示用户B不在线
    if(m_mapIdToChatdlg.count(rs->friendId)>0){
        ChatDialog* chat = m_mapIdToChatdlg[rs->friendId];
        chat->setFriendOffline();
        chat->showNormal();
    }
}

//处理添加好友请求（B用户）
void CKernel::dealAddFriendRq(char *data, int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

    //2.弹出选择窗口
    _STRU_ADD_FRIEND_RS rs;
    QString str = QString("【%1】想添加你为好友，是否同意？").arg(rq->userName);
    if(QMessageBox::Yes == QMessageBox::question(m_pFriendList,"提示",str)){
        rs.result = _add_friend_success;
    }else{
        rs.result = _add_friend_refuse;
    }
    rs.userId = rq->userId;
    rs.friendId = m_id;
    strcpy_s(rs.friendName,sizeof(rs.friendName),rq->friendName);

    //3.把添加结果发给服务端
    m_pMediator->sendData((char*)&rs,sizeof(rs),12);
}

//处理添加好友回复（A用户）
void CKernel::dealAddFriendRs(char *data, int len, long lFrom)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;
    QString friendName = gb2312ToUtf8(rs->friendName);

    //2.根据结果提示用户
    switch(rs->result){
    case _add_friend_success:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】为好友成功").arg(friendName));
        break;
    case _add_friend_not_exists:
        QMessageBox::about(m_pFriendList,"提示",QString("添加好友失败，【%1】不存在").arg(friendName));
        break;
    case _add_friend_refuse:
        QMessageBox::about(m_pFriendList,"提示",QString("添加好友失败，【%1】拒绝添加你为好友").arg(friendName));
        break;
    case _add_friend_offline:
        QMessageBox::about(m_pFriendList,"提示",QString("添加好友失败，【%1】不在线").arg(friendName));
        break;
    default:
        break;
    }
}


//初始化协议头数组
void CKernel::setProtocolFun()
{
    qDebug()<<__func__;
    //初始化数组
        memset(m_pProtocolToFun, 0, sizeof(m_pProtocolToFun));

        //绑定协议头和函数
        m_pProtocolToFun[_DEF_REGISTER_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRs;
        m_pProtocolToFun[_DEF_LOGIN_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRs;
        m_pProtocolToFun[_DEF_FRIEND_INFO - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealInfo;
        m_pProtocolToFun[_DEF_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
        m_pProtocolToFun[_DEF_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
        m_pProtocolToFun[_DEF_CHAT_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRs;
        m_pProtocolToFun[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
        m_pProtocolToFun[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
}

void CKernel::slot_transmitData(char *data, int len, long lFrom)
{
    qDebug()<<__func__;
    //根据协议头，判断收到的是哪个结构体，是哪个操作，不同的操作走不同的处理流程
    PackType type = *(PackType*)data;
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
                qDebug() << "type2 error:" << type;
            }
        }
        else
        {
            qDebug() << "type1 error:" << type;
        }

        delete[] data;
}

//处理注册信息
void CKernel::slot_registerMessage(QString name, QString tel, QString password)
{
    qDebug()<<__func__;
    //1.装包
    _STRU_REGISTER_RQ rq;
   // strcpy_s(rq.name,name.toStdString().c_str());
    utf8ToGb2312(name,rq.name,sizeof(rq.name));
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,password.toStdString().c_str());

    //2.发给客户端
    m_pMediator -> sendData((char*)&rq,sizeof(rq),678);
}

//处理登录信息
void CKernel::slot_loginMessage(QString tel, QString password)
{
    qDebug()<<__func__;
    //1.装包
    _STRU_LOGIN_RQ rq;
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,password.toStdString().c_str());

    //2.发给客户端
    m_pMediator -> sendData((char*)&rq,sizeof(rq),678);
}

void CKernel::slot_closeProcess()
{
    qDebug()<<__func__;
    //1.回收资源
    if(m_pMediator){
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator =nullptr;
    }
    if(m_pFriendList){
        m_pFriendList->hide();
        delete m_pFriendList;
        m_pFriendList =nullptr;
    }
    if(m_pLoginDlg){
        m_pLoginDlg->hide();
        delete m_pLoginDlg;
        m_pLoginDlg = nullptr;
    }
    for(auto ite = m_mapIdToChatdlg.begin();ite != m_mapIdToChatdlg.end();){
        ChatDialog* chat = ite.value();
        if(chat){
            chat->hide();
            delete chat;
            chat = nullptr;
        }
        //把无效的节点从map中移除
        ite = m_mapIdToChatdlg.erase(ite);
    }
    //2.关闭程序
    exit(0);
}

//通知kernel关闭程序
void CKernel::slot_exitProcess()
{
    qDebug()<<__func__;

    //1.通知服务端下线
    _STRU_OFFLINE_RQ rq;
    rq.userId = m_id;
    m_pMediator->sendData((char*)&rq, sizeof(rq),89);
    //2.回收资源

    //3.退出程序
    slot_exitProcess();
}

//显示与该好友的聊天窗口
void CKernel::slot_showChatDialog(int id)
{
    qDebug()<<__func__;
    if(m_mapIdToChatdlg.count(id)>0){
        ChatDialog* chat = m_mapIdToChatdlg[id];
        chat->showNormal();
    }
}

//把聊天窗内容发给kernel
void CKernel::slot_ChatInfo(int id, QString content)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_CHAT_RQ rq;
    rq.userId = m_id;
    rq.friendId = id;
    strcpy_s(rq.content,sizeof(rq.content),content.toStdString().c_str());

    //2.发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),67);

}

    //通知kernel添加好友
void CKernel::slot_addFriend()
{
    qDebug()<<__func__;
    //1.弹出询问窗口
    QString name = QInputDialog::getText(m_pFriendList, "添加好友", "请输入好友的昵称：");
    QString nameTmp = name;
    qDebug()<<name;
    //2.判断用户输入的是否合法
    if(name.isEmpty()||nameTmp.remove(" ").isEmpty()){
        QMessageBox::about(m_pFriendList,"提示","好友昵称不能为空或全空格");
        return ;
    }

    //3.判断昵称是否为自己
    if( m_name == name){
        QMessageBox::about(m_pFriendList,"提示","不能添加自己为好友");
        return;
    }

    //4.判断昵称是否已经为好友
    for(useritem* item : m_mapIdToUseritem){
        //取出好友的昵称
        QString friendName = item->name();
        if(friendName == name){
            QMessageBox::about(m_pFriendList,"提示","已经是好友啦，不需要重复添加");
            return;
        }
    }
    //5.给服务端发送添加好友的请求
    _STRU_ADD_FRIEND_RQ rq;
    rq.userId = m_id;
    strcpy_s(rq.userName,sizeof(rq.userName),m_name.toStdString().c_str());
    //strcpy_s(rq.friendName,sizeof(rq.friendName),name.toStdString().c_str());
    utf8ToGb2312(name,rq.friendName,sizeof(rq.friendName));
    m_pMediator->sendData((char*)&rq,sizeof(rq),32);

}
