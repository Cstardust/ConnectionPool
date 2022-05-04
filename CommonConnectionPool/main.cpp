#include"Connection.h"
#include<string.h>

int main()
{
	//  添加测试MySQL第三方库接口
	Connection conn;
	char sql[1024];
	sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", 
		"zhang san", 20, "female");
	conn.connect("127.0.0.1", 13306, "root", "C361456shc","chat");
	conn.update(sql);
}