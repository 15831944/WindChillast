#include "stdafx.h"

#include "UtilityFunction.h"
#include "Basex64.h"
#include "WebServiceInterface.h"
#include "WindChillSetting.h"

using namespace KmWindChillCommon;
#define INI_VALUE_LEN 512


CString CWindChillSetting::m_sOut_PdfPath_Rule = _T("");

CString CWindChillSetting::m_strIni = _T("");

CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strQueryMatch;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strCheckMatch;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strMaterialMatch;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strMaterialType;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strAccessoryMatch;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strRelMatch;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strCheckPropMatch;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strUpdatePropMatch;
CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strChangeinfoMatch;

CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strResourceTypeMatch;

CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> CWindChillSetting::m_strCollectionMatch;
CString CWindChillSetting::m_strFTPURL = _T("");
CString CWindChillSetting::m_strFTPPort = _T("");
CString CWindChillSetting::m_strFTPUserName = _T("");
CString CWindChillSetting::m_strFTPPasswd = _T("");
CString CWindChillSetting::m_strWebURL = _T("");

CString CWindChillSetting::m_strLoginUser = _T("");
CString CWindChillSetting::m_strLoginPass = _T("");

CString CWindChillSetting::m_strpartFirstName = _T("");

//记录中间服务器
CString CWindChillSetting::m_strSeverDBDrive = _T("");
CString CWindChillSetting::m_strSeverDBHost = _T("");
CString CWindChillSetting::m_strSeverDBPort = _T("");
CString CWindChillSetting::m_strSeverDBService = _T("");
CString CWindChillSetting::m_strSeverDBUser = _T("");
CString CWindChillSetting::m_strSeverDBPassword = _T("");

std::map<CString, CString> CWindChillSetting::titleValue;
std::map<CString, CString> CWindChillSetting::ReviewValue;
std::map<CString, CString> CWindChillSetting::changeValue;

std::vector<std::string>  CWindChillSetting::GYJID;
std::vector<std::string>  CWindChillSetting::PROCUDEID;    //所有产品ID
int  CWindChillSetting::m_iConvertMode=0;//转换方式： =1 InterOP，= 0 exchange
//////////////////////////////////////////////////////////////////////////////////////////////////
namespace KmWindChillCommon
{
	bool FindPropData(CArray<PropData, PropData>& propArys, CString strKey)
	{
		bool bFind = false;
		for (int i = 0; i < propArys.GetSize(); i++)
		{
			if (propArys[i].m_strName.CompareNoCase(strKey) == 0)
			{
				bFind = true;
				break;
			}
		}

		return bFind;
	}


	int FindPropDataIndex(CArray<PropData, PropData>& propArys, CString strKey)
	{
		int index = -1;
		for (int i = 0; i < propArys.GetSize(); i++)
		{
			if (propArys[i].m_strName.CompareNoCase(strKey) == 0)
			{
				index = i;
				break;
			}

			if (propArys[i].m_strValue.CompareNoCase(strKey) == 0)
			{
				index = i;
				break;
			}
		}
		return index;
	}

	bool FindPropDataInfo(CArray<PropData, PropData>& propArys, CString strValue, CString& strKey)
	{
		bool bFind = false;
		for (int i = 0; i < propArys.GetSize(); i++)
		{
			if (propArys[i].m_strValue.CompareNoCase(strValue) == 0)
			{
				bFind = true;
				strKey = propArys[i].m_strName;
				break;
			}
		}

		return bFind;
	}

	void AddPropData(CArray<PropData, PropData>& propArys, PropData prop)
	{
		if (FindPropData(propArys, prop.m_strName))
		{
			return;
		}

		propArys.Add(prop);
	}
}
////////////////////////////////
CWindChillSetting::CWindChillSetting()
{
	m_bLoad = FALSE;
	m_strIni = GetModuleDir() + _T("\\WindChillConfig.ini");
	if (!ExistFile(m_strIni))
	{
		CString strPrompt;
		strPrompt.Format("请检查配置文件%s是否存在 ", m_strIni);
		MessageBox(NULL, strPrompt, APS_MSGBOX_TITIE, MB_ICONINFORMATION);
	}

	InitRouteParam();
	InitRuleParam();
}


CWindChillSetting::~CWindChillSetting()
{

}


