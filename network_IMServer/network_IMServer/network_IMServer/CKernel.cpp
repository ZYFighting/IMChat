#include "CKernel.h"
#include"mediator/TcpServerMediator.h"
CKernel* CKernel::pKernel = nullptr;
CKernel::CKernel() :m_pMediator(nullptr) {
	pKernel = this;
	setProtocol();
}
CKernel::~CKernel() {

}
//给函数指针数组初始化并存数据
void CKernel::setProtocol()
{
	cout << __func__ << endl;
	//初始化为0
	memset(m_protocol, 0, sizeof(m_protocol));
	//存入数据
	m_protocol[_DEF_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;//将函数地址存入数组
	m_protocol[_DEF_LOGIN_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_protocol[_DEF_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_protocol[_DEF_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_protocol[_DEF_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_protocol[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
	m_protocol[_DEF_FRIEND_INFO - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealUserInfo;
}
//打开服务器（打开网络，连接数据库）
bool CKernel::startServer() {
	//1.打开网络
	m_pMediator = new TcpServerMediator;
	if (!m_pMediator->openNet())
	{
		cout << "打开网络失败" << endl;
	}
	//2.连接数据库
	char ip[] = "127.0.0.1";
	char user[] = "root";
	char pass[] = "zhangyu0416..";
	char dp[] = "im";
	if (!m_sql.ConnectMySql(ip, user, pass, dp))
	{
		cout << "连接数据库失败" << endl;
		return false;
	}
	return true;
}
//关闭服务器（回收资源，关闭网络，断开跟数据库的连接）
void CKernel::endServer() {
	if (m_pMediator)
	{
		m_pMediator->closeNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}
//处理所有接收到的数据
void CKernel::dealData(char* data, int len, unsigned long from) {//服务端处理请求
	cout << __func__ << endl;
	//取出协议头
	packtype type = *(packtype*)data;//将data强转成int*，取出data中前四个字节的内容	
	//计算下标
	int index = type - _DEF_PROTOCOL_BASE - 1;
	//判断下标是否在有效范围内
	if (0 <= index && index < _DEF_PROTOCOL_COUNT)
	{
		//根据数组下标取出函数地址
		PFUN pf = m_protocol[index];//指针
		if (pf)
		{
			(this->*pf)(data, len, from);//因为在类作用域中，用this调用函数
		}
		else
		{
			//打印type2原因：1.定义结构体的时候，type值错误  2.对端发送的结构体不对
			cout << "type2:" << type << endl;
		}
	}
	else
	{
		//打印type1原因：1.packtype没有放在结构体第一个位置  2.接收数据的时候Offset没清零
		cout << "type1:" << type << endl;
	}
	//回收空间
	delete[]data;

}
//处理注册请求
void CKernel::dealRegisterRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1.拆包
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;//收到客户端的请求，进行处理
	//2.校验电话号码是否被注册
	//从数据库中，根据电话号码查询电话号码
	list<string>lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select tel from t_use where tel = '%s';", rq->tel);//将字符串打印到指定空间内
	if (!m_sql.SelectMySql(sql/*要执行的sql语句*/,
		1/*sql语句中查询的列的个数*/,
		lstStr/*sql语句查询到的结果*/))
	{
		//查询数据库失败原因只有两个
		//1.没有连接数据库
		//2.sql语句有语法错误或者列名和表对应不上(把日志中打印的sql语句拷贝到workbench里面运行)
		cout << "查询数据库失败" << sql << endl;
		return;
	}
	//判断查询结果是否为空
	if (0 != lstStr.size())
	{
		//说明电话号码被注册过，注册失败,_def_register_tel_exists
		_STRU_REGISTER_RS rs;//给客户端回复
		rs.result = _def_register_tel_exists;
		//给客户端回复注册结果
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		//结束
		return;
	}
	//3.校验昵称是否被校验
	//从数据库中，根据昵称查询昵称
	sprintf_s(sql, "select tel from t_use where name = '%s';", rq->name);
	if (!m_sql.SelectMySql(sql, 1, lstStr))
	{
		//查询数据库失败原因只有两个
		//1.没有连接数据库
		//2.sql语句有语法错误或者列名和表对应不上(把日志中打印的sql语句拷贝到workbench里面运行)
		cout << "查询数据库失败" << sql << endl;
	}
	if (0 != lstStr.size())
	{
		//说明昵称被注册过，注册失败,_def_register_name_exists
		_STRU_REGISTER_RS rs;//给客户端回复
		rs.result = _def_register_name_exists;
		//给客户端回复注册结果
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		//结束
		return;
	}
	//4.把注册数据插入数据库
	sprintf_s(sql, "insert into t_use (name, tel, passw, feeling, iconid) values ('%s', '%s', '%s', '等风来不如追风去', 8);",
		rq->name, rq->tel, rq->password);
	if (!m_sql.UpdateMySql(sql))
	{
		//查询数据库失败原因只有两个
		//1.没有连接数据库
		//2.sql语句有语法错误或者列名和表对应不上(把日志中打印的sql语句拷贝到workbench里面运行)
		cout << "查询数据库失败" << sql << endl;
	}
	//5.注册成功
	_STRU_REGISTER_RS rs;//给客户端回复
	rs.result = _def_register_success;
	//6.给客户端回复注册结果
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}
//处理登录请求
void CKernel::dealLoginRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1.拆包
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	cout << rq->tel << "  " << rq->password;
	//2.从数据库查询，根据电话号码查询密码
	list<string>lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select passw, id from t_use where tel = '%s';", rq->tel);//将字符串打印到指定空间内
	//3.判断查询结果是否为空
	_STRU_LOGIN_RS rs;//定义局部变量，把局部变量的数据拷贝发出去，而不是定义指针把空间发出去
	if (!m_sql.SelectMySql(sql, 2, lstStr))
	{
		cout << "查询数据库失败" << sql << endl;
		return;
	}
	if (0 == lstStr.size())
	{
		//4.查询结果为空，说明电话号码未注册，登录失败
		rs.result = _def_login_tel_not_exists;
	}
	else
	{
		//5.比较查询到的密码和输入的密码
		string pass = lstStr.front();
		lstStr.pop_front();

		int userId = stoi(lstStr.front());//字符串转int
		lstStr.pop_front();
		if (pass == rq->password)//string类型使用类重载操作符比较，如果是char*则用strcmp()进行比较
		{
			//相等，判断是否在线
			if (m_mapIdToSocket.count(userId) > 0) {
				//该用户已在线，登录失败
				rs.result = _def_status_online;
			}
			else
			{
				//不在线，登陆成功
				rs.result = _def_login_success;
				rs.userId = userId;
				//保存当前用户的socket
				m_mapIdToSocket[userId] = from;
				//把结果发给客户端
				m_pMediator->sendData((char*)&rs, sizeof(rs), from);//先发loginrs
				//获取当前登录用户的信息以及好友的信息
				getUserInfoAndFriendInfo(userId);
				return;
			}
		}
		else
		{
			//不相等，登录失败，密码错误
			rs.result = _def_login_passwd_error;
		}
	}
	//把结果发给客户端
	m_pMediator->sendData((char*)&rs, sizeof(rs), from);
}
//获取当前登录用户的信息以及好友的信息
//应用层最大传输数据64K
void CKernel::getUserInfoAndFriendInfo(int userId)
{
	cout << __func__ << endl;
	//根据自己的id查询自己的信息
	_STRU_FRIEND_INFO userInfo;
	getInfoById(userId, &userInfo);
	//把自己的信息发给客户端
	if (m_mapIdToSocket.count(userId) > 0)//纯C++的map在查找前先判断是否有value
	{
		m_pMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);//再发info
	}
	else
	{
		cout << "m_mapIdToSocket中没有id:" << userId << endl;
		return;
	}
	//根据自己的id查询好友的id列表
	list<string>lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select idB from t_friend where idA = %d;", userId);//将字符串打印到指定空间内
	//3.判断查询结果是否为空
	_STRU_LOGIN_RS rs;//定义局部变量，把局部变量的数据拷贝发出去，而不是定义指针把空间发出去
	if (!m_sql.SelectMySql(sql, 1, lstStr))
	{
		cout << "查询数据库失败" << sql << endl;
		return;
	}
	//遍历好友的id
	int friendId = 0;
	_STRU_FRIEND_INFO friendinfo;
	while (lstStr.size() > 0)
	{
		//取出好友的id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//根据好友的id查询好友的信息
		getInfoById(friendId, &friendinfo);
		//在把好友的信息发给客户端
		if (m_mapIdToSocket.count(userId) > 0)
		{
			m_pMediator->sendData((char*)&friendinfo, sizeof(friendinfo), m_mapIdToSocket[userId]);
		}
		else
		{
			cout << "m_mapIdToSocket中没有id:" << userId << endl;
			return;
		}
		//判断好友是否在线，
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			//如果在线就通知好友自己上线了
			m_pMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);//把自己（后上线）的信息发给好友（先上线）
		}
	}
}
//根据id查询用户信息
void CKernel::getInfoById(int id, _STRU_FRIEND_INFO* info/*查询到的用户信息作为输出参数返回给调用函数*/)
{
	cout << __func__ << endl;
	info->id = id;
	if (m_mapIdToSocket.count(id) > 0)
	{
		//在线
		info->status = _def_status_online;
	}
	else
	{
		info->status = _def_status_offline;
	}
	//从数据库查询昵称、签名和头像id
	list<string>lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select tel,name,feeling,iconid from t_use where id = %d;", id);//将字符串打印到指定空间内
	//3.判断查询结果是否为空
	_STRU_LOGIN_RS rs;//定义局部变量，把局部变量的数据拷贝发出去，而不是定义指针把空间发出去
	if (!m_sql.SelectMySql(sql, 4, lstStr))//查3列
	{
		cout << "查询数据库失败" << sql << endl;
		return;
	}
	if (4 == lstStr.size())
	{
		//取出手机号
		strcpy_s(info->tel, sizeof(info->tel), lstStr.front().c_str());
		lstStr.pop_front();
		//取出昵称
		strcpy_s(info->name, sizeof(info->name), lstStr.front().c_str());
		lstStr.pop_front();
		//取出签名
		strcpy_s(info->feeling, sizeof(info->feeling), lstStr.front().c_str());
		lstStr.pop_front();
		//取出头像id
		info->iconid = stoi(lstStr.front());
		lstStr.pop_front();
	}
	else
	{
		cout << "查询昵称，签名和头像id错误:" << sql << endl;
	}
}
//处理聊天请求函数
void CKernel::dealChatRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;
	//2.判断好友是否在线
	if (m_mapIdToSocket.count(rq->friendId) > 0)
	{
		//如果好友在线，把聊天请求转发给好友
		m_pMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);//转发数据，不用转rq
	}
	else
	{
		//好友不在线，给客户端回复发送失败
		//正常软件，应该不给不在线好友发的消息存入数据库，等到好友上线的时候，
		//再把数据库里面的聊天内容转发给好友，把数据库保存的数据删掉
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}
//处理下线请求函数
void CKernel::dealOfflineRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;
	//2.查询好友id列表
	list<string>lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select idB from t_friend where idA = %d;", rq->userId);//将字符串打印到指定空间内
	if (!m_sql.SelectMySql(sql, 1, lstStr))
	{
		cout << "查询数据库失败" << sql << endl;
		return;
	}
	//3.遍历好友id列表
	int friendId = 0;
	while (lstStr.size() > 0)
	{
		//4.取出好友id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//5.判断好友是否在线，在线就转发下线请求给好友
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);//转发
		}
	}
	//6.关闭下线用户的socket，并且回收map空间
	auto ite = m_mapIdToSocket.find(rq->userId);//find返回一个指针，auto匹配任何类型
	if (ite != m_mapIdToSocket.end())//不为空则找到
	{
		closesocket(ite->second);
		m_mapIdToSocket.erase(ite);
	}
}
//处理添加好友请求
void CKernel::dealAddFriendRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;
	//2.根据好友手机号查询好友id
	list<string>lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "select id,name from t_use where tel = '%s';", rq->friendTel);//将字符串打印到指定空间内
	//3.判断查询结果是否为空
	if (!m_sql.SelectMySql(sql, 2, lstStr))
	{
		cout << "查询数据库失败" << sql << endl;
		return;
	}
	//3.判断查询结果是否为空
	if (0 == lstStr.size())
	{
		//4.查询结果为空，说明昵称不存在，添加失败_def_add_friend_not_exists
		_STRU_ADD_FRIEND_RS rs;
		rs.result = _def_add_friend_not_exists;
		strcpy_s(rs.friendTel, sizeof(rs.friendTel), rq->friendTel);
		//把添加结果发回给A客户端
		m_pMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else
	{
		//取出好友id
		int friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//5.判断好友是否在线
		if (m_mapIdToSocket.count(friendId) > 0)
		{
			//6.好友在线，把添加好友的请求转发给好友B客户端
			m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else
		{
			//7.好友不在线，添加失败_def_add_friend_offline
			_STRU_ADD_FRIEND_RS rs;
			rs.result = _def_add_friend_offline;
			//取出昵称
			strcpy_s(rs.friendName, sizeof(rs.friendName), lstStr.front().c_str());
			lstStr.pop_front();
			//把添加结果发回给A客户端
			m_pMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}
}
//处理添加好友回复
void CKernel::dealAddFriendRs(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;
	//2.判断是否同意
	if (_def_add_friend_success == rs->result)
	{
		//3.如果同意，把好友关系写入数据库（双向存储需要写两次）
		char sql[1024] = "";
		sprintf_s(sql, "insert into t_friend values(%d,%d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sql))
		{
			cout << "查询数据库失败" << sql << endl;
			return;
		}
		sprintf_s(sql, "insert into t_friend values(%d,%d);", rs->friendId, rs->userId);
		if (!m_sql.UpdateMySql(sql))
		{
			cout << "查询数据库失败" << sql << endl;
			return;
		}
		//4.更新双端好友列表
		getUserInfoAndFriendInfo(rs->friendId);
	}
	//5.无论结果如何，都要把结果转发给A客户端
	if (m_mapIdToSocket.count(rs->userId) > 0)
	{
		m_pMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
	}
}

//处理用户信息
void CKernel::dealUserInfo(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_FRIEND_INFO* userInfo = (_STRU_FRIEND_INFO*)data;
	list<string>lstStr;
	char sql[1024] = "";
	sprintf_s(sql, "update t_use set iconid = %d where id = %d;",userInfo->iconid, userInfo->id);
	if (!m_sql.UpdateMySql(sql))
	{
		//查询数据库失败原因只有两个
		//1.没有连接数据库
		//2.sql语句有语法错误或者列名和表对应不上(把日志中打印的sql语句拷贝到workbench里面运行)
		cout << "查询数据库失败" << sql << endl;
	}
	//根据自己的id查询好友的id列表
	sprintf_s(sql, "select idB from t_friend where idA = %d;", userInfo->id);//将字符串打印到指定空间内
	if (!m_sql.SelectMySql(sql, 1, lstStr))
	{
		cout << "查询数据库失败" << sql << endl;
		return;
	}
	//遍历好友的id
	int friendId = 0;
	while (lstStr.size() > 0)
	{
		//取出好友的id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		m_pMediator->sendData(data, len, m_mapIdToSocket[friendId]);
	}
}