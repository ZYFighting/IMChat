#include"TcpClientMediator.h"
#include"../Net/TcpClient.h"

TcpClientMediator::TcpClientMediator() {
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {
	if (m_pNet)
	{
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

//打开网络
bool TcpClientMediator::openNet() {
	return m_pNet->initNet();
}

//关闭网络
void TcpClientMediator::closeNet() {
	m_pNet->unInitNet();
}

//发送数据
bool TcpClientMediator::sendData(char* data, int len, unsigned long to) {
	return m_pNet->sendData(data, len, to);
}

//转发数据（把net层收到的数据传给核心处理类）
//from数据从哪来(udp:ip u_long类型，决定数据从哪来；tcp:socket，决定数据从哪来)
void TcpClientMediator::transmitData(char* data, int len, unsigned long from) {
    //传给核心处理类
    Q_EMIT sig_dealData(data,len,from);
}
