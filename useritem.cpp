#include "useritem.h"
#include "ui_useritem.h"
#include "net/def.h"

useritem::useritem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::useritem)
{
    ui->setupUi(this);
}
useritem::~useritem()
{
    delete ui;
}

//设置好友信息
void useritem::setFriendInfo(int id, QString name, QString feeling, int iconId, int status)
{
    //保存好友信息
    m_id = id;
    m_name = name;
    m_feeling = feeling;
    m_iconId = iconId;
    m_status = status;

    //设置到界面上
    ui->lb_name->setText(m_name);
    ui->lb_feeling->setText(m_feeling);

    //拼接头像文件路径
    QString path = QString(":/tx/%1.png").arg(m_iconId);
    if(_status_online == m_status)
    {
        //在线，头像亮显
        ui->pb_icon->setIcon(QIcon(path));
    }
    else
    {
        //不在线，头像暗显
        QBitmap bp;
        bp.load(path);
        ui->pb_icon->setIcon(bp);
    }
}

//设置好友下线
void useritem::setFriendOffline()
{
    m_status = _status_offline;
    QString path = QString(":/tx/%1.png").arg(m_iconId);
    QBitmap bp;
    bp.load(path);
    ui->pb_icon->setIcon(bp);
    //重绘
    repaint();
}

void useritem::on_pb_icon_clicked()
{
    //点击好友头像，通知kernel显示与该好友的聊天窗口
    Q_EMIT sig_showChatDialog(m_id);
}

const QString &useritem::name() const
{
    return m_name;
}


