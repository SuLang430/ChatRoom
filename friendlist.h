#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include "useritem.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class FriendList; }
QT_END_NAMESPACE

class FriendList : public QDialog
{
    Q_OBJECT
signals:
    //通知kernel关闭程序
    void sig_exitProcess();
    //通知kernel添加好友
    void sig_addFriend();
public:
    FriendList(QWidget *parent = nullptr);
    ~FriendList();
    //重写关闭窗口事件
    void closeEvent(QCloseEvent * event);
    //添加好友到列表上
    void addFriend(useritem* item);
    //保存并设置自己的信息
    void setInfo(int id, QString name, QString feeling, int iconId);

private slots:
    void on_pb_menu_clicked();
    //点击菜单项执行的槽函数
    void slot_triggered(QAction *action);

private:
    int m_id;
    QString m_name;
    QString m_feeling;
    int m_iconId;
    Ui::FriendList *ui;
    //垂直布局的层
    QVBoxLayout* m_pLayout;
    QMenu* m_pMenu;
};
#endif // FRIENDLIST_H
