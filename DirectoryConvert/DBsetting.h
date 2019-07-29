#pragma once

class CFTPInterface;

//////////////////////////////////////////////////////////////////////////////

class CWindChillSetting
{
public:
	CWindChillSetting();
	~CWindChillSetting();

	static CString GetStrSeverDBDrive()  { return m_strSeverDBDrive; };
	static CString GetStrSeverDBHost()  { return m_strSeverDBHost; };
	static CString GetStrSeverDBPort()  { return m_strSeverDBPort; };
	static CString GetStrSeverDBService()  { return m_strSeverDBService; };
	static CString GetStrSeverDBUser()  { return m_strSeverDBUser; };
	static CString GetStrSeverDBPassword()  { return m_strSeverDBPassword; };
	
	static bool LoadServerConfig();//serverDB参数配置
protected:
	//记录中间服务器
	static CString m_strSeverDBDrive;
	static CString m_strSeverDBHost;
	static CString m_strSeverDBPort;
	static CString m_strSeverDBService;
	static CString m_strSeverDBUser;
	static CString m_strSeverDBPassword;

	static CString m_strIni;//配置文件全路径

};	



