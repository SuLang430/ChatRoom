#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QDebug>
#include <QTime>
ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

//设置聊天窗口信息
void ChatDialog::setDialogInfo(int id, QString name)
{
    m_id = id;
    m_name = name;

    setWindowTitle(QString("与【%1】的聊天窗口").arg(m_name));
}

//设置好友发来的聊天内容
void ChatDialog::setChatContent(QString content)
{
    ui->tb_chat->append(QString("【%1】 %2").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

//显示好友不在线
void ChatDialog::setFriendOffline()
{
    ui->tb_chat->append(QString("【%1】 %2 不在线").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));

}

void ChatDialog::on_pb_send_clicked()
{
    //1.获取用户输入的内容（纯文本）
    QString content = ui->te_chat->toPlainText();

    //2.校验用户输入的数据是否是空或全空格
    if(content.isEmpty()||content.remove(" ").isEmpty()){
        ui->te_chat->setText("");
        return;
    }

    //3.获取用户输入的内容（带格式的）
    content = ui->te_chat->toHtml();
    qDebug()<<content;

    //4.清空下面编辑窗口，设置到浏览窗口上
    ui->te_chat->setText("");
    ui->tb_chat->append(QString("【我】 %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);

    //5.把聊天内容发给kernel
    Q_EMIT sig_ChatInfo(m_id, content);
}

