
#include"CommonConnectionPool.h"

atomic_int reuse_cnt = 0;
atomic_int produce_cnt = 0;

CommonConnectionPool* CommonConnectionPool::getCommonConnectionPool()
{
	static CommonConnectionPool pool;
	return &pool;
}

bool CommonConnectionPool::loadConfigFile()
{
	//  以下应该可以用c++的文件流替换。整体写完再改
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {
		LOG("mysql.ini file is not existed");
		return false;
	}
	while (!feof(pf))
	{
		char line[512];
		if (fgets(line, 512, pf) == nullptr) {
			break;
		}
		string str(line);
		int idx = str.find('=');
		if (idx == -1) continue;
		// username=root
		int end_idx = str.find('\n',idx);	//  find(char,pos)。从pos开始招char
		if (end_idx == -1) break;
		string key(str.substr(0, idx));		//  substr(pos,len)。从pos开始，截取长度为len的字符串
		string val(str.substr(idx + 1, end_idx - 1 - (idx + 1) + 1));
		//  以下是否可以用bind function替换？ 待研究
		if (key == "ip") {
			_ip = val;
		}
		else if (key == "port") {
			_port = atoi(val.c_str());
		}
		else if (key == "username") {
			_name = val;
		}
		else if (key == "password") {
			_password = val;
		}
		else if (key == "dbname") {
			_dbname = val;
		}
		else if (key == "initSize") {
			_initSize = atoi(val.c_str());
		}
		else if (key == "maxSize") {
			_maxSize = atoi(val.c_str());
		}
		else if (key == "maxIdleTime") {
			_maxIdleTime = atoi(val.c_str());
		}
		else if (key == "maxConnectionTimeout") {
			_maxConnectionTimeout = atoi(val.c_str());
		}
	}
	return true;
}

CommonConnectionPool::CommonConnectionPool()
{
	//  加载配置
	if (!loadConfigFile()) {
		return;		//   throw吧
	}
	//  创建初始数量的连接
	for (int i = 0; i < _initSize; ++i) {
		Connection* p = new Connection();
		p->connect(_ip, _port,_name, _password, _dbname);
		_connectionQueue.push(p);	//  空闲连接加入队列
		p->refreshAliveTime();		//  更新时间
		++_cntOfconns;
	}

	//  启动一个新的线程，作为连接的生产者。
	//  因为是成员函数，所以要传一个this
	thread produce(&CommonConnectionPool::produceConnectionTask, this);
	produce.detach();	//  线程分离 
	thread scanner(&CommonConnectionPool::scannConnectionTask, this);
	scanner.detach();	//  线程分离
}

//  生产者-消费者模型 
//  生产Connection。在队列不为空时不占cpu
void CommonConnectionPool::produceConnectionTask()
{
	while (true)
	{
		//  上锁
		unique_lock<mutex> uni_lck(_queueMtx);
		//  为空时再生产 其余时候阻塞 不占cpu
		while (!_connectionQueue.empty()) {
			_cv.wait(uni_lck);			//  1.释放锁 2.阻塞等待条件变量 3.条件变量符合后被唤醒 再次lock
		}
		if (_cntOfconns < _maxSize) {
			//  生产
			Connection* pc = new Connection();
			if (!pc->connect(_ip, _port, _name, _password, _dbname)) {
				LOG("MySQL connection failed");
				continue;
			}
			++_cntOfconns;
			_connectionQueue.push(pc);
			pc->refreshAliveTime();
		}
		cout<<"produce_cnt "<<produce_cnt++<<endl;
		//  唤醒其他阻塞在条件变量上的线程（消费者线程）
		_cv.notify_all();
	}
}

//  消费者线程
//  给外部提供接口，从连接池中获取一个可用的空闲连接
shared_ptr<Connection> CommonConnectionPool::getConnection()
{
	//  上锁
	unique_lock<mutex> uni_lck(_queueMtx);
	//  等待
	while (_connectionQueue.empty()) {	//  有可能两个消费者同时在等待条件变量，然后生产者生产了一个。有一个消费者先拿到，另一个没拿到。但是这种情况另一个虽然没拿到，可是生产者仍有能力再生产。所以要while循环。
		//  为空、通知生产
		 _cv.notify_all();
		//  等待_connectionTimeout ms
		cv_status st = _cv.wait_for(uni_lck,chrono::milliseconds(_connectionTimeout));
		if (st == cv_status::timeout && _connectionQueue.empty()) {		//  超时且队列为空，则无法再获取新连接
			LOG("获取空闲连接超时，获取连接失败！");
			return nullptr;
		}
	}

	//  消费 返回给用户一个封装好的Connection智能指针。
	//  shared_ptr智能指针析构时，connection就被close掉了
	//  我们需要重定义shared_ptr释放资源的方式 使用lambda表达式
	shared_ptr<Connection> sp(_connectionQueue.front(), [&](Connection *pcon)->void {
		cout<<"reuse_cnt "<<++reuse_cnt << endl;
		//  cout << "connection放回pool" << endl;
			lock_guard<mutex> guard(_queueMtx);		//  线程安全
			_connectionQueue.push(pcon);
			pcon->refreshAliveTime();
		});
	_connectionQueue.pop();

	//  队列为空，则通知生产
	if (_connectionQueue.empty()) {
		_cv.notify_all();
	}

	//  返回
	return sp;
}


void CommonConnectionPool::scannConnectionTask()
{
	while (true)
	{
		//  定时检查
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));	//  60s
		//  扫描队列 释放资源
		lock_guard<mutex> guard(_queueMtx);
		while (!(_connectionQueue.empty()) && _connectionQueue.size()>_initSize) {
			Connection* pt = _connectionQueue.front();
			if (pt->getAliveTime() <= _maxIdleTime*1000)	//  靠前的没过期 那么靠后的也一定没过期
			{
				break;
			}
			_connectionQueue.pop();	//  弹出
			--_cntOfconns;			//  队列数量--
			delete pt;				//  关闭连接
			LOG("delete");
		}
	}
}



/*
* 线程分离
* 线程分离状态：指定该状态，线程主动与主控线程断开关系。线程结束后，其退出状态不由其他线程获取，而
直接自己自动释放。网络、多线程服务器常用。
进程若有该机制，将不会产生僵尸进程。僵尸进程的产生主要由于进程死后，大部分资源被释放，一点残留资
源仍存于系统中，导致内核认为该进程仍存在
*/