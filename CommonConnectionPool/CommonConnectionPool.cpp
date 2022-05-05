

#include"public.h"
#include"CommonConnectionPool.h"


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
		_connectionQueue.push(p);
		++cntOfconns;
	}
	//  启动一个新的线程，作为连接的生产者。

}