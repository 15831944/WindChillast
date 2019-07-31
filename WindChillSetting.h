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
	
	//��ȡ����
	static CWindChillSetting* GetAppSettings();
	
	//��¼partFirstName
	static CString m_strpartFirstName;

	//��ʼ������
	static void InitRouteParam();
	static void InitRuleParam();

	//��������
	//����·�� pdf
	//static void SetPdfPath_Route(CString sVal){ m_sOut_PdfPath_Route = sVal; };
	//static CString GetPdfPath_Route(){ return m_sOut_PdfPath_Route; };

	//����
	///static void CheckInRoute(CString strKmapxFullName);

	//���չ�� pdf
	static void SetPdfPath_Rule(CString sVal){ m_sOut_PdfPath_Rule = sVal; };
	static CString GetPdfPath_Rule(){ return m_sOut_PdfPath_Rule; };



	static bool LoadLoginConfig();
	//����FTP����
	static bool LoadQueryConfig();//��ѯ��������ӳ���������
	static bool LoadCheckConfig();//�������ӳ���������
	static bool LoadMaterialConfig();//��¼��Щ����Ϊ�������Ե�����
	static bool LoadFTPConfig();	//FTP��������
	static bool LoadWebServices();	//webservice��������
	static bool LoadServerConfig();	//serverDB��������

	static bool LoadCheckPropMatch();	//������Բ�������
	static bool LoadUpadtePropMatch();	//�����㲿����������

	static bool LoadChangeinfoMatch();   //���ĵ���Ϣӳ��

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

	//static CString m_sOut_PdfPath_Route;//��¼���һ���������·��pdf�ļ�ȫ·��
	static CString m_sOut_PdfPath_Rule;//��¼���һ��������չ��pdf�ļ�ȫ·��

	static CString m_strIni;//�����ļ�ȫ·��

	//FTP��¼��������
	static CString m_strFTPURL;
	static CString m_strFTPPort;
	static CString m_strFTPUserName;
	static CString m_strFTPPasswd;

	//��¼willchill��½�û���������
	static CString m_strLoginUser;
	static CString m_strLoginPass;

	//��¼�м������
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

	static int m_iConvertMode;//ת����ʽ�� =1 InterOP��= 0 exchange
	static BOOL m_bUseFileName;//ת������������Ƿ���ģ���ļ���Ϊ׼

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strQueryMatch;//��ѯ��������ӳ��
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strCheckMatch;//[�������ӳ��]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strMaterialMatch;//[���϶�������ӳ��]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strMaterialType;//[������������ӳ��]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strAccessoryMatch;//[�Ĳĸ�������ӳ��]
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strRelMatch;     //[��������ӳ��]

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strCheckPropMatch;     //[�����Զ�������У��]

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strUpdatePropMatch;    //�����㲿������
	
	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strChangeinfoMatch;     //[���ĵ���Ϣ]

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strResourceTypeMatch;    //����װ������

	static CArray<KmWindChillCommon::PropData, KmWindChillCommon::PropData> m_strCollectionMatch;    //��������


	static CString m_strWebURL; //webservice·��

	static std::vector<std::string> GYJID;    //���ռ�ID
	static std::vector<std::string> PROCUDEID;    //���в�ƷID
};	



