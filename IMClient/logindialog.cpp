#include "logindialog.h"
#include "ui_logindialog.h"
#include<QMessageBox>
#include<QDebug>
#include <QRegularExpression>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("矩联即时通讯");
    this->setWindowIcon(QIcon("D:/KnowledgeStructure/IMClient/IMClient/images/message.ico"));
    this->setStyleSheet(R"(
            QDialog {
                background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                                                  stop:0 #4FACFE, stop:1 #00F2FE);
            }
            QTabWidget::pane {
                border: none;
                background-color: rgba(255,255,255,0.9);
                border-radius: 8px;
            }
            QTabWidget::tab-bar {
                alignment: center;
            }
            QTabBar::tab {
                background-color: #e6f2ff;
                color: #409EFF;
                padding: 8px 20px;
                border-top-left-radius: 6px;
                border-top-right-radius: 6px;
                margin-right: 5px;
            }
            QTabBar::tab:selected {
                background-color: #409EFF;
                color: white;
                font-weight: bold;
            }
            QLineEdit {
                background-color: white;
                border: 1px solid #b3d8ff;
                border-radius: 4px;
                padding: 4px;
            }
            QPushButton {
                background-color: #409EFF;
                color: white;
                border-radius: 4px;
                padding: 5px 15px;
            }
            QPushButton:hover {
                background-color: #66b1ff;
            }
        )");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

//重写关闭事件
void LoginDialog::closeEvent(QCloseEvent *event)
{
    //通知kernel，关闭程序
    Q_EMIT sig_closeProcess();
}

//登录清除按钮
void LoginDialog::on_pb_clear_clicked()
{
    ui->le_password->setText("");
    ui->le_tel->setText("");
}

//登录提交按钮
void LoginDialog::on_pb_commit_clicked()
{
    //1.从空间上获取用户输入的数据
    QString tel = ui->le_tel->text();
    QString pass  =ui->le_password->text();
    QString telTmp = tel;
    QString passTmp  =pass;
    //2.校验数据合法性
    //2.1校验是否为空或全空格(移除所有的空格以后判断是否为空)
    if(tel.isEmpty()||pass.isEmpty()||telTmp.remove(" ").isEmpty()||
       passTmp.remove(" ").isEmpty())
    {
        QMessageBox::about(this,"提示","输入不能为空或者是全空格");
        return;
    }
    //2.2检查长度是否合法(电话号码11，密码不超过15)
    if(tel.length()!=11||pass.length()>15)
    {
        QMessageBox::about(this,"提示","电话号码必须是11，密码不超过15");
        return;
    }
    //2.3内容是否合法（密码只能是字母、数字、 下划线，电话号必须是全数字）——正则表达式
    QRegularExpression pwdReg("^[a-zA-Z0-9_]+$");
    if(!pwdReg.match(pass).hasMatch())
    {
        QMessageBox::about(this,"提示","密码只能包含字母、数字、下划线");
        return;
    }
    QRegularExpression phoneReg("^1[3-9]\\d{9}$");
    if(!phoneReg.match(tel).hasMatch())
    {
        QMessageBox::about(this,"提示","请输入正确的手机号");
        return;
    }
    //3.把登录数据发给kernel
    Q_EMIT sig_loginData(tel,pass);
}

//注册清除按钮
void LoginDialog::on_pb_clear_register_clicked()
{
    ui->le_tel_register->setText("");
    ui->le_password_register->setText("");
    ui->le_name_register->setText("");
}

//注册提交按钮
void LoginDialog::on_pb_commit_register_clicked()
{
    //1.从空间上获取用户输入的数据
    QString name = ui->le_name_register->text();
    QString tel = ui->le_tel_register->text();
    QString pass  =ui->le_password_register->text();
    QString nameTmp = name;//调用remove函数会改变字符串长度
    QString telTmp = tel;
    QString passTmp  =pass;
    //2.校验数据合法性
    //2.1校验是否为空或全空格(移除所有的空格以后判断是否为空)
    if(name.isEmpty()||tel.isEmpty()||pass.isEmpty()||
       nameTmp.remove(" ").isEmpty()||telTmp.remove(" ").isEmpty()||
       passTmp.remove(" ").isEmpty())
    {
        QMessageBox::about(this,"提示","输入不能为空或者是全空格");
        return;
    }
    //2.2检查长度是否合法(昵称不超过10，电话号码11，密码不超过15)
    if(name.length()>10||tel.length()!=11||pass.length()>15)
    {
        QMessageBox::about(this,"提示","昵称不超过10，电话号码必须是11，密码不超过15");
        return;
    }
    //2.3内容是否合法（密码只能是字母、数字、 下划线，电话号必须是全数字）——正则表达式
    QRegularExpression pwdReg("^[a-zA-Z0-9_]+$");
    if(!pwdReg.match(pass).hasMatch())
    {
        QMessageBox::about(this,"提示","密码只能包含字母、数字、下划线");
        return;
    }
    QRegularExpression phoneReg("^1[3-9]\\d{9}$");
    if(!phoneReg.match(tel).hasMatch())
    {
        QMessageBox::about(this,"提示","请输入正确的手机号");
        return;
    }
    //3.把数据发给kernel
    Q_EMIT sig_registerData(name,tel,pass);
}
