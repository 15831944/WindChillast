#include "stdafx.h"

#include "DBsetting.h"
#include "UtilityFunction.h"
#define INI_VALUE_LEN 512

//记录中间服务器
CString CWindChillSetting::m_strSeverDBDrive = _T("");
CString CWindChillSetting::m_strSeverDBHost = _T("");
CString CWindChillSetting::m_strSeverDBPort = _T("");
CString CWindChillSetting::m_strSeverDBService = _T("");
CString CWindChillSetting::m_strSeverDBUser = _T("");
CString CWindChillSetting::m_strSeverDBPassword = _T("");


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
			/*m_strSeverDBPassword = base64_decode(strVal.GetBuffer()).c_str();
			strVal.ReleaseBuffer();*/
		}
	}
	return true;
}

