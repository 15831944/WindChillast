#include <string>
using namespace std;

#include "stdafx.h"



class CDBOperation
{
public:
	//初始化数据库操作需要的对象
	CDBOperation(void);
	~CDBOperation(void);

	//连接至数据库
	bool			ConnToDB();
	bool			ConnToDB(const char *ConnectionString,const char *UserID,const char *Password);

	void			BeginTans();	//开启事务
	void			Rollback();		//回滚事务
	void			Commit();		//提交事务

	//数据库操作函数
	//查询操作 删除以及添加

	_RecordsetPtr ExecuteWithResSQL(const char *);
	string ExecuteWithResSQL(const char *sql,int colno );
	bool ExecuteWithNoSQL(const char *sql);

	//bool ExecuteNoResSQL(const char *);//delete and add

	string			GetUser();
	string			GetPassword();
	string			GetConnectString();
	string			GetTransIDFromDB();
	
private:
	//打印错误信息
	void			PrintErrorInfo(_com_error &);

	//初始化数据库连接、命令、记录集
	_ConnectionPtr	CreateConnPtr();
	_CommandPtr		CreateCommPtr();
	_RecordsetPtr	CreateRecsetPtr();
private:

	//数据库连接串以及用户名、密码
	string	m_sConnectStr;
	string	m_sUser;
	string	m_sPassword;

	//数据库连接需要的连接、命令操作对象
	_ConnectionPtr	m_pConnection;
	_CommandPtr		m_pCommand;
};
  