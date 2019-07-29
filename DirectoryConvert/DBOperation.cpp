#include "stdafx.h"
#include "DBOperation.h"
//#include "common.h"
#include <Windows.h>
#include <assert.h>
#include <direct.h>
#include "WindChillSetting.h"


CDBOperation::CDBOperation(void)
{

	CoInitialize(NULL);
	m_pConnection = CreateConnPtr();
	m_pCommand = CreateCommPtr();
	m_sConnectStr = "";
}

CDBOperation::~CDBOperation(void)
{
	//m_pCommand->Close();
	if (m_pConnection != NULL)
	{
		m_pConnection->Close();
		m_pConnection = NULL;
	}
	CoUninitialize();
}



bool CDBOperation::ConnToDB(const char *ConnectionString,const char *UserID,const char *Password)
{
	if (NULL == m_pConnection)
	{
		printf("Failed to create connection\n");
		m_pConnection = NULL;
		return false;
	}

	if (strlen(ConnectionString) == 0 
		|| strlen(UserID) == 0
		|| strlen(Password) == 0)
	{
		m_pConnection = NULL;
		return false;
	}

	try
	{
		HRESULT hr = m_pConnection->Open(ConnectionString, UserID, Password, adModeUnknown);
		if (TRUE == FAILED(hr))
		{
			m_pConnection = NULL;
			return false;
		}
		m_pCommand->ActiveConnection = m_pConnection;
		return true;
	}
	catch(_com_error &e)
	{
		PrintErrorInfo(e);
		m_pConnection = NULL;
		return false;
	}
}

bool CDBOperation::ConnToDB()
{
	return ConnToDB(GetConnectString().c_str(),GetUser().c_str(),GetPassword().c_str());
}


void CDBOperation::BeginTans()
{
	//debug_log(__FUNCTION__);
	assert(m_pConnection);
	if (m_pConnection->State == 0)
	{
		return;
	}
	m_pConnection->BeginTrans();
}

void CDBOperation::Rollback()
{
	//debug_log(__FUNCTION__);
	assert(m_pConnection);
	if (m_pConnection->State == 0)
	{
		return;
	}
	m_pConnection->RollbackTrans();
}

void CDBOperation::Commit()
{
	//debug_log(__FUNCTION__);
	assert(m_pConnection);
	if (m_pConnection->State == 0)
	{
		return;
	}
	m_pConnection->CommitTrans();
}

string CDBOperation::ExecuteWithResSQL(const char *sql,int colno )
{
	string strRet = "";
	/*
	_RecordsetPtr pRecord = ExecuteWithResSQL(sql);
	if (pRecord == NULL)
	{
	return "";
	}
	if (!pRecord->adoEOF)
	{
	_variant_t vRet = pRecord->GetCollect(_variant_t(colno));
	strRet = (char*)(_bstr_t)vRet;
	}*/
	try
	{
		_RecordsetPtr pRecord;
		HRESULT hret = pRecord.CreateInstance(__uuidof(Recordset));
		if (!SUCCEEDED(hret))
		{
			return strRet;
		}
		pRecord = m_pConnection->Execute(sql,NULL,adCmdText);

		//pRecord->Open(sql,_variant_t((IDispatch*)m_pConnection,true),adOpenStatic,adLockOptimistic,adCmdText);
		int count = pRecord->GetRecordCount();
		pRecord->MoveFirst();
		int i = 0;
		while (count-1!=i)
		{
			_variant_t vRet = (_variant_t)pRecord->GetCollect((long)colno);
			if (vRet.vt != VT_NULL)
			{
				strRet = (char*)(_bstr_t)vRet;
			}
			pRecord->Move(i++);
		}
		

		while (!pRecord->adoEOF)
		{
			_variant_t vRet = (_variant_t)pRecord->GetCollect((long)colno);
			if (vRet.vt != VT_NULL)
			{
				strRet = (char*)(_bstr_t)vRet;
			}
			
		} 
		/*if(!pRecord->adoEOF)
		{
			_variant_t vRet = (_variant_t)pRecord->GetCollect((long)colno);
			if (vRet.vt != VT_NULL)
			{
				strRet = (char*)(_bstr_t)vRet;
			}
			
		}*/
		pRecord->Close();
		pRecord.Release();

	}
	catch (_com_error &e)
	{
		//debug_log("获取数据集错误：%s",(char*)e.Description());
	}
	return strRet;
}

_RecordsetPtr CDBOperation::ExecuteWithResSQL(const char *sql)
{
	assert(m_pConnection);
	assert(m_pCommand);
	if (!m_pConnection->State)
	{
		if(!ConnToDB(m_sConnectStr.c_str(),m_sUser.c_str(),m_sPassword.c_str())) return NULL;
		m_pCommand->ActiveConnection = m_pConnection;
	}
	
	//char *query = new char;
	//strcpy(query, sql);
	try
	{
		m_pCommand->CommandText = _bstr_t(sql);
		_RecordsetPtr pRst = m_pCommand->Execute(NULL, NULL, adCmdText);
		return pRst;
		//_variant_t ra;
		//_RecordsetPtr pRst = m_pConnection->Execute((_bstr_t)query, &ra, adCmdText);

	}
	catch(_com_error &e)
	{
		PrintErrorInfo(e);
		return NULL;
	}
}

bool CDBOperation::ExecuteWithNoSQL(const char *sql)
{
	_RecordsetPtr pRecord = ExecuteWithResSQL(sql);
	if (pRecord == NULL)
	{
		return false;
	}
	return true;
}

