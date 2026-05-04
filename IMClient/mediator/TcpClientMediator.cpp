#include"TcpClientMediator.h"
#include"../Net/TcpClient.h"

TcpClientMediator::TcpClientMediator() {
    m_pNet = new TcpClient(this);
    m_heartbeat = new ClientHeartbeat(this, this);
    connect(m_heartbeat, &ClientHeartbeat::sig_heartbeatTimeout, this, [=]() {
        qDebug() << "心跳超时，重连...";
        closeNet();
        openNet();
    });
}

TcpClientMediator::~TcpClientMediator() {
	if (m_pNet)
	{
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
        delete m_heartbeat;
        m_heartbeat = nullptr;
	}
}

//打开网络
bool TcpClientMediator::openNet() {
    bool ret = m_pNet->initNet();
    if(ret){
        m_heartbeat->start();
    }
    return ret;
}

//关闭网络
void TcpClientMediator::closeNet() {
    m_heartbeat->stopFromAnyThread();
    m_pNet->unInitNet();
}

//发送数据
bool TcpClientMediator::sendData(char* data, int len, unsigned long to) {
	return m_pNet->sendData(data, len, to);
}

//转发数据（把net层收到的数据传给核心处理类）
//from数据从哪来(udp:ip u_long类型，决定数据从哪来；tcp:socket，决定数据从哪来)
void TcpClientMediator::transmitData(char* data, int len, unsigned long from) {
    if (len == strlen(HEARTBEAT_RESP) && strncmp(data, HEARTBEAT_RESP, len) == 0)
    {
        QMetaObject::invokeMethod(m_heartbeat, "onHeartbeatReceived", Qt::QueuedConnection);
        return;
    }
    //传给核心处理类
    Q_EMIT sig_dealData(data,len,from);
}
