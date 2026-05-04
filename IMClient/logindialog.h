#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT
signals:
    //把注册数据发给kernel
    void sig_registerData(QString name,QString tel,QString pass);

    //把登陆数据发给kernel
    void sig_loginData(QString tel,QString pass);

    //通知kernel，关闭程序
    void sig_closeProcess();

public:
    LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    //重写关闭事件
    void closeEvent(QCloseEvent *event);//只有登录窗口才能下线
                           //回收该类的的指针，但是该类没有指针，而kernel有很多指针

private slots:
    void on_pb_clear_clicked();

    void on_pb_commit_clicked();

    void on_pb_clear_register_clicked();

    void on_pb_commit_register_clicked();

private:
    Ui::LoginDialog *ui;
};
#endif // LOGINDIALOG_H
