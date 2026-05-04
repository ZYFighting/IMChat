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
    void sig_ChatMessage(int friendId, QString content);
    //把文件发给kernel
    void sig_sendFile(QStringList filePath, int friendId);

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

    //保存并设置好友的昵称和id
    void setFriendInfo(int id,QString name);

    //把聊天内容设置到窗口上
    void setChatMessage(QString content);

    //设置好友不在线
    void setFriendOffine();

private slots:
    //发送聊天内容
    void on_pb_send_clicked();
    //发送文件
    void on_pb_tool1_clicked();

private:
    Ui::ChatDialog *ui;
    int m_id;
    QString m_name;
};

#endif // CHATDIALOG_H
