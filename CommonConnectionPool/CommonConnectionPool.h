#ifndef _COMMONCONNECTIONPOOL_H_
#define _COMMONCONNECTIONPOOL_H_

#include"public.h"
#include"Connection.h"
#include<string>
#include<queue>
#include<mutex>
#include<atomic>

using std::atomic_int;
using std::string;
using std::queue;
using std::mutex;

/*
* 线程池
* 单例模式
*/
class CommonConnectionPool {
public:
	//  static函数：为了在还没有对象时就可调用
	static CommonConnectionPool* getCommonConnectionPool();
	bool loadConfigFile();


	CommonConnectionPool(const CommonConnectionPool&) = delete;
	CommonConnectionPool& operator=(const CommonConnectionPool&) = delete;
private:
	CommonConnectionPool();		//  构造函数私有
	string _ip;					//  mysql server ip
	unsigned  _port;			//  mysql server port
	string _name;				//  mysql server 登录用户名
	string _password;			//  mysql server 登录密码
	string _dbname;				//  要访问的数据库名称
	int _initSize;				//  连接池的初始连接量
	int _maxSize;				//  连接池最大连接量
	int _maxIdleTime;			//  连接池最大空闲时间
	int _connectionTimeout;		//  连接池获取连接的超时时间
	int _maxConnectionTimeout;	//  连接超时时间 单位ms
	queue<Connection*> _connectionQueue;	//  存储mysql连接的队列
	mutex _queueMutex;						//  维护连接队列的线程安全互斥锁
	volatile atomic_int cntOfconns;			//  连接数目
};


#endif // !_COMMONCONNECTIONPOOL_H_
