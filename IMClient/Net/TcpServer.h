#include"INet.h"
#include<list>
#include<map>

class TcpServer :public INet
{
public:
	TcpServer(INetMediator* p);
	~TcpServer();
	//初始化网络
	bool initNet();

	//发送数据
	bool sendData(char* data, int len, unsigned long to);

	//接收数据
	void recvData();

	//关闭网络
	void unInitNet();

	//接收连接的线程函数
	static unsigned __stdcall acceptThread(void*);

	//接收数据的线程函数
	static unsigned __stdcall recvThread(void*);
private:
	list<HANDLE> m_listHandle;
	map<unsigned int, SOCKET>m_mapThreadToSocket;
};