string CDBOperation::GetTransIDFromDB()
{
	assert(m_pConnection);
	assert(m_pCommand);
	if (!m_pConnection->State)
	{
		if(!ConnToDB(m_sConnectStr.c_str(),m_sUser.c_str(),m_sPassword.c_str())) return NULL;
		m_pCommand->ActiveConnection = m_pConnection;
	}


	try
	{
		_ParameterPtr outParam1 = NULL;
		outParam1.CreateInstance(__uuidof(Parameter));

		m_pCommand->CommandType = adCmdStoredProc;
		m_pCommand->CommandText = _bstr_t("getTransID");

		outParam1 = m_pCommand->CreateParameter(_bstr_t("@P1"),adVarChar,adParamOutput,11,_bstr_t("P1"));
		m_pCommand->Parameters->Append(outParam1);
		m_pCommand->Execute(NULL, NULL, adCmdStoredProc);

		char retstring[512] = {0};
		VARIANT vtP1 = m_pCommand->Parameters->GetItem("@P1")->Value;
		strcpy(retstring,(_bstr_t)vtP1);
		return retstring;

	}
	catch(_com_error &e)
	{
		PrintErrorInfo(e);
		return NULL;
	}
}

string CDBOperation::GetUser()
{
	if (m_sUser.empty())
	{
		const int MAX_LEN = 10000;

		char keyValue[MAX_LEN];
		strcpy(keyValue, CWindChillSetting::GetStrSeverDBUser().GetBuffer());
		//memset(keyValue,0,MAX_LEN);
		//GetInifileString(DBSection,KEY_User,_T(""),keyValue);
		m_sUser = keyValue;
	}
	return m_sUser;
}

string CDBOperation::GetPassword()
{
	if (m_sPassword.empty())
	{
		const int MAX_LEN = 10000;

		char keyValue[MAX_LEN];
		strcpy(keyValue, CWindChillSetting::GetStrSeverDBPassword().GetBuffer());
		//memset(keyValue, 0, MAX_LEN);
		//GetInifileString(DBSection, KEY_Password, _T(""), keyValue);
		m_sPassword = keyValue;
	}
	return m_sPassword;
}


string CDBOperation::GetConnectString()
{
	if (m_sConnectStr.empty())
	{
		const int MAX_LEN = 10000;
		char keyValue[MAX_LEN];
		memset(keyValue, 0, MAX_LEN);
		strcpy(keyValue, CWindChillSetting::GetStrSeverDBDrive().GetBuffer());

		//GetInifileString(DBSection, KEY_Drive, "OraOLEDB.Oracle.1", keyValue);
		m_sConnectStr.append("Provider=");
		m_sConnectStr.append(keyValue);
		m_sConnectStr.append(";Persist Security Info=True");

		m_sConnectStr.append(";Data Source=");
		char szHost[MAX_LEN];
		strcpy(szHost,CWindChillSetting::GetStrSeverDBHost().GetBuffer());
		//GetInifileString(DBSection, KEY_DBHost, _T(""), keyValue);
		//strcpy(szHost, keyValue);

		char szPort[MAX_LEN]; 
		strcpy(szPort, CWindChillSetting::GetStrSeverDBPort().GetBuffer());
		//GetInifileString(DBSection, KEY_DBPort, _T(""), keyValue);
		//strcpy(szPort, keyValue);
		char szService[MAX_LEN]  ;
		strcpy(szService, CWindChillSetting::GetStrSeverDBService().GetBuffer());
		//GetInifileString(DBSection, KEY_DBService, _T(""), keyValue);
	//	strcpy(szService, keyValue);

		char szTnsName[MAX_LEN] = {0};
		sprintf(szTnsName, "(DESCRIPTION =(ADDRESS = (PROTOCOL = TCP)(HOST = %s)(PORT = %s))(CONNECT_DATA =(SERVICE_NAME = %s)))", szHost, szPort, szService);
		m_sConnectStr.append(szTnsName);
	}
	return m_sConnectStr;
}



void CDBOperation::PrintErrorInfo(_com_error &e)
{
	printf("Error infomation are as follows\n");
	CString str;
	str.Format(_T("0x%016x;Error:%s"),e.Error(),e.ErrorMessage());
	if (e.ErrorInfo())
	{
		str+=TEXT("\nSource:")+CString((LPCTSTR(e.Source())))
			+TEXT(";Description:")+CString((LPCTSTR(e.Description())));
	}
	AfxMessageBox(str);
	
}

_ConnectionPtr CDBOperation::CreateConnPtr()
{
	HRESULT hr;
	_ConnectionPtr connPtr;
	hr = connPtr.CreateInstance(__uuidof(Connection));
	if (FAILED(hr) == TRUE)
	{
		return NULL;
	}
	return connPtr;
}


_CommandPtr CDBOperation::CreateCommPtr()
{
	HRESULT hr;

	_CommandPtr commPtr;
	hr = commPtr.CreateInstance(__uuidof(Command));
	if (FAILED(hr) == TRUE)
	{
		return NULL;
	}
	return commPtr;

}

_RecordsetPtr CDBOperation::CreateRecsetPtr()
{
	HRESULT hr;
	_RecordsetPtr recsetPtr;

	hr = recsetPtr.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr) ==TRUE)
	{
		return NULL;
	}
	return recsetPtr;
}
