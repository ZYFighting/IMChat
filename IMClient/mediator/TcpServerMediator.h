#include"INetMediator.h"

class TcpServerMediator :public INetMediator
{
public:
	TcpServerMediator();
	~TcpServerMediator();

	//打开网络
	bool openNet();

	//关闭网络
	void closeNet();

	//发送数据
	bool sendData(char* data, int len, unsigned long to);

	//转发数据（把net层收到的数据传给核心处理类）
	//from数据从哪来(udp:ip u_long类型，决定数据从哪来；tcp:socket，决定数据从哪来)
	void transmitData(char* data, int len, unsigned long from);
};