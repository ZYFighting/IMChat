#include"TcpClient.h"
#include"../mediator/TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* p):m_handle(nullptr) {
	m_pMediator = p;
}
TcpClient::~TcpClient() {

}

//接收数据的线程函数（调用接收数据的函数）
unsigned __stdcall TcpClient::recvThread(void* lpvoid) {
	TcpClient* pThis = (TcpClient*)lpvoid;
	pThis->recvData();
	return 1;
}

//初始化网络
bool TcpClient::initNet() {
	//1.加载库
	WORD version = MAKEWORD(_DEF_VERSION_HIGH, _DEF_VERSION_LOW);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
	if (0 != err)
	{
		cout << "WSAStartup fail" << endl;
		return false;
	}
	//判断端口号是否正确
	if (_DEF_VERSION_HIGH == HIBYTE(data.wVersion) && _DEF_VERSION_LOW == LOBYTE(data.wVersion))
	{
		cout << "WSAStartup success" << endl;
	}
	else
	{
		cout << "WSAStartup fail" << endl;
		return false;
	}
	//2.创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock)
	{
		cout << "socket error:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "socket success" << endl;
	}
	//3.连接服务端
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	err = connect(m_sock, (sockaddr*)&addrServer, sizeof(addrServer));
	if (SOCKET_ERROR == err)
	{
		cout << "connect error:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "connect success" << endl;
	}
	//4.创建接收数据的线程
	m_handle = (HANDLE)_beginthreadex(0, 0, &recvThread, this, 0, nullptr);
	return true;
}

//发送数据
bool TcpClient::sendData(char* data, int len, unsigned long to) {
	cout << "TcpClient::" << __func__ << endl;
	//1.校验参数合法性
	if (nullptr == data || len <= 0)
	{
		cout << "paramater error" << endl;
		return false;
	}
	//2.先发包长度
	int nSendNum = send(m_sock, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum)
	{
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}
	//3.在发包内容
	nSendNum = send(m_sock, data, len, 0);
	if (SOCKET_ERROR == nSendNum)
	{
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

//接收数据
void TcpClient::recvData() {
	cout << "TcpClient::" << __func__ << endl;
	//接收数据长度
	int nRecvNum = 0;
	//保存包的长度
	int nPackLen = 0;
	//记录累计接受多少数据
	int nOffset = 0;
	while (m_isRunning)
	{
		//先接收包长度
		nRecvNum = recv(m_sock, (char*)&nPackLen, sizeof(int), 0);
		if (nRecvNum > 0)
		{
            if (nPackLen <= 0 || nPackLen > 1024 * 1024)
            {
                std::cout << "invalid pack length: " << nPackLen << std::endl;
                break;  // 关闭连接
            }
			//接受包长度之后，再接受数据
			//按照包长度new一个空间
			char* packBuf = new char[nPackLen];
			//当数据过大时，会有拆包现象，需要循环接收
			while (nPackLen > 0)
			{
				nRecvNum = recv(m_sock, packBuf + nOffset, nPackLen, 0);
				if (nRecvNum > 0)
				{
					nOffset += nRecvNum;
					nPackLen -= nRecvNum;
				}
				else
				{
					cout << "recv error2:" << WSAGetLastError() << endl;
					break;
				}
			}
			//一个包的内容接受完成，此时nPackLen=0，nOffse变成包长度
			m_pMediator->transmitData(packBuf, nOffset, m_sock);
			nOffset = 0;
		}
		else
		{
			cout << "recv error1:" << WSAGetLastError() << endl;
			break;
		}

	}
}

//关闭网络
void TcpClient::unInitNet() {
	//1.回收线程资源
	m_isRunning = false;
	if (m_handle)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 1000))
		{
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}

	//2.关闭套接字
	if (!m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);//计数器减1，为0时操作系统自动回收
	}
	//3.卸载库
	WSACleanup();
}
