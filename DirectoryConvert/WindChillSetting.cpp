#include "stdafx.h"

#include "UtilityFunction.h"
#include "Basex64.h"

#include "WindChillSetting.h"
#include "stdlib.h"

#define INI_VALUE_LEN 512




CString CWindChillSetting::m_strIni = _T("");

CString CWindChillSetting::m_path = _T("");
CString	  CWindChillSetting::m_converpath= _T("");



//记录中间服务器
CString CWindChillSetting::m_strSeverDBDrive = _T("");
CString CWindChillSetting::m_strSeverDBHost = _T("");
CString CWindChillSetting::m_strSeverDBPort = _T("");
CString CWindChillSetting::m_strSeverDBService = _T("");
CString CWindChillSetting::m_strSeverDBUser = _T("");
CString CWindChillSetting::m_strSeverDBPassword = _T("");



////////////////////////////////
CWindChillSetting::CWindChillSetting()
{
	//char chpath[100];

	//m_strIni = CString("E:\\3DCAPP-A 12.1\\Plugins\\Project\\VC2013\\x64\\Debug") + _T("\\WindChillConfig.ini");
	m_strIni =GetModuleDir() + _T("\\DirectoryConvert.ini");
	if (!ExistFile(m_strIni))
	{
		CString strPrompt;
		strPrompt.Format("请检查配置文件%s是否存在 ", m_strIni);
		MessageBox(NULL, strPrompt, "KMAPS", MB_ICONINFORMATION);
	}
	
}


CWindChillSetting::~CWindChillSetting()
{

}

CWindChillSetting* CWindChillSetting::GetAppSettings()
{
	static CWindChillSetting appSettings;

	
	appSettings.LoadSettings();


	return &appSettings;
}

void CWindChillSetting::LoadSettings()
{
	try
	{
		if (m_strIni.IsEmpty())
		{
			return;
		}

		LoadpathConfig();
		LoadServerConfig();
		
		
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

}


//Service配置
bool CWindChillSetting::LoadpathConfig()
{
	CString strFilter = _T("转换目录配置");

	CStringArray arKeys;
	EnumIniKeys(m_strIni, strFilter, arKeys);
	for (INT_PTR i = 0; i < arKeys.GetSize(); i++)
	{
		CString strVal;
		CString strKey = arKeys.GetAt(i);
		GetPrivateProfileString(strFilter, strKey, _T(""), strVal.GetBuffer(INI_VALUE_LEN), INI_VALUE_LEN, m_strIni);
		strVal.ReleaseBuffer();

		if (strKey.CompareNoCase("path") == 0)
		{
			m_path = strVal;
		}

		if (strKey.CompareNoCase(_T("convertoolpath")) == 0)
		{
			m_converpath = strVal;
		}
	}
	return true;
}



//Service配置
bool CWindChillSetting::LoadServerConfig()
{
	CString strFilter = _T("Service参数配置");

	CStringArray arKeys;
	EnumIniKeys(m_strIni, strFilter, arKeys);
	for (INT_PTR i = 0; i < arKeys.GetSize(); i++)
	{
		CString strVal;
		CString strKey = arKeys.GetAt(i);
		GetPrivateProfileString(strFilter, strKey, _T(""), strVal.GetBuffer(INI_VALUE_LEN), INI_VALUE_LEN, m_strIni);
		strVal.ReleaseBuffer();

		if (strKey.CompareNoCase("Drive") == 0)
		{
			m_strSeverDBDrive = strVal;
		}
		else if (strKey.CompareNoCase("Host") == 0)
		{
			m_strSeverDBHost = strVal;
		}
		else if (strKey.CompareNoCase("Port") == 0)
		{
			m_strSeverDBPort = strVal;
		}
		else if (strKey.CompareNoCase("Service") == 0)
		{
			m_strSeverDBService = strVal;
		}
		else if (strKey.CompareNoCase("User") == 0)
		{
			m_strSeverDBUser = strVal;
		}
		else if (strKey.CompareNoCase("Password") == 0)
		{
			m_strSeverDBPassword = base64_decode(strVal.GetBuffer()).c_str();
			strVal.ReleaseBuffer();
		}
	}
	return true;
}
