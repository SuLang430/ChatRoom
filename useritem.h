#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>
#include <QBitmap>

namespace Ui {
class useritem;
}

class useritem : public QWidget
{
    Q_OBJECT
signals:
    //通知kernel显示与该好友的聊天窗口
    void sig_showChatDialog(int id);
public:
    explicit useritem(QWidget *parent = nullptr);
    ~useritem();

    //设置好友信息
    void setFriendInfo(int id, QString name, QString feeling, int iconId, int status);

    //设置好友下线
    void setFriendOffline();

    const QString &name() const;

private slots:
    void on_pb_icon_clicked();

private:
    Ui::useritem *ui;
    int m_id;
    QString m_name;
    QString m_feeling;
    int m_iconId;
    int m_status;


};

#endif // USERITEM_H
