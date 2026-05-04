#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include<QMenu>
#include <QDialog>
#include<QVBoxLayout>
#include<QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include"frienditem.h"
#include"./Net/def.h"

namespace Ui {
class FriendList;
}

class FriendList : public QDialog
{
    Q_OBJECT

signals:
    //通知kernel，下线
    void sig_offline();

    //通知kernel添加好友
    void sig_addFriend();

    //通知kernel改变头像
    void sig_ChangeIcon(int m_iconid);
public:
    explicit FriendList(QWidget *parent = nullptr);
    ~FriendList();

    //设置自己的信息
    void setUserInfo(int id, QString name,QString feeling,int iconid);

    //添加到好友的列表上
    void addFriend(friendItem *item);

    //重写父类关闭事件
    void closeEvent(QCloseEvent* event);

private slots:
    void on_pb_menu_clicked();

    //点击菜单栏槽函数
    void slot_menuTriggered(QAction *action);

    void on_pb_icon_clicked();

private:
    Ui::FriendList *ui;
    //定义一个垂直布局层
    QVBoxLayout *m_pLayout;
    //定义一个菜单
    QMenu* m_pMenu;
    int m_iconid;
    int m_id;
};

#endif // FRIENDLIST_H
