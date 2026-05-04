#pragma once
//加载库的版本号(定义宏加括号：为了替换运算)
#define _DEF_VERSION_LOW                   (2)
#define _DEF_VERSION_HIGH                  (2)
//UDP协议端口号
#define _DEF_UDP_PORT                      (12345)
//TCP协议端口号
#define _DEF_TCP_PORT                      (56789)
//TCP协议监听队列的最大长度
#define _DEF_TCP_LISTEN_MAX                (100)
//昵称、手机号、密码的最大长度
#define _DEF_MAX_LENGTH                    (20)
//聊天内容长度
#define _DEF_CONTENT_LENGTH                (8*1024)//8K
//结构体宏的个数
#define _DEF_PROTOCOL_COUNT                (15)
//签名的长度
#define _DEF_MAX_FEELING                   (100)
//文件路径最大长度
#define _DEF_MAX_FILE_PATH                 (512)
//文件内容大小
#define _DEF_FILE_CONTENT_SIZE             (8 * 1024)
//声明结构体类型宏——定义协议头，在dealData中知道来自哪个结构体
#define _DEF_PROTOCOL_BASE                 (1000)
//注册请求
#define _DEF_REGISTER_RQ                   (_DEF_PROTOCOL_BASE + 1)
//注册回复
#define _DEF_REGISTER_RS                   (_DEF_PROTOCOL_BASE + 2)
//登录请求
#define _DEF_LOGIN_RQ                      (_DEF_PROTOCOL_BASE + 3)
//登录回复
#define _DEF_LOGIN_RS                      (_DEF_PROTOCOL_BASE + 4)
//添加好友请求
#define _DEF_ADD_FRIEND_RQ                 (_DEF_PROTOCOL_BASE + 5)
//添加好友回复
#define _DEF_ADD_FRIEND_RS                 (_DEF_PROTOCOL_BASE + 6)
//聊天请求
#define _DEF_CHAT_RQ                       (_DEF_PROTOCOL_BASE + 7)
//聊天回复
#define _DEF_CHAT_RS                       (_DEF_PROTOCOL_BASE + 8)
//下线请求
#define _DEF_OFFLINE_RQ                    (_DEF_PROTOCOL_BASE + 9)
//用户信息
#define _DEF_FRIEND_INFO                   (_DEF_PROTOCOL_BASE + 10)

//声明结果宏
//注册结果
#define _def_register_success              (0)
#define _def_register_tel_exists           (1)
#define _def_register_name_exists          (2)
//登录结果
#define _def_login_success                 (0)
#define _def_login_tel_not_exists          (1)
#define _def_login_passwd_error            (2)
//添加好友的结果
#define _def_add_friend_success            (0)
#define _def_add_friend_offline            (1)
#define _def_add_friend_refuse             (2)
#define _def_add_friend_not_exists         (3)
//发送结果
#define _def_send_success                  (0)
#define _def_send_fail                     (1)
//用户状态
#define _def_status_online                 (3)
#define _def_status_offline                (4)

