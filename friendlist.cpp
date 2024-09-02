#include "friendlist.h"
#include "ui_friendlist.h"
#include <QMessageBox>
#include <QDebug>
FriendList::FriendList(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FriendList)
{
    ui->setupUi(this);
    //new一个垂直布局层的对象
    m_pLayout = new QVBoxLayout;
    m_pLayout->setContentsMargins(0,0,0,0);
    m_pLayout->setSpacing(3);
    //把层添加到大控件上
    ui->wdg_list->setLayout(m_pLayout);

    //new一个菜单的对象
    m_pMenu = new QMenu;
    //添加菜单项
    m_pMenu->addAction("添加好友");
    m_pMenu->addAction("系统设置");
    //绑定点击菜单项的信号和槽函数
    connect(m_pMenu, &QMenu::triggered,
            this,&FriendList::slot_triggered);
}

FriendList::~FriendList()
{
    delete ui;
}

    //重写关闭窗口事件
void FriendList::closeEvent(QCloseEvent *event)
{
    //忽略用户操作
    event ->ignore();
    //弹出询问窗口，是否确认关闭程序
    if(QMessageBox::Yes == QMessageBox::question(this,"关闭提示","是否确定退出程序？"))
    {
        //通知kernel关闭程序
        Q_EMIT sig_exitProcess();
    }
}

//把好友添加到列表上
void FriendList::addFriend(useritem *item)
{
    m_pLayout->addWidget(item);
}

//保存并设置自己的信息
void FriendList::setInfo(int id, QString name, QString feeling, int iconId)
{
    m_id = id;
    m_name = name;
    m_feeling = feeling;
    m_iconId = iconId;

    ui->lb_name->setText(m_name);
    ui->le_feeling->setText(m_feeling);
    ui->pb_icon->setIcon(QIcon(QString(":/tx/%1.png").arg(m_iconId)));
}

//在点击的位置向上显示一个菜单列表
void FriendList::on_pb_menu_clicked()
{
    //获取鼠标点击的坐标
    QPoint point = QCursor::pos();

    //获取菜单项列表的绝对大小
    QSize size = m_pMenu->sizeHint();

    //显示菜单项
    m_pMenu->exec(QPoint(point.x(),point.y() - size.height()));
}

//点击菜单项执行的槽函数
void FriendList::slot_triggered(QAction *action)
{
    if("添加好友" == action->text()){
        //通知kernel添加好友
        Q_EMIT sig_addFriend();
    }else{
        qDebug()<<action->text();
    }
}

