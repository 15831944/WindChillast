#pragma once

class CWindChillSetting
{
public:
	CWindChillSetting();
	~CWindChillSetting();
	
	//��ȡ����
	static CWindChillSetting* GetAppSettings();


	static bool LoadServerConfig();//serverDB��������

	static bool CWindChillSetting::LoadpathConfig();

	static CString GetStrSeverDBDrive()  { return m_strSeverDBDrive; };
	static CString GetStrSeverDBHost()  { return m_strSeverDBHost; };
	static CString GetStrSeverDBPort()  { return m_strSeverDBPort; };
	static CString GetStrSeverDBService()  { return m_strSeverDBService; };
	static CString GetStrSeverDBUser()  { return m_strSeverDBUser; };
	static CString GetStrSeverDBPassword()  { return m_strSeverDBPassword; };

	static CString GetPath()  { return m_path; };
	static CString GetConverpath()  { return m_converpath; };
protected:
	void LoadSettings();


	static CString m_strIni;//�����ļ�ȫ·��

	static CString m_path;
	static CString m_converpath;

	//��¼�м������
	static CString m_strSeverDBDrive;
	static CString m_strSeverDBHost;
	static CString m_strSeverDBPort;
	static CString m_strSeverDBService;
	static CString m_strSeverDBUser;
	static CString m_strSeverDBPassword;

};	



