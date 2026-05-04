#pragma once
#include<iostream>
#include<map>
#include"./Net/def.h"
#include"./MySQL/CMySql.h"
#include"mediator/INetMediator.h"
using namespace std;

//声明指向函数的指针
class CKernel;
typedef void (CKernel::*PFUN)(char* data, int len, unsigned long from);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//给函数指针数组初始化并存数据
	void setProtocol();
	//打开服务器（打开网络，连接数据库）
	bool startServer();
	//关闭服务器（回收资源，关闭网络，断开跟数据库的连接）
	void endServer();
	//获取当前登录用户的信息以及好友的信息
	void getUserInfoAndFriendInfo(int userId);
	//根据id查询用户信息
	void getInfoById(int id, _STRU_FRIEND_INFO* info/*查询到的用户信息作为输出参数返回给调用函数*/);
	//处理所有接收到的数据
	void dealData(char* data, int len, unsigned long from);
	//处理注册请求
	void dealRegisterRq(char* data, int len, unsigned long from);
	//处理登录请求
	void dealLoginRq(char* data, int len, unsigned long from);
	//处理聊天请求函数
	void dealChatRq(char* data, int len, unsigned long from);
	//处理下线请求函数
	void dealOfflineRq(char* data, int len, unsigned long from);
	//处理添加好友请求
	void dealAddFriendRq(char* data, int len, unsigned long from);
	//处理添加好友回复
	void dealAddFriendRs(char* data, int len, unsigned long from);
	//处理文件信息请求
	void dealFileInfoRq(char* data, int len, unsigned long from);
	//处理文件块请求
	void dealFileBlockRq(char* data, int len, unsigned long from);
	//处理文件信息回复
	void dealFileInfoRs(char* data, int len, unsigned long from);
	//处理用户信息
	void dealUserInfo(char* data, int len, unsigned long from);

	static CKernel* pKernel;//解决头文件重复包含
private:
	INetMediator* m_pMediator;
	CMySql m_sql;
	//声明函数指针数组
	PFUN m_protocol[_DEF_PROTOCOL_COUNT];
	map<int, SOCKET>m_mapIdToSocket;//socket池存放客户端的id，sock
};

