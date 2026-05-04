#include "ckernel.h"
#include"./mediator/TcpClientMediator.h"
#include<QMessageBox>
#include<QInputDialog>
#include<QDebug>
CKernel::CKernel(QObject *parent) : QObject(parent)
{
    setProtocol();
    //new好友列表界面对象
    m_pFriendList = new FriendList;
    //绑定下线的信号和槽函数
    connect(m_pFriendList,&FriendList::sig_offline,this,&CKernel::slot_offline);
    //绑定添加好友的信号和槽函数
    connect(m_pFriendList,&FriendList::sig_addFriend,this,&CKernel::slot_addFriend);
    //绑定改变头像的信号和槽函数
    connect(m_pFriendList,&FriendList::sig_ChangeIcon,this,&CKernel::slot_ChangeIcon);
    //new一个登录窗口对象
    m_pLoginDlg = new LoginDialog;
    //显式窗口
    m_pLoginDlg->show();
    //绑定注册处理信息的信号和槽函数
    connect(m_pLoginDlg,&LoginDialog::sig_registerData,
            this,&CKernel::slot_registerData);
    connect(m_pLoginDlg,&LoginDialog::sig_loginData,
            this,&CKernel::slot_loginDta);
    connect(m_pLoginDlg,&LoginDialog::sig_closeProcess,
            this,&CKernel::slot_closeProcess);
    //new一个中介者类对象
    m_pMediator= new TcpClientMediator;
    //连接处理所有数据的信号和槽函数
    QObject::connect(m_pMediator,&TcpClientMediator::sig_dealData,
                     this,&CKernel::slot_dealData);
    //打开客户端网络
    if(!m_pMediator->openNet())
    {
        //弹出提示用户
        QMessageBox::about(m_pLoginDlg/*弹出窗口的父窗口，决定了弹出窗口的位置*/,
                           "标题"/*弹出窗口的标题*/,"打开网络失败"/*提示信息*/);
        //退出程序
        exit(1);
    }
}
CKernel::~CKernel()
{
    //回收资源
    if(m_pLoginDlg)
    {
        m_pLoginDlg->hide();//隐藏窗口
        delete m_pLoginDlg;
        m_pLoginDlg = nullptr;
    }
    if(m_pMediator)
    {
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }
    if(m_pFriendList)
    {
        m_pFriendList->hide();
        delete m_pFriendList;
        m_pFriendList = nullptr;
    }
}
//给函数指针数组初始化并存数据
void CKernel::setProtocol()
{
    qDebug()<<__func__;
    //初始化为0
    memset(m_protocol, 0, sizeof(m_protocol));
    //存入数据
    m_protocol[_DEF_REGISTER_RS   - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRs;//将函数地址存入数组
    m_protocol[_DEF_LOGIN_RS      - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRs;
    m_protocol[_DEF_FRIEND_INFO   - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealFriendInfo;
    m_protocol[_DEF_CHAT_RQ       - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
    m_protocol[_DEF_CHAT_RS       - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRs;
    m_protocol[_DEF_OFFLINE_RQ    - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
    m_protocol[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
    m_protocol[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
}
QString CKernel::gb2312ToUtf8(char *src)
{
    QTextCodec* dc = QTextCodec::codecForName("gb2312");
    return dc->toUnicode(src);
}
void CKernel::utf8Togb2312(QString src, char *dst, int len)
{
    QTextCodec* dc = QTextCodec::codecForName("gb2312");
    QByteArray ba = dc->fromUnicode(src);
    strcpy_s(dst, len, ba.data());
}
//处理注册回复
void CKernel::dealRegisterRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_REGISTER_RS*rs = (_STRU_REGISTER_RS*)data;
    //2.根据结果提示用户
    switch (rs->result){
        case _def_register_success:
            QMessageBox::about(m_pLoginDlg,"提示","注册成功");
            break;
        case _def_register_tel_exists:
            QMessageBox::about(m_pLoginDlg,"提示","注册失败，电话号码已被注册");
            break;
        case _def_register_name_exists:
            QMessageBox::about(m_pLoginDlg,"提示","注册失败，昵称已经被注册");
            break;
        default:
            break;
    }
}
//处理登录回复
void CKernel::dealLoginRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_LOGIN_RS*rs = (_STRU_LOGIN_RS*)data;//客户端处理回复
    qDebug()<<"abc:"<<rs->result;
    //2.根据结果提示用户
    switch (rs->result)
    {
    case _def_login_success:
        {
            //保存当前登录用户的ID
            m_id = rs->userId;

            //隐藏登陆界面，显示好友列表
            m_pLoginDlg->hide();
            m_pFriendList->show();
        }
        break;
    case _def_login_tel_not_exists:
        QMessageBox::about(m_pLoginDlg,"提示","登录失败，电话号码未注册");
        break;
    case _def_login_passwd_error:
        QMessageBox::about(m_pLoginDlg,"提示","登录失败，密码错误");
        break;
    case _def_status_online:
        QMessageBox::about(m_pLoginDlg,"提示","登录失败，该用户已在线");
    default:
        break;
    }
}
//处理自己和好友信息
void CKernel::dealFriendInfo(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_FRIEND_INFO*info = (_STRU_FRIEND_INFO*)data;
    QString tel = gb2312ToUtf8(info->tel);
    QString name = gb2312ToUtf8(info->name);
    QString feeling = gb2312ToUtf8(info->feeling);
    //2.判断是不是自己的信息
    if(m_id == info->id)
    {
        //保存自己的信息
        m_tel = tel;
        m_name = name;
        m_feeling = feeling;
        m_status = info->status;
        //把自己的信息设置到用户界面
        m_pFriendList->setUserInfo(info->id,name,feeling,info->iconid);
        return;
    }
    //是好友的信息，判断是否添加到列表上
    if(m_mapIdToFriendItem.count(info->id)>0)
    {
        //如果已经在列表上
        //取出好友对象
        friendItem *item = m_mapIdToFriendItem[info->id];
        //更新好友的信息
        item->setFriendInfo(info->id,tel,name,feeling,info->iconid,info->status);
    }
    else
    {
        //没有在列表上
        //new一个好友
        friendItem* item = new friendItem;
        //设置好友的信息
        item->setFriendInfo(info->id,tel,name,feeling,info->iconid,info->status);
        //把好友添加到列表上
        m_pFriendList->addFriend(item);
        //保存好友对象
        m_mapIdToFriendItem[info->id] = item;
        //绑定显式聊天窗口的信号和槽函数
        connect(item,&friendItem::sig_showChatDialog,this,
                &CKernel::slot_showChatDialog);
        //new一个跟这个好友的聊天窗口
        ChatDialog* chat = new ChatDialog;
        //设置聊天窗口属性
        chat->setFriendInfo(info->id, name);
        //把聊天窗口保存在map中
        m_mapIdToChatdlg[info->id] = chat;
        //绑定发送聊天内容的信号和槽函数
        connect(chat,&ChatDialog::sig_ChatMessage,this,
                &CKernel::slot_ChatMessage);
    }
}
//处理聊天请求(是B客户端)
void CKernel::dealChatRq(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_CHAT_RQ*rq = (_STRU_CHAT_RQ*)data;
    //2.判断跟A客户端的聊天窗口是否存在
    if(m_mapIdToChatdlg.count(rq->userId) > 0)
    {
        //取出跟A的聊天窗口
        ChatDialog* chat = m_mapIdToChatdlg[rq->userId];
        //把聊天内容设置在窗口上
        chat->setChatMessage(rq->content);
        //显式窗口
        chat->show();
    }
}
//处理聊天回复（是A客户端，B好友不在线）
void CKernel::dealChatRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_CHAT_RS*rs = (_STRU_CHAT_RS*)data;
    //2.判断跟B客户端的聊天窗口是否存在
    if(m_mapIdToChatdlg.count(rs->friendId) > 0)
    {
        //取出跟B的聊天窗口
        ChatDialog* chat = m_mapIdToChatdlg[rs->friendId];
        //把B不在线设置在窗口上
        chat->setFriendOffine();
        //显式窗口
        chat->show();
    }
}
//处理下线请求(是B客户端，好友A下线了)
void CKernel::dealOfflineRq(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;
    //2.找到好友A的friendItem
    if(m_mapIdToFriendItem.count(rq->userId)>0)
    {
        //取出好友的friendItem
        friendItem*item = m_mapIdToFriendItem[rq->userId];
        //设置好友下线状态
        item->setFriendOffline();
    }
}
//处理添加好友请求(B客户端)
void CKernel::dealAddFriendRq(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RQ*rq = (_STRU_ADD_FRIEND_RQ*)data;
    //2.弹出询问窗口，询问用户是否同意添加好友
    _STRU_ADD_FRIEND_RS rs;
    rs.userId = rq->userId;
    rs.friendId = m_id;
    strcpy_s(rs.userName,sizeof (rs.userName),rq->userName);
    strcpy_s(rs.friendTel,sizeof (rs.friendTel),rq->friendTel);
    strcpy_s(rs.friendName,sizeof (rs.friendName),m_name.toStdString().c_str());
    if(QMessageBox::Yes == QMessageBox::question(m_pFriendList,"添加好友",QString("【%1】想添加你为好友，是否同意？").arg(rq->userName)))
    {
        rs.result = _def_add_friend_success;
    }
    else
    {
        rs.result = _def_add_friend_refuse;
    }
    //3.添加结果发给服务端
    m_pMediator->sendData((char*)&rs,sizeof(rs),65);
}
//处理添加好友回复（A客户端）
void CKernel::dealAddFriendRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1.拆包
    _STRU_ADD_FRIEND_RS*rs = (_STRU_ADD_FRIEND_RS*)data;
    QString friendName = rs->friendName;
    QString friendTel = gb2312ToUtf8(rs->friendTel);
    //2.根据添加结果提示用户
    switch (rs->result) {
    case _def_add_friend_success:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】为好友成功").arg(friendName));
        break;
    case _def_add_friend_offline:
        friendName = gb2312ToUtf8(rs->friendName);
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】为好友失败，好友不在线").arg(friendName));
        break;
    case _def_add_friend_refuse:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】为好友失败，好友拒绝").arg(friendName));
        break;
    case _def_add_friend_not_exists:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】为好友失败，好友不存在").arg(friendTel));
        break;
    default:
        break;
    }
}
//处理所有接收的数据
void CKernel::slot_dealData(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //取出协议头
    packtype type = *(packtype*)data;//取出data中前四个字节的内容
    //计算下标
    int index = type - _DEF_PROTOCOL_BASE - 1;
    //判断下标是否在有效范围内
    if (0 <= index && index < _DEF_PROTOCOL_COUNT)
    {
        //根据数组下标取出函数地址
        PFUN pf = m_protocol[index];//指针
        if (pf)
        {
            (this->*pf)(data, len, from);//因为在类作用域中，用this调用函数
        }
        else
        {
            //打印type2原因：1.定义结构体的时候，type值错误  2.对端发送的结构体不对
            qDebug() << "type2:" << type;
        }
    }
    else
    {
        //打印type1原因：1.packtype没有放在结构体第一个位置  2.接收数据的时候Offset没清零
        qDebug() << "type1:" << type;
    }
    //回收空间
    delete[]data;
}
//处理注册数据，发给服务端
void CKernel::slot_registerData(QString name, QString tel, QString pass)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_REGISTER_RQ rq;
    utf8Togb2312(name,rq.name,sizeof(rq.name));//转码
    strcpy_s(rq.password,sizeof(rq.password),pass.toStdString().c_str());
    strcpy_s(rq.tel,     sizeof(rq.tel),     tel.toStdString().c_str());
    //2.发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),89);
}
//处理登录数据，发给服务端
void CKernel::slot_loginDta(QString tel, QString pass)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_LOGIN_RQ rq;
                                        //QString->char*
    strcpy_s(rq.password,sizeof(rq.password),pass.toStdString().c_str());
    strcpy_s(rq.tel,     sizeof(rq.tel),     tel.toStdString().c_str());
    //2.发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),80);
}
//处理显式和当前好友的聊天窗口
void CKernel::slot_showChatDialog(int friendId)
{
    qDebug()<<__func__;
    //判断map中有没有这个聊天窗口
    if(m_mapIdToChatdlg.count(friendId) > 0)
    {
        ChatDialog* chat = m_mapIdToChatdlg[friendId];
        chat->show();
    }
}
//处理聊天内容发给服务端
void CKernel::slot_ChatMessage(int friendId, QString content)
{
    qDebug()<<__func__;
    //1.打包
    _STRU_CHAT_RQ rq;
    rq.userId = m_id;
    rq.friendId = friendId;
    strcpy_s(rq.content, sizeof (rq.content), content.toStdString().c_str());

    //2.发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),89);
}
//处理关闭程序的信号
void CKernel::slot_closeProcess()
{
    qDebug()<<__func__;
    //1.回收资源
    if(m_pLoginDlg)
    {
        m_pLoginDlg->hide();
        delete m_pLoginDlg;
        m_pLoginDlg = nullptr;
    }
    if(m_pMediator)
    {
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }
    if(m_pFriendList)
    {
        m_pFriendList->hide();
        delete m_pFriendList;
        m_pFriendList = nullptr;
    }
    for(auto ite = m_mapIdToChatdlg.begin(); ite != m_mapIdToChatdlg.end();)
    {
        //取出节点中窗口对象
        ChatDialog* chat = ite.value();
        if(chat)
        {
            chat->hide();
            delete chat;
            chat = nullptr;
        }
        //把无效节点从map移除
        ite = m_mapIdToChatdlg.erase(ite);
    }
    //2.退出进程
    exit(0);
}
//处理下线信号
void CKernel::slot_offline()
{
    qDebug()<<__func__;
    //1.给服务端发送下线请求,打包
    _STRU_OFFLINE_RQ rq;
    rq.userId = m_id;
    m_pMediator->sendData((char*)&rq, sizeof(rq), 76);
    //2.回收资源，退出进程
    slot_closeProcess();
}
//处理添加好友的信号
void CKernel::slot_addFriend()
{
    qDebug()<<__func__;
    //弹出一个输入窗口，让用户输入好友的昵称
    QString tel = QInputDialog::getText(m_pFriendList,"添加好友","请输入手机号");
    QString telTmp = tel;
    //判断字符串是否为空或全空格
    if(tel.isEmpty()||telTmp.remove(" ").isEmpty())
    {
        QMessageBox::about(m_pFriendList,"提示","请输入正确的手机号");
        return;
    }
    //判断是否是自己的昵称:定义成员变量，在处理自己和好友信息中保存昵称
    if(m_tel == tel)
    {
        QMessageBox::about(m_pFriendList,"提示","不能添加自己为好友");
        return;
    }
    //判断是否已经是好友
    for(auto ite = m_mapIdToFriendItem.begin();ite!=m_mapIdToFriendItem.end();ite++)
    {
        //取出好友的friendItem
        friendItem* item = ite.value();
        //判断好友的昵称和输入的昵称是否相同:friendItem中的m_name是私有的不能通过对象直接使用，
        //可以对该成员变量设置getter函数，获取该值
        if(tel == item->tel())
        {
            QMessageBox::about(m_pFriendList,"提示","已经是好友，不能重复添加");
            return;
        }
    }
    //给服务端发送添加好友
    _STRU_ADD_FRIEND_RQ rq;
    rq.userId = m_id;
    strcpy_s(rq.userName,sizeof (rq.userName),m_name.toStdString().c_str());//自己的名字
    utf8Togb2312(tel,rq.friendTel,sizeof (rq.friendTel));//想要添加好友的手机号
    m_pMediator->sendData((char*)&rq,sizeof(rq),90);
}

//处理改变用户头像的信号
void CKernel::slot_ChangeIcon(int m_iconid)
{
    _STRU_FRIEND_INFO userInfo;
    userInfo.id = m_id;
    userInfo.iconid = m_iconid;
    userInfo.status = m_status;
    utf8Togb2312(m_name,userInfo.name,sizeof (userInfo.name));
    utf8Togb2312(m_feeling,userInfo.feeling,sizeof (userInfo.feeling));
    m_pMediator->sendData((char*)&userInfo,sizeof(userInfo),99);
}
