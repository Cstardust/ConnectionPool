#include"Connection.h"
#include"CommonConnectionPool.h"
#include<string.h>

void work()
{
	const int maxn = 10000;
	for (int i = 0; i < maxn / 4; ++i)
	{
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 20, "female");
		conn.connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
		conn.update(sql);
	}
}



int main()
{
	//  CommonConnectionPool* p = CommonConnectionPool::getCommonConnectionPool();
	/*
	//  添加测试MySQL第三方库接口
	Connection conn;
	char sql[1024];
	sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", 
		"zhang san", 20, "female");
	conn.connect("127.0.0.1", 13306, "root", "C361456shc","chat");
	conn.update(sql);
	*/
#if 1
# if 0
	clock_t bg = clock();
	for (int i = 0; i < 10000; ++i)
	{
		Connection conn;
		char sql[1024]={0};
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 20, "female");
		conn.connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
		conn.update(sql);
	}
	clock_t ed = clock();
	cout << (ed - bg) << "ms" << endl;

# else

	clock_t bg = clock();
	//  加载配置文件、创建initSize连接、开启生产者线程、开启扫描线程
	CommonConnectionPool *p = CommonConnectionPool::getCommonConnectionPool();	
	for (int i = 0; i < 10000; ++i)
	{
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"zhang san", 20, "female");
		//  从连接池pool中获取连接connection
		shared_ptr<Connection> sp = p->getConnection();
		//  对connection进行操作
		sp->connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
		sp->update(sql);
		//  一轮结束之后，sp就会调用删除器，把这个Connection*放回pool的queue
	}
	clock_t ed = clock();
	cout << (ed - bg) << "ms" << endl;
	
#endif	

#else
#if 0
	 Connection conn;
	 conn.connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
	clock_t bg = clock();
	thread t1(work);
	thread t2(work);
	thread t3(work);
	thread t4(work);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t ed = clock();
	std::cout << (ed - bg) << "ms" << std::endl;
#else
	clock_t bg = clock();

	CommonConnectionPool* p = CommonConnectionPool::getCommonConnectionPool();
	const int maxn = 10000;
	thread t1([&]() {
		for (int i = 0; i < maxn / 4; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "female");
			shared_ptr<Connection> sp = p->getConnection();
			sp->connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
			sp->update(sql);
		}
		});

	thread t2([&]() {
		for (int i = 0; i < maxn / 4; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "female");
			shared_ptr<Connection> sp = p->getConnection();
			sp->connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
			sp->update(sql);
		}
		});

	thread t3([&]() {
		for (int i = 0; i < maxn / 4; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "female");
			shared_ptr<Connection> sp = p->getConnection();
			sp->connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
			sp->update(sql);
		}
		});

	thread t4([&]() {
		for (int i = 0; i < maxn / 4; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "female");
			shared_ptr<Connection> sp = p->getConnection();
			sp->connect("127.0.0.1", 13306, "root", "C361456shc", "chat");
			sp->update(sql);
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	clock_t ed = clock();
	cout << (ed - bg) << "ms" << endl;
#endif
	
#endif



}