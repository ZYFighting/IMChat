#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<winsock2.h>
#include<iostream>//其他库用<>
#include"def.h"//自己项目用""
#include<process.h>

using namespace std;

class INetMediator;//避免头文件相互包含
class INet
{
public:
	INet() :m_sock(INVALID_SOCKET), m_isRunning(true),m_pMediator(nullptr) {};
	~INet() {};
	//初始化网络
	virtual bool initNet() = 0;

	//发送数据(udp:ip u_long类型，决定发给谁；tcp:socket，决定发给谁——第三个参数兼容udp和tcp)
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//udp:sendto(socket,buf,len,flag,to,tolen);
	//tcp:send(socket,buf,len,flag);

	//接收数据(放在线程里)
	virtual void recvData() = 0;

	//关闭网络
	virtual void unInitNet() = 0;

protected:
	SOCKET m_sock;//一个类中不同函数可用，子类可用
	bool m_isRunning;
	INetMediator* m_pMediator;
};
