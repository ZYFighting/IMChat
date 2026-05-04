#include"INet.h"
class Udp:public INet
{
public:
	Udp(INetMediator* p);
	~Udp() ;
	//初始化网络
	bool initNet();

	//发送数据
	bool sendData(char* data, int len, unsigned long to);

	//接收数据
	void recvData();

	//关闭网络
	void unInitNet();

	//接收数据的线程函数
	static unsigned __stdcall recvThread(void*);//静态函数没有定义对象时就可以使用
private:
	HANDLE m_handle;
};