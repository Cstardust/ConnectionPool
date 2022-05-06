#ifndef _COMMONCONNECTIONPOOL_H_
#define _COMMONCONNECTIONPOOL_H_

#include"public.h"
#include"Connection.h"
#include<string>
#include<queue>
#include<mutex>
#include<atomic>
#include<functional>
#include<condition_variable>
#include<memory>

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
	//  �������߳�
	//  ���ⲿ�ṩ�ӿڣ������ӳ��ػ�ȡһ�����õĿ������� 
	//  ������Ҫ�ض���shared_ptr��������
	shared_ptr<Connection> getConnection();

	CommonConnectionPool(const CommonConnectionPool&) = delete;
	CommonConnectionPool& operator=(const CommonConnectionPool&) = delete;
	~CommonConnectionPool() {	//  �ͷ���Դ
		while (!_connectionQueue.empty()) {
			Connection * p = _connectionQueue.front();
			delete p;
			_connectionQueue.pop();
		}
	}
private:
	//  ���캯��˽��
	CommonConnectionPool();		
	//  ���������ļ�
	bool loadConfigFile();		
	//  �������߳�
	//  �����ڶ������߳��У�ר�Ÿ�������������
	//  ����Ϊ��Ա��������ȫ�ֺ����ĺô������Է���ķ������ӳ��еĳ�Ա
	void produceConnectionTask();	

	//  ��ʱ����߳�
	//  ��ʱ��Ⲣ�ͷſ�������
	void scannConnectionTask();

	//  mysql��Ϣ
	string _ip;					//  mysql server ip
	unsigned  _port;			//  mysql server port
	string _name;				//  mysql server ��¼�û���
	string _password;			//  mysql server ��¼����
	string _dbname;				//  Ҫ���ʵ����ݿ�����
	//  �������ӳ�
	int _initSize;				//  ���ӳصĳ�ʼ������    
	int _maxSize;				//  ���ӳؿ������ӵ����������	       1024��  queue�����ֻ����1024���������ӡ�
	int _maxIdleTime;			//  ���ӳ��п������ӵ�������ʱ��	   60s  (��queue������ʱ�䣩�������ͷš�scan�߳����õ�����ѹ����벻�ᴥ����
	int _connectionTimeout;		//  �û������ӳػ�ȡ���ӵĳ�ʱʱ��     100ms  ��ʱ���������ȡ���ӡ���getConnection�߳����õ�����ѹ����벻�ᱻ������	

	//  ���ӳ��Լ��ĳ�Ա
	queue<Connection*> _connectionQueue;	//  �洢mysql���ӵĶ���
	volatile atomic_int _cntOfconns;	    //  ������Ŀ
	condition_variable _cv;					//  �������� ��Ӧ��������������ģ�ͣ�
	mutex _queueMtx;						//  ��
};


#endif // !_COMMONCONNECTIONPOOL_H_
