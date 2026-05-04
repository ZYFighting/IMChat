#include"Udp.h"
#include"../mediator/UdpMediator.h"

Udp::Udp(INetMediator* p):m_handle(nullptr) {
	m_pMediator = p;
}
Udp::~Udp() {

}

//接收数据的线程函数（调用接收数据的函数）,帮助文档查询_beginthreadex
unsigned __stdcall Udp::recvThread(void* lpvoid) {
	Udp* pThis = (Udp*)lpvoid;//this为Udp*，要将void*类型的lpvoid转为Udp*
	pThis->recvData();//使用对象调用函数，创建Udp时已有对象，直接传入
	return 1;
}

//初始化网络:加载库，创建套接字，绑定ip和端口，创建接收数据的线程
bool Udp::initNet() {
	//1.加载库
	//魔鬼数字：把数字定义成宏
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
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
	addr.sin_port = htons(_DEF_UDP_PORT);//转换为网络字节序
	addr.sin_addr.S_un.S_addr = INADDR_ANY;//绑定任意网卡
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
	//4.创建接收数据的线程
	//CreateThread和ExitThread是一对，如果在线程中调用C++运行库时（如strcpy），这些函数会申请空间并且自己不会释放掉
	//ExitThread在退出线程的时候不会回收申请的空间，就会造成内存泄漏
	//_beginthreadex和_endthreadex是一对，_endthreadex在退出线程的时候会先回收空间，再调用ExitThread
	m_handle =(HANDLE)_beginthreadex(0/*线程安全级别，0是默认的安全级别*/,
				   0/*线程堆栈大小，使用默认堆栈大小1M*/,
				   &recvThread/*线程要执行函数的起始地址——编译期（创建对象之前）就要使用*/,
				   this/*线程要执行函数的参数——this为当前对象，赋给lpvoid*/,
				   0/*线程创建的状态，0是创建以后立刻开始运行，CREATE_SUSPENDED是创建以后挂起*/,
				   nullptr/*创建线程以后，操作系统给每个线程分配的线程id*/);
	return true;
}

//发送数据
bool Udp::sendData(char* data, int len, unsigned long to) {
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);
	addr.sin_addr.S_un.S_addr = to;
	int nSendNum = sendto(m_sock, data, len, 0, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == nSendNum)
	{
		cout << "sendto error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

//接收数据(放在线程里，一直循环等待接收数据)
void Udp::recvData() {
	cout << __func__ << endl;
	int nRecvNum;
	char recvBuf[4096] = "";
	sockaddr_in addrFrom = {};
	int size = sizeof(addrFrom);
	while (m_isRunning)
	{
		nRecvNum = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &size);
		if (nRecvNum > 0)
		{
			//接收一个数据包成功
			//根据接收数据的大小申请一个新的空间
			char* pPack = new char[nRecvNum];
			//把接收到的数据拷贝到新的空间中
			memcpy(pPack, recvBuf, nRecvNum);
			//TODO：把接收到的数据传给中介者类
			m_pMediator->transmitData(pPack, nRecvNum, addrFrom.sin_addr.S_un.S_addr);
		}
		else
		{
			cout << "recvfrom error:" << WSAGetLastError() << endl;
			break;
		}
	}
}

//关闭网络
void Udp::unInitNet() {
	//1.回收线程资源
	//创建线程时，操作系统给每个线程分配3个资源：句柄（对应线程），线程id，内核对象，引用计数器是2
	//想要回收线程资源，就需要让引用计数器变为0：结束线程工作：关闭句柄
	m_isRunning = false;//跳出接收数据循环
	if (m_handle)
	{
		//返回值如果等于WAIT_TIMEOUT，就说明等待的线程在等待时间结束后，还在继续运行
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle/*等待哪个线程，就填哪个线程的句柄*/, 1000/*等待的时间，单位是毫秒*/))
		{
			//如果线程还在运行就强制杀死
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