#include "chatdialog.h"
#include "ui_chatdialog.h"
#include<QTime>
#include<QDebug>
#include<QFileDialog>
ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("D:/KnowledgeStructure/IMClient/IMClient/images/message.ico"));
}
ChatDialog::~ChatDialog()
{
    delete ui;
}
//保存并设置好友的昵称和id
void ChatDialog::setFriendInfo(int id, QString name)
{
    //保存好友的昵称和id
    m_id = id;
    m_name = name;
    //设置窗口的标题
    setWindowTitle(QString("与【%1】的聊天窗口").arg(m_name));
}
//把聊天内容设置到窗口上
void ChatDialog::setChatMessage(QString content)
{
    ui->tb_chat->append(QString("【%1】 %2").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}
//把B不下线设置在窗口上
void ChatDialog::setFriendOffine()
{
    ui->tb_chat->append(QString("【%1】 %2不在线").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
}
//点击发送按钮
void ChatDialog::on_pb_send_clicked()
{
    //1.获取用户输入的内容（纯文本）
    QString content = ui->te_chat->toPlainText();
    //2.校验内容是否为空或者全空格
    if(content.isEmpty()||content.remove(" ").isEmpty())
    {
        ui->te_chat->setText("");
        return;
    }
    //3.获取用户输入的内容（带格式的）
    content = ui->te_chat->toHtml();
    //4.把内容显示到上面的浏览窗口上
    ui->tb_chat->append(QString("【我】 %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
    //5.清空下面的编辑窗口
    ui->te_chat->setText("");
    //6.把聊天内容发给kernel
    Q_EMIT sig_ChatMessage(m_id,content);
}
//发送文件
void ChatDialog::on_pb_tool1_clicked()
{
    qDebug()<<__func__;
    //1.弹出系统文件夹，选择一个文件，将文件信息存入list
    QStringList fileList = QFileDialog::getOpenFileNames(this, "打开文件", "./", "All Files (*)");
    //判断所选文件是否为空
    if(fileList.count() <= 0)
    {
        return;
    }
    //2.把文件发送给kernel
    Q_EMIT sig_sendFile(fileList, m_id);
}
