#ifndef FRIENDITEM_H
#define FRIENDITEM_H

#include <QWidget>

namespace Ui {
class friendItem;
}

class friendItem : public QWidget
{
    Q_OBJECT
signals:
    //通知kernel，显式和当前好友的聊天窗口
    void sig_showChatDialog(int friendId);

public:
    explicit friendItem(QWidget *parent = nullptr);
    ~friendItem();

    //保存到成员变量中并设置好友信息
    void setFriendInfo(int friendId, QString tel, QString name, QString feeling, int iconId, int status);

    //设置好友下线状态
    void setFriendOffline();

    const QString &tel() const;

private slots:
    void on_pb_icon_clicked();

private:
    Ui::friendItem *ui;
    int m_friendId;
    QString m_tel;
    QString m_name;
    QString m_feeling;
    int m_iconId;
    int m_status;
};

#endif // FRIENDITEM_H
