#ifndef CKERNEL_H
#define CKERNEL_H

#include<QMap>
#include<QObject>
#include<QTextCodec>
#include<QFileInfo>
#include<QFile>
#include<QFileDialog>
#include<QDateTime>
#include<Qtime>
#include<time.h>
#include<windows.h>
#include"./Net/def.h"
#include"frienditem.h"
#include"chatdialog.h"
#include"friendlist.h"
#include<logindialog.h>
#include"./mediator/INetMediator.h"

class CKernel;
typedef void (CKernel::*PFUN)(char* data, int len, unsigned long from);
class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();
    //给函数指针数组初始化并存数据
    void setProtocol();
    //gb2312转utf8
    QString gb2312ToUtf8(char* src);//空间里是真实存在的字符串以\0为结尾，不用告诉长度
    //utf8转gb2312
    void utf8Togb2312(QString src, char* dst, int len);//目的是空间且为空，必须告诉长度
    //处理注册回复
    void dealRegisterRs(char* data, int len, unsigned long from);
    //处理登录回复
    void dealLoginRs(char* data, int len, unsigned long from);
    //处理好友信息
    void dealFriendInfo(char* data, int len, unsigned long from);
    //处理聊天请求
    void dealChatRq(char* data, int len, unsigned long from);
    //处理聊天回复
    void dealChatRs(char* data, int len, unsigned long from);
    //处理下线请求
    void dealOfflineRq(char* data, int len, unsigned long from);
    //处理添加好友请求
    void dealAddFriendRq(char* data, int len, unsigned long from);
    //处理添加好友回复
    void dealAddFriendRs(char* data, int len, unsigned long from);

signals:

private slots:
    //处理所有接收到的数据
    void slot_dealData(char* data, int len, unsigned long from);
    //处理注册数据发给服务端
    void slot_registerData(QString name,QString tel,QString pass);
    //处理登录数据发给服务端
    void slot_loginDta(QString tel,QString pass);
    //处理显式和当前好友的聊天窗口
    void slot_showChatDialog(int friendId);
    //处理聊天内容发给服务端
    void slot_ChatMessage(int friendId, QString content);
    //处理关闭程序的信号
    void slot_closeProcess();
    //处理下线信号
    void slot_offline();
    //处理添加好友的信号
    void slot_addFriend();
    //处理用户头像
    void slot_ChangeIcon(int m_iconid);
private:
    int m_id;
    QString m_tel;
    QString m_name;
    QString m_feeling;
    int m_status;
    LoginDialog*m_pLoginDlg;
    INetMediator*m_pMediator;
    FriendList*m_pFriendList;
    //声明函数指针数组
    PFUN m_protocol[_DEF_PROTOCOL_COUNT];
    //保存好友的对象(QT的控件回收机制：子控件的回收由父空间负责)
    QMap<int, friendItem*>m_mapIdToFriendItem;
    //保存跟好友的聊天
    QMap<int,ChatDialog*>m_mapIdToChatdlg;
};

#endif // CKERNEL_H