//声明结构体类型变量
typedef int packtype;
//请求结构体
//注册请求
typedef struct _STRU_REGISTER_RQ
{
	_STRU_REGISTER_RQ() :type(_DEF_REGISTER_RQ)
	{
		memset(tel, 0, _DEF_MAX_LENGTH);
		memset(password, 0, _DEF_MAX_LENGTH);
		memset(name, 0, _DEF_MAX_LENGTH);
	}
	packtype type;
	char tel[_DEF_MAX_LENGTH];
	char password[_DEF_MAX_LENGTH];
	char name[_DEF_MAX_LENGTH];
}_STRU_REGISTER_RQ;
//注册回复：结果（成功，电话号码已被注册，昵称已被注册）
typedef struct _STRU_REGISTER_RS
{
	_STRU_REGISTER_RS() :type(_DEF_REGISTER_RS), result(_def_register_tel_exists)
	{}
	packtype type;
	int result;
}_STRU_REGISTER_RS;
//登录请求
typedef struct _STRU_LOGIN_RQ
{
	_STRU_LOGIN_RQ() :type(_DEF_LOGIN_RQ)
	{
		memset(tel, 0, _DEF_MAX_LENGTH);
		memset(password, 0, _DEF_MAX_LENGTH);
	}
	packtype type;
	char tel[_DEF_MAX_LENGTH];
	char password[_DEF_MAX_LENGTH];
}_STRU_LOGIN_RQ;
//登录回复:结果（登录成功，电话号码不存在，密码错误）
typedef struct _STRU_LOGIN_RS
{
	_STRU_LOGIN_RS() :type(_DEF_LOGIN_RS), result(_def_login_tel_not_exists), userId(0)
	{}
	packtype type;
	int result;
	int userId;
}_STRU_LOGIN_RS;
//添加好友请求:好友昵称，自己的id，自己的昵称
typedef struct _STRU_ADD_FRIEND_RQ
{
	_STRU_ADD_FRIEND_RQ() :type(_DEF_ADD_FRIEND_RQ), userId(0)
	{
		memset(userName, 0, _DEF_MAX_LENGTH);
		memset(friendTel, 0, _DEF_MAX_LENGTH);
		memset(friendName, 0, _DEF_MAX_LENGTH);
	}
	packtype type;
	int userId;
	char userName[_DEF_MAX_LENGTH];
	char friendTel[_DEF_MAX_LENGTH];
	char friendName[_DEF_MAX_LENGTH];
}_STRU_ADD_FRIEND_RQ;
//添加好友回复：结果（添加成功，好友不存在，好友拒绝，好友不在线）,自己的ID，自己的昵称，好友的ID
typedef struct _STRU_ADD_FRIEND_RS
{
	_STRU_ADD_FRIEND_RS() :type(_DEF_ADD_FRIEND_RS), result(_def_add_friend_not_exists), userId(0), friendId(0)
	{
		memset(userName, 0, _DEF_MAX_LENGTH);
		memset(friendName, 0, _DEF_MAX_LENGTH);
		memset(friendTel, 0, _DEF_MAX_LENGTH);
	}
	packtype type;
	int result;
	int userId;
	int friendId;
	char userName[_DEF_MAX_LENGTH];
	char friendName[_DEF_MAX_LENGTH];
	char friendTel[_DEF_MAX_LENGTH];
}_STRU_ADD_FRIEND_RS;
//聊天请求:聊天内容，自己的ID，好友的ID
typedef struct _STRU_CHAT_RQ
{
	//发送缓冲区默认64K
	_STRU_CHAT_RQ() :type(_DEF_CHAT_RQ), userId(0), friendId(0)
	{
		memset(content, 0, _DEF_CONTENT_LENGTH);
	}
	packtype type;
	char content[_DEF_CONTENT_LENGTH];
	int userId;
	int friendId;
}_STRU_CHAT_RQ;
//聊天回复：结果（成功，失败）
typedef struct _STRU_CHAT_RS
{
	_STRU_CHAT_RS() :type(_DEF_CHAT_RS), result(_def_send_fail)
	{}
	packtype type;
	int friendId;
	int result;
}_STRU_CHAT_RS;
//下线请求:自己的Id
typedef struct _STRU_OFFLINE_RQ
{
	_STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ), userId(0)
	{}
	packtype type;
	int userId;
}_STRU_OFFLINE_RQ;
//用户信息：手机号，type,id，昵称，签名，头像id，状态
typedef struct _STRU_FRIEND_INFO
{
	_STRU_FRIEND_INFO() :type(_DEF_FRIEND_INFO), id(0), iconid(0), status(_def_status_offline)
	{
		memset(name, 0, _DEF_MAX_LENGTH);
		memset(feeling, 0, _DEF_MAX_LENGTH);
		memset(tel, 0, _DEF_MAX_LENGTH);
	}
	packtype type;
	int id;
	int iconid;
	int status;
	char name[_DEF_MAX_LENGTH];
	char feeling[_DEF_MAX_FEELING];
	char tel[_DEF_MAX_LENGTH];
}_STRU_FRIEND_INFO;

