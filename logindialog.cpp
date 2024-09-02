#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

    //重写关闭窗口事件
void LoginDialog::closeEvent(QCloseEvent *event)
{
    //通知kernel关闭程序
    Q_EMIT sig_closeProcess();
}


void LoginDialog::on_pb_clear_register_clicked()
{
    ui->le_name->setText("");
    ui->le_password_register->setText("");
    ui->le_tel_register->setText("");
}

void LoginDialog::on_pb_commit_register_clicked()
{
    //1.获取用户输入的数据
    QString name = ui->le_name->text();
    QString tel = ui->le_tel_register->text();
    QString password = ui->le_password_register->text();
    qDebug()<<name<<","<<tel<<","<<password;
    QString nameTmp = name;
    QString telTmp = tel;
    QString passwordTmp = password;

    //2.校验数据合法性
    //2.1 判断字符串是否为空或全为空格
    if(name.isEmpty()||tel.isEmpty()||password.isEmpty()||nameTmp.remove(" ").isEmpty()||telTmp.remove(" ").isEmpty()||passwordTmp.remove(" ").isEmpty())
    {
        QMessageBox::about(this,"提示","昵称、电话号码、密码不能为空或是全空格");
        return;
    }
    //2.2 校验长度（电话号码11位，昵称不超过20位，密码不超过15位）
    if(name.length()>20||tel.length()!=11||password.length()>15)
    {
        QMessageBox::about(this,"提示","昵称、电话号码、密码长度不符合要求");
    }

    //2.3 校验内容（电话号码全数字，昵称不能有特殊字符，密码是字母、数字、下划线的组合）

    //3.把数据发给kernel
    Q_EMIT sig_registerMessage(name, tel, password);
}
void LoginDialog::on_pb_clear_clicked()
{
    ui->le_password->setText("");
    ui->le_tel->setText("");
}

void LoginDialog::on_pb_commit_clicked()
{
    //1.获取用户输入的数据
    QString tel = ui->le_tel->text();
    QString password = ui->le_password->text();
    qDebug()<<tel<<","<<password;
    QString telTmp = tel;
    QString passwordTmp = password;
    //2.校验数据合法性
    if(tel.isEmpty()||password.isEmpty()||telTmp.remove(" ").isEmpty()||passwordTmp.remove(" ").isEmpty())
    {
        QMessageBox::about(this,"提示","电话号码、密码不能为空或是全空格");
        return;
    }
    //2.2 校验长度（电话号码11位，昵称不超过20位，密码不超过15位）
    if(tel.length()!=11||password.length()>15)
    {
        QMessageBox::about(this,"提示","电话号码、密码长度不符合要求");
    }

    //2.3 校验内容（电话号码全数字，昵称不能有特殊字符，密码是字母、数字、下划线的组合）
    //3.把数据发给kernel
    Q_EMIT sig_loginMessage(tel,password);
}






