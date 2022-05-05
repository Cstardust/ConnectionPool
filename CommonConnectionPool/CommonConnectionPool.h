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
* �̳߳�
* ����ģʽ
*/
class CommonConnectionPool {
public:
	//  static������Ϊ���ڻ�û�ж���ʱ�Ϳɵ���
	static CommonConnectionPool* getCommonConnectionPool();
	bool loadConfigFile();


	CommonConnectionPool(const CommonConnectionPool&) = delete;
	CommonConnectionPool& operator=(const CommonConnectionPool&) = delete;
private:
	CommonConnectionPool();		//  ���캯��˽��
	string _ip;					//  mysql server ip
	unsigned  _port;			//  mysql server port
	string _name;				//  mysql server ��¼�û���
	string _password;			//  mysql server ��¼����
	string _dbname;				//  Ҫ���ʵ����ݿ�����
	int _initSize;				//  ���ӳصĳ�ʼ������
	int _maxSize;				//  ���ӳ����������
	int _maxIdleTime;			//  ���ӳ�������ʱ��
	int _connectionTimeout;		//  ���ӳػ�ȡ���ӵĳ�ʱʱ��
	int _maxConnectionTimeout;	//  ���ӳ�ʱʱ�� ��λms
	queue<Connection*> _connectionQueue;	//  �洢mysql���ӵĶ���
	mutex _queueMutex;						//  ά�����Ӷ��е��̰߳�ȫ������
	volatile atomic_int cntOfconns;			//  ������Ŀ
};


#endif // !_COMMONCONNECTIONPOOL_H_
