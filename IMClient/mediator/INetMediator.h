#pragma once
#include<QObject>

class INet;//先声明有INet类，可以直接使用
class INetMediator:public QObject
{
    Q_OBJECT
public:
    INetMediator();//没有cpp要在头文件中进行实现
    ~INetMediator();

	//打开网络
	virtual bool openNet() = 0;

	//关闭网络
	virtual void closeNet() = 0;

	//发送数据
	virtual bool sendData(char* data, int len, unsigned long to) = 0;

	//转发数据（把net层收到的数据传给核心处理类）
	//from数据从哪来(udp:ip u_long类型，决定数据从哪来；tcp:socket，决定数据从哪来)
	virtual void transmitData(char* data, int len, unsigned long from) = 0;

signals:
    //把接收到的数据传给核心处理类
    void sig_dealData(char* data, int len, unsigned long from);


protected:
	INet* m_pNet;
};
