#include "friendlist.h"
#include "ui_friendlist.h"
#include<QMessageBox>
#include<QDebug>

FriendList::FriendList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FriendList)
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
            QPushButton:hover {
                background-color: #66b1ff;
            }
            QWidget#wdg_list {
                background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                                                  stop:0 #4FACFE, stop:1 #00F2FE);
            }
        )");
    //new一个垂直布局的层对象
    m_pLayout = new QVBoxLayout;
    //设置小控件之间的间距
    m_pLayout->setSpacing(3);
    //设置小控件和大控件的
    m_pLayout->setContentsMargins(0, 0, 0, 0);
    //把层设置到大控件上
    ui->wdg_list->setLayout(m_pLayout);
    //new一个菜单对象
    m_pMenu = new QMenu(this);//有参构造的父窗口
    //添加菜单项
    m_pMenu->addAction("添加好友");//由QMenu触发信号，只需要实现槽函数和连接
    m_pMenu->addAction("系统设置");
    //绑定点击菜单栏的信号和槽函数
    connect(m_pMenu,&QMenu::triggered,this,&FriendList::slot_menuTriggered);
}

FriendList::~FriendList()
{
    if(m_pLayout)
    {
        delete m_pLayout;
        m_pLayout = nullptr;
    }
    delete ui;
}

//设置用户信息到控件
void FriendList::setUserInfo(int id, QString name, QString feeling, int iconid)
{
    m_id = id;
    ui->lb_name->setText(name);
    ui->le_feeling->setText(feeling);
    //拼接头像图片的文件的路径
    QString path = QString(":/tx/%1.png").arg(iconid);
    ui->pb_icon->setIcon(QIcon(path));
}

//添加到好友的列表上
void FriendList::addFriend(friendItem *item)
{
    m_pLayout->addWidget(item);
}

//重写父类关闭事件
void FriendList::closeEvent(QCloseEvent *event)
{
    //忽略关闭事件（如果不忽略，不管子类是否处理关闭事件，父类都会处理）
    event->ignore();
    //弹询问窗口，询问用户是否关闭
    if(QMessageBox::Yes == QMessageBox::question(this, "询问", "是否确认关闭？"))
    {
        //通知kernel，下线
        Q_EMIT sig_offline();
    }
}

//在鼠标点击的位置向上弹出一个菜单栏
void FriendList::on_pb_menu_clicked()
{
    //1.获取鼠标点击按钮
    QPoint p = QCursor::pos();
    //获取菜单栏的绝对大小
    QSize size = m_pMenu->sizeHint();
    //2.显示菜单
    QPoint pa = QPoint(p.x(),p.y() - size.height());
    m_pMenu->exec(pa);
}

//点击菜单栏槽函数
void FriendList::slot_menuTriggered(QAction *action)
{
    qDebug()<<__func__;
    //判断用户点击的是哪个菜单项
    if(action->text() == "添加好友")
    {
        //通知kernel添加好友
        Q_EMIT sig_addFriend();
    }
    else if(action->text() == "系统设置")
    {
        qDebug()<<"系统设置";
    }
}

//点击更换头像
void FriendList::on_pb_icon_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择头像",
        ":/tx",
        "图片文件 (*.png *.jpg *.jpeg *.bmp)"
    );
    if (filePath.isEmpty()) {
        return;
    }
    //解析选中的文件名，提取头像ID
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.baseName();
    bool ok;
    int newIconId = fileName.toInt(&ok);
    if (!ok || newIconId < 0 || newIconId > 36) {
        QMessageBox::warning(this, "提示", "请选择tx文件夹中命名为数字的头像文件！");
        return;
    }
    // 3. 更新界面显示
    QString newPath = QString(":/tx/%1.png").arg(newIconId);
    ui->pb_icon->setIcon(QIcon(newPath));
    ui->pb_icon->setIconSize(ui->pb_icon->size());
    // 4. 保存新的iconId到成员变量
    m_iconid = newIconId;
    //将头像id发给服务端
    Q_EMIT sig_ChangeIcon(m_iconid);
}

