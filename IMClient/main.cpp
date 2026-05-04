#include "ckernel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CKernel kernel;
    return a.exec();
}


//QT的信号和槽机制（通知事件发生或者传输数据，两个类之间）
//1.两个类必须直接或者间接继承QObject，并且有Q_OBJECT
//2.在发送数据的类的头文件使用signals声明函数，返回值只能是void，参数就是要传输数据，也可以没有参数（单纯通知事情的发生）
//  信号不是函数，不需要在cpp中实现，需要在通知或者发送数据的地方使用Q_EMIT/qemit信号名（参数列表）
//3.接受数据的头文件中使用public/private/protected slots声明槽函数，槽函数的参数和返回值要和信号一致
//  槽函数需要在cpp中实现
//4.在接收数据的类中，发送信号的对象new出来的下面，连接信号和槽函数


//字符串可以存在char*、QString、std::string
//char*可以直接给QString和std::string赋值，因为类里面重载了等号操作符
//std::string.c_str=》char*
//QString.toStdString()=>std::string.c_str()=>char*


//QT使用utf-8的编码方式，vs使用gb2312的编码方式
//统一在客户端转码
//QT先转gb2312，再发给vs
//QT接收到的数据，先转成UTF-8，再设置到界面上
//UTF-8编码方式的字符换选保存在QString里
//gb2312编码方式的字符串保存在char*里
