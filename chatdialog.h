#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT
signals:
    //把聊天内容发给kernel
    void sig_ChatInfo(int id, QString content);
public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

    //设置聊天窗口信息
    void setDialogInfo(int id, QString name);

    //设置好友发来的聊天内容
    void setChatContent(QString content);

    //显示好友不在线
    void setFriendOffline();

private slots:
    void on_pb_send_clicked();

private:
    Ui::ChatDialog *ui;
    int m_id;
    QString m_name;
};

#endif // CHATDIALOG_H
