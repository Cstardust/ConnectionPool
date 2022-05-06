#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <mysql.h>
#include <string>
#include<iostream>
#include "public.h"

using namespace std;

// 数据库操作类。负责与MySQL数据库进行交互。如连接Server，以及操作数据库文件。
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
	//  更新起始时间 ms为单位
	void refreshAliveTime() { _alivetime = clock(); }
	clock_t getAliveTime() {
		return clock() -_alivetime;
	}
private:
	MYSQL* _conn;			//  表示和MySQL Server的一条连接
	clock_t _alivetime;		//  记录进入空闲状态后的起始时间
};


//  mysql.h 里都是函数和结构体、变量的声明
//  函数的实现我们见不到，因为被封在动态库里了。

#endif