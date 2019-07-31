#pragma once

class CFTPInterface;
#include <map>
#include <vector>
#include <string>
//////////////////////////////////////////////////////////////////////////////
namespace KmWindChillCommon
{
	class PropData
	{
	public:
		CString m_strName;
		CString m_strValue;

		PropData()
		{
			m_strName = _T("");
			m_strValue = _T("");
		}

		PropData(CString sName, CString sVal)
		{
			m_strName = sName;
			m_strValue = sVal;
		}

		PropData(const PropData& sour)
		{
			*this = sour;
		}

		PropData& operator = (const PropData& sour)
		{
			if (&sour != this)
			{
				m_strName = sour.m_strName;
				m_strValue = sour.m_strValue;
			}
			return *this;
		}
	};

	int FindPropDataIndex(CArray<PropData, PropData>& propArys, CString strKey);
	bool FindPropData(CArray<PropData, PropData>& propArys, CString strKey);
	void AddPropData(CArray<PropData, PropData>& propArys, PropData prop);
	bool FindPropDataInfo(CArray<PropData, PropData>& propArys,CString strKey,CString& strValue);
}

class CWindChillSetting
{
public:
	CWindChillSetting();
	~CWindChillSetting();
	
	//获取数据
	static CWindChillSetting* GetAppSettings();
	
	//记录partFirstName
	static CString m_strpartFirstName;

	//初始化数据
	static void InitRouteParam();
	static void InitRuleParam();

	//设置数据
	//工艺路线 pdf
	//static void SetPdfPath_Route(CString sVal){ m_sOut_PdfPath_Route = sVal; };
	//static CString GetPdfPath_Route(){ return m_sOut_PdfPath_Route; };

	//检入
	///static void CheckInRoute(CString strKmapxFullName);

	//工艺规程 pdf
	static void SetPdfPath_Rule(CString sVal){ m_sOut_PdfPath_Rule = sVal; };
	static CString GetPdfPath_Rule(){ return m_sOut_PdfPath_Rule; };



	static bool LoadLoginConfig();
	//加载FTP数据
	static bool LoadQueryConfig();//查询界面属性映射参数配置
	static bool LoadCheckConfig();//检出属性映射参数配置
	static bool LoadMaterialConfig();//记录哪些属性为关联属性的配置
	static bool LoadFTPConfig();	//FTP参数配置
	static bool LoadWebServices();	//webservice参数配置
	static bool LoadServerConfig();	//serverDB参数配置

	static bool LoadCheckPropMatch();	//检出属性参数配置
	static bool LoadUpadtePropMatch();	//更新零部件新增属性

	static bool LoadChangeinfoMatch();   //更改单信息映射

	static bool LoadRelConfig();

	static bool  LoadResourceTyoeMatch();
	static bool  LoadReadModelConfig();

	static bool LoadCollectionMatch();

	static bool ReadProAry(CString strFilter,CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData>& propArys);

	static void SetLoginInfo(CString strUser, CString strPass);

	static CString GetStrInI() { return m_strIni; };
	static CString GetStrFTPURL() { return m_strFTPURL; };
	static CString GetStrFTPPort() { return m_strFTPPort; };
	static CString GetStrFTPUserName() { return m_strFTPUserName; };
	static CString GetStrFTPPasswd() { return m_strFTPPasswd; };

	static CString GetStrLoginUser() { return m_strLoginUser; };
	static CString GetStrLoginPass() { return m_strLoginPass; };

	static CString GetStrSeverDBDrive()  { return m_strSeverDBDrive; };
	static CString GetStrSeverDBHost()  { return m_strSeverDBHost; };
	static CString GetStrSeverDBPort()  { return m_strSeverDBPort; };
	static CString GetStrSeverDBService()  { return m_strSeverDBService; };
	static CString GetStrSeverDBUser()  { return m_strSeverDBUser; };
	static CString GetStrSeverDBPassword()  { return m_strSeverDBPassword; };
protected:
	void LoadSettings();

	BOOL m_bLoad;

	//static CString m_sOut_PdfPath_Route;//记录最后一次输出工艺路线pdf文件全路径
	static CString m_sOut_PdfPath_Rule;//记录最后一次输出工艺规程pdf文件全路径

	static CString m_strIni;//配置文件全路径

	//FTP记录所有数据
	static CString m_strFTPURL;
	static CString m_strFTPPort;
	static CString m_strFTPUserName;
	static CString m_strFTPPasswd;

	//记录willchill登陆用户名和密码
	static CString m_strLoginUser;
	static CString m_strLoginPass;

	//记录中间服务器
	static CString m_strSeverDBDrive;
	static CString m_strSeverDBHost;
	static CString m_strSeverDBPort;
	static CString m_strSeverDBService;
	static CString m_strSeverDBUser;
	static CString m_strSeverDBPassword;


public:
	static std::map<CString, CString> titleValue;
	static std::map<CString, CString> ReviewValue;
	static std::map<CString, CString> changeValue;

	static int m_iConvertMode;//转换方式： =1 InterOP，= 0 exchange
	static BOOL m_bUseFileName;//转换后代号属性是否以模型文件名为准

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strQueryMatch;//查询界面属性映射
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strCheckMatch;//[检出属性映射]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strMaterialMatch;//[材料定额属性映射]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strMaterialType;//[材料类型属性映射]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strAccessoryMatch;//[材材辅料属性映射]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strRelMatch;     //[关联属性映射]

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strCheckPropMatch;     //[检入自定义属性校验]

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strUpdatePropMatch;    //更新零部件属性
	
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strChangeinfoMatch;     //[更改单信息]

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strResourceTypeMatch;    //工艺装备类型

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strCollectionMatch;    //汇总类型


	static CString m_strWebURL; //webservice路径

	static std::vector<std::string> GYJID;    //工艺件ID
	static std::vector<std::string> PROCUDEID;    //所有产品ID
};	



