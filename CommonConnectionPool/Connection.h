#include <mysql.h>
#include <string>
#include<iostream>
#include "public.h"

using namespace std;

// ���ݿ�����ࡣ
//  Connection��װ��MySQL���������ṩ��ԭ���ӿ�.
class Connection
{
public:
	// ��ʼ�����ݿ�����
	Connection();
	// �ͷ����ݿ�������Դ
	~Connection();
	// �������ݿ�
	bool connect(string ip,
		unsigned short port, 
		string user, 
		string password,
		string dbname);
	// ���²��� insert��delete��update
	bool update(string sql);
	// ��ѯ���� select
	MYSQL_RES* query(string sql);
private:
	MYSQL* _conn; // ��ʾ��MySQL Server��һ������
};


//  mysql.h �ﶼ�Ǻ����ͽṹ�塢����������
//  ������ʵ�����Ǽ���������Ϊ�����ڶ�̬�����ˡ