#include <string>
using namespace std;

#include "stdafx.h"



class CDBOperation
{
public:
	//��ʼ�����ݿ������Ҫ�Ķ���
	CDBOperation(void);
	~CDBOperation(void);

	//���������ݿ�
	bool			ConnToDB();
	bool			ConnToDB(const char *ConnectionString,const char *UserID,const char *Password);

	void			BeginTans();	//��������
	void			Rollback();		//�ع�����
	void			Commit();		//�ύ����

	//���ݿ��������
	//��ѯ���� ɾ���Լ����

	_RecordsetPtr ExecuteWithResSQL(const char *);
	string ExecuteWithResSQL(const char *sql,int colno );
	bool ExecuteWithNoSQL(const char *sql);

	//bool ExecuteNoResSQL(const char *);//delete and add

	string			GetUser();
	string			GetPassword();
	string			GetConnectString();
	string			GetTransIDFromDB();
	
private:
	//��ӡ������Ϣ
	void			PrintErrorInfo(_com_error &);

	//��ʼ�����ݿ����ӡ������¼��
	_ConnectionPtr	CreateConnPtr();
	_CommandPtr		CreateCommPtr();
	_RecordsetPtr	CreateRecsetPtr();
private:

	//���ݿ����Ӵ��Լ��û���������
	string	m_sConnectStr;
	string	m_sUser;
	string	m_sPassword;

	//���ݿ�������Ҫ�����ӡ������������
	_ConnectionPtr	m_pConnection;
	_CommandPtr		m_pCommand;
};
  