void CWindChillSetting::InitRouteParam()
{
	//m_sOut_PdfPath_Route = _T("");
}

void CWindChillSetting::InitRuleParam()
{
	m_sOut_PdfPath_Rule = _T("");
}



CWindChillSetting* CWindChillSetting::GetAppSettings()
{
	static CWindChillSetting appSettings;

	if (!appSettings.m_bLoad)
	{
		appSettings.LoadSettings();
	}

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

		if (!m_bLoad)
		{
			LoadLoginConfig();
			LoadQueryConfig();
			
			LoadMaterialConfig();
			LoadFTPConfig();
			LoadWebServices();
			LoadServerConfig();
			LoadRelConfig();

			LoadCheckPropMatch();
			LoadUpadtePropMatch();

			LoadReadModelConfig();

			LoadChangeinfoMatch();
			LoadResourceTyoeMatch();

			LoadCollectionMatch();
		}
	}
	catch (...)
	{
		ASSERT(FALSE);
	}
	m_bLoad = TRUE;
}


bool CWindChillSetting::LoadLoginConfig()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	CString strFilter = _T("登录信息");

	CStringArray arKeys;
	 EnumIniKeys(m_strIni, strFilter, arKeys);

	 for (INT_PTR i = 0; i < arKeys.GetSize(); i++)
	 {
		 CString strVal;
		 CString strKey = arKeys.GetAt(i);
		 GetPrivateProfileString(strFilter, strKey, _T(""), strVal.GetBuffer(INI_VALUE_LEN), INI_VALUE_LEN, m_strIni);
		 strVal.ReleaseBuffer();
		 if (i==0)
		 {
			 m_strLoginUser=strVal;
		 }
		 else
		 {
			 m_strLoginPass = base64_decode(strVal.GetBuffer()).c_str();
			 strVal.ReleaseBuffer();
		 }
	 }

	return true;
}




//查询界面属性映射参数配置
bool CWindChillSetting::LoadQueryConfig()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	bool bRet = ReadProAry(_T("查询界面属性映射"), m_strQueryMatch);
	return bRet;
}

bool CWindChillSetting::LoadCheckPropMatch()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	bool bRet = ReadProAry(_T("检入属性校验"), m_strCheckPropMatch);
	return bRet;
}



bool CWindChillSetting::LoadUpadtePropMatch()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	bool bRet = ReadProAry(_T("添加零部件新增属性"), m_strUpdatePropMatch);
	return bRet;
}


bool CWindChillSetting::LoadRelConfig()
{
	m_strRelMatch.RemoveAll();
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	CString strFilter = _T("关联属性映射");

	CStringArray arKeys;
	EnumIniKeys(m_strIni, strFilter, arKeys);
	for (INT_PTR i = 0; i < arKeys.GetSize(); i++)
	{
		CString strVal;
		CString strKey = arKeys.GetAt(i);
		GetPrivateProfileString(strFilter, strKey, _T(""), strVal.GetBuffer(INI_VALUE_LEN), INI_VALUE_LEN, m_strIni);
		strVal.ReleaseBuffer();

		PropData prop(strKey, strVal);
		AddPropData(m_strRelMatch, prop);
	}

	return true;
}

bool CWindChillSetting::LoadChangeinfoMatch()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	bool bRet = ReadProAry(_T("更改单信息映射"), m_strChangeinfoMatch);
	return bRet;
}

bool CWindChillSetting::LoadResourceTyoeMatch()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	bool bRet = ReadProAry(_T("工艺装备类型"), m_strResourceTypeMatch);
	return bRet;
}
bool CWindChillSetting::LoadMaterialConfig()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	ReadProAry(_T("材料定额编辑属性映射"),m_strMaterialMatch);
	ReadProAry(_T("材料类型属性映射"), m_strMaterialType);
	ReadProAry(_T("材辅料属性映射"), m_strAccessoryMatch);
	
	return true;
}

