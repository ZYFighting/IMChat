#include"TcpServer.h"
#include"../mediator/TcpServerMediator.h"

TcpServer::TcpServer(INetMediator* p) {
	m_pMediator = p;
}
TcpServer::~TcpServer() {

}
//初始化网络
bool TcpServer::initNet() {
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
	//3.绑定ip和端口
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_TCP_PORT);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err)
	{
		cout << "bind error:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "bind success" << endl;
	}
	//4.监听
	err = listen(m_sock, _DEF_TCP_LISTEN_MAX);
	if (SOCKET_ERROR == err)
	{
		cout << "listen error:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "listen success" << endl;
	}
	//5.创建接受连接的线程函数
	HANDLE handle = (HANDLE)_beginthreadex(0, 0, &acceptThread, this, 0, nullptr);
	if (handle)
	{
		m_listHandle.push_back(handle);//句柄会有很多，放在链表中最后统一销毁
	}
	return true;
}

unsigned __stdcall TcpServer::recvThread(void* lpvoid)
{
	TcpServer* pThis = (TcpServer*)lpvoid;
	pThis->recvData();
	return 1;
}

unsigned __stdcall TcpServer::acceptThread(void* lpvoid) {
	cout << __func__ << endl;
	TcpServer* pThis = (TcpServer*)lpvoid;
	SOCKET sock = INVALID_SOCKET;
	sockaddr_in addr;
	HANDLE handle;
	int size = sizeof(addr);
	unsigned int threadId = 0;
	while (pThis->m_isRunning)//静态函数不能直接使用成员变量，需要定义对象来引用
	{
		//接受连接成功时返回的socket用来跟连接的这个客户端进行收发数据
		sock = accept(pThis->m_sock, (sockaddr*)&addr, &size);
		if (INVALID_SOCKET == sock)
		{
			//接受连接失败了，打印错误日志，跳出循环
			cout << "accept error:" << WSAGetLastError() << endl;
			break;
		}
		else
		{
			//连接成功，打印客户端的ip
			cout << "client ip:" << inet_ntoa(addr.sin_addr) << endl;

			//创建接受这个客户端数据的线程，每连接一个客户端就创建一个handle
			handle = (HANDLE)_beginthreadex(0, 0, &recvThread, pThis, 0, &threadId);
			if (handle)
			{
				pThis->m_listHandle.push_back(handle);
			}
			//保存socket
			pThis->m_mapThreadToSocket[threadId] = sock;
		}
	}
	return 1;
}

//发送数据
bool TcpServer::sendData(char* data, int len, unsigned long to) {
	cout << "TcpServer::" << __func__ << endl;
	//1.校验参数合法性
	if (nullptr == data || len <= 0)
	{
		cout << "paramater error" << endl;
		return false;
	}
	//2.先发包长度
	int nSendNum = send(to, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum)
	{
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}
	//3.在发包内容
	nSendNum = send(to, data, len, 0);
	if (SOCKET_ERROR == nSendNum)
	{
		cout << "send error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

//接收数据
void TcpServer::recvData() {
	cout << "TcpServer::" << __func__ << endl;
	//休眠一会：等待acceptThread把socket保存到map，再去map中取值
	Sleep(5);
	//取出当前线程的socket，用这个socket去接收这个数据
	unsigned int threadId = GetCurrentThreadId();
	SOCKET sock = INVALID_SOCKET;
	if (m_mapThreadToSocket.count(threadId) > 0)
	{
		sock = m_mapThreadToSocket[threadId];
	}
	else
	{
		cout << "socket error" << endl;
		return;
	}
	//接收数据长度
	int nRecvNum = 0;
	//保存包的长度
	int nPackLen = 0;
	//记录累计接受多少数据
	int nOffset = 0;
	while (m_isRunning)
	{
		//先接收包长度
		nRecvNum = recv(sock, (char*)&nPackLen, sizeof(int), 0);
		if (nRecvNum > 0)
		{
			//接受包长度之后，再接受数据
			//按照包长度new一个空间
			char* packBuf = new char[nPackLen];
			//当数据过大时，会有拆包现象，需要循环接收
			while (nPackLen > 0)
			{
				nRecvNum = recv(sock, packBuf + nOffset, nPackLen, 0);//此时的sock的接受连接的sock
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
			m_pMediator->transmitData(packBuf, nOffset, sock);
			//一个包的内容接受完成后，nOffset清零
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
void TcpServer::unInitNet() {
	//1.回收线程资源
	m_isRunning = false;
	HANDLE handle = nullptr;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();)
	{
		//取出当前节点中的句柄
		handle = *ite;
		//回收句柄
		if (handle)
		{
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 1000))
			{
				TerminateThread(handle, -1);
			}
			//关闭句柄
			CloseHandle(handle);
			handle = nullptr;
		}
		//从list中移除无效节点(erase移除迭代器指向的当前节点，返回下一个有效节点)
		ite = m_listHandle.erase(ite);
	}
	
	//2.关闭套接字
	if (!m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);//计数器减1，为0时操作系统自动回收
	}
	SOCKET sock = INVALID_SOCKET;
	for (auto ite = m_mapThreadToSocket.begin() ; ite != m_mapThreadToSocket.end();)
	{
		//取出当前节点中的socket
		sock = ite->second;
		//2.关闭套接字
		if (!sock && INVALID_SOCKET != sock)
		{
			closesocket(sock);
		}
		//从map中移除无效节点(erase移除迭代器指向的当前节点，返回下一个有效节点)
		ite = m_mapThreadToSocket.erase(ite);
	}
	//3.卸载库
	WSACleanup();
}