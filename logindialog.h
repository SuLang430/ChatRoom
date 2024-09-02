#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
signals:
    //把注册信息发给kernel
    void sig_registerMessage(QString name, QString tel, QString password);

    //把登录信息发给kernel
    void sig_loginMessage(QString tel, QString password);

    //通知kernel关闭程序
    void sig_closeProcess();

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    //重写关闭窗口事件
    void closeEvent(QCloseEvent * event);


private slots:
    void on_pb_clear_register_clicked();

    void on_pb_commit_register_clicked();

    void on_pb_clear_clicked();

    void on_pb_commit_clicked();



private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