//FTP参数配置
bool CWindChillSetting::LoadFTPConfig()
{
	CString strFilter = _T("FTP参数配置");

	CStringArray arKeys;
	EnumIniKeys(m_strIni, strFilter, arKeys);
	for (INT_PTR i = 0; i < arKeys.GetSize(); i++)
	{
		CString strVal;
		CString strKey = arKeys.GetAt(i);
		GetPrivateProfileString(strFilter, strKey, _T(""), strVal.GetBuffer(INI_VALUE_LEN), INI_VALUE_LEN, m_strIni);
		strVal.ReleaseBuffer();

		if (strKey.CompareNoCase("URL") == 0)
		{
			m_strFTPURL = strVal;
		}
		else if (strKey.CompareNoCase("Port") == 0)
		{
			m_strFTPPort = strVal;
		}
		else if (strKey.CompareNoCase("UserName") == 0)
		{
			m_strFTPUserName = strVal;
		}
		else if (strKey.CompareNoCase("Passwd") == 0)
		{
			m_strFTPPasswd = base64_decode(strVal.GetBuffer()).c_str();
			strVal.ReleaseBuffer();
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



bool CWindChillSetting::LoadWebServices()
{
	m_strWebURL = _T("");
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	CString strFilter = _T("webservice");

	CStringArray arKeys;
	EnumIniKeys(m_strIni, strFilter, arKeys);
	for (INT_PTR i = 0; i < arKeys.GetSize(); i++)
	{
		CString strVal;
		CString strKey = arKeys.GetAt(i);
		GetPrivateProfileString(strFilter, strKey, _T(""), strVal.GetBuffer(INI_VALUE_LEN), INI_VALUE_LEN, m_strIni);
		strVal.ReleaseBuffer();

		if (strKey.CompareNoCase("URL") ==0)
		{
			m_strWebURL = strVal;
			break;
		}
	}
	return true;
}

bool CWindChillSetting::ReadProAry(CString strFilter,CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData>& propArys)
{
	propArys.RemoveAll();
	CStringArray arKeys;
	EnumIniKeys(m_strIni, strFilter, arKeys);
	for (INT_PTR i = 0; i < arKeys.GetSize(); i++)
	{
		CString strVal;
		CString strKey = arKeys.GetAt(i);
		GetPrivateProfileString(strFilter, strKey, _T(""), strVal.GetBuffer(INI_VALUE_LEN), INI_VALUE_LEN, m_strIni);
		strVal.ReleaseBuffer();

		PropData prop(strKey, strVal);
		AddPropData(propArys, prop);
	}

	return true;
}



void CWindChillSetting::SetLoginInfo(CString strUser, CString strPass)
{
	m_strLoginUser = strUser;
	m_strLoginPass = strPass;

	WritePrivateProfileString(_T("登录信息"),_T("用户名"),strUser,m_strIni);

	auto encodepass =base64_encode(strPass.GetBuffer(),strPass.GetLength());
 	WritePrivateProfileString(_T("登录信息"),_T("密码"),encodepass.c_str(),m_strIni);
}	

CString GetRightValByStr(const CString& strLine, const CString& strTag)
{
	CString ss(strLine);
	ss.Delete(0, strTag.GetLength());

	ss.TrimLeft();
	ss.TrimRight();

	int nIndex = ss.Find('=');
	if (nIndex != -1)
	{
		ss.Delete(0, nIndex + 1);
		ss.TrimLeft();

		return ss;
	}

	return "";
}

bool CWindChillSetting::LoadReadModelConfig()
{
	CString strIni = GetModuleDir() + _T("\\readmodel.ini");
	if (!ExistFile(strIni))
	{
		return false;
	}

	CStdioFile ff;
	if (ff.Open(strIni, CFile::modeRead))
	{
		CString strLine;
		while (ff.ReadString(strLine))
		{
			if (strLine.GetLength() < 2)
			{//空行
				continue;
			}

			if (strLine.Mid(0, 2).CompareNoCase(_T("//")) == 0)
			{//注释
				continue;
			}

			int nIndex = strLine.Find(_T("[UseIop]"));
			if (nIndex != -1)
			{
				m_iConvertMode = _tstoi(GetRightValByStr(strLine, _T("[UseIop]")));
				break;;
			}
		}

		ff.Close();
	}

	return true;
}

bool CWindChillSetting::LoadCollectionMatch()
{
	if (m_strIni.IsEmpty())
	{
		return false;
	}

	bool bRet = ReadProAry(_T("汇总信息映射"), m_strCollectionMatch);
	return bRet;
}
