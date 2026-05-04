#include "frienditem.h"
#include "ui_frienditem.h"
#include"Net/def.h"
#include<QBitmap>
friendItem::friendItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::friendItem)
{
    ui->setupUi(this);
}
friendItem::~friendItem()
{
    delete ui;
}
//保存到成员变量中并设置好友信息
void friendItem::setFriendInfo(int friendId, QString tel, QString name, QString feeling, int iconId, int status)
{
    //1.保存到成员变量
    m_friendId = friendId;
    m_name = name;
    m_feeling = feeling;
    m_iconId = iconId;
    m_status = status;
    m_tel = tel;
    //2.设置昵称和签名
    ui->lb_name->setText(m_name);
    ui->lb_feeling->setText(m_feeling);
    //3.设置头像
    //拼接头像图片的文件的路径
    QString path = QString(":/tx/%1.png").arg(iconId);
    //判断状态是否在线
    if(_def_status_online == m_status)
    {
        //在线，亮显头像
        QIcon icon = QIcon(path);
        ui->pb_icon->setIcon(icon);
    }
    else
    {
        //不在线，暗显头像
        QBitmap bp;
        bp.load(path);
        ui->pb_icon->setIcon(bp);
    }
}
//设置好友下线状态
void friendItem::setFriendOffline()
{
    //设置状态为下线
    m_status = _def_status_offline;
    //头像暗线
    QString path = QString(":/tx/%1.png").arg(m_iconId);
    QBitmap bp;
    bp.load(path);
    ui->pb_icon->setIcon(bp);
    //重绘
    repaint();
}
//点击好友头像
void friendItem::on_pb_icon_clicked()
{
    //通知kernel显式和当前好友的聊天窗口
    Q_EMIT sig_showChatDialog(m_friendId);
}
const QString &friendItem::tel() const
{
    return m_tel;
}
