#include <mysql.h>
#include <string>
#include<iostream>
#include "public.h"

using namespace std;

// 数据库操作类。
//  Connection封装了MySQL第三方库提供的原生接口.
class Connection
{
public:
	// 初始化数据库连接
	Connection();
	// 释放数据库连接资源
	~Connection();
	// 连接数据库
	bool connect(string ip,
		unsigned short port, 
		string user, 
		string password,
		string dbname);
	// 更新操作 insert、delete、update
	bool update(string sql);
	// 查询操作 select
	MYSQL_RES* query(string sql);
private:
	MYSQL* _conn; // 表示和MySQL Server的一条连接
};


//  mysql.h 里都是函数和结构体、变量的声明
//  函数的实现我们见不到，因为被封在动态库里了。