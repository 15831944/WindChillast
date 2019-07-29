#pragma once
#include "stdafx.h"
#include <string>
typedef void CURL;

class CWebServiceInterface
{
public:
	CWebServiceInterface();   // 标准构造函数
	virtual ~CWebServiceInterface();

	CString checkLogin(CString strUserName, CString strPasswd);
	CString getBOMOfHsf(CString operType, CString partNumber);
	CString getPartList(CString operType, CString partNumber, CString productName, CString department);
	CString getBom(CString  operType, CString partNumber);
	CString getDocInfo(CString  operType, CString docNumber);
	CString getDocInfoOfContent(CString  operType, CString docNumber);
	CString CheckOutPBOM(CString partName);
	CString checkInPbom(CString inputxml);
	CString SearchResource(CString  operType, CString resourceNumber);
	CString SearchResourceOfContent(CString  operType, CString resourceNumber);
	CString CheckInDoc(CString  operType, CString inputxml);

	bool WFromUTF8(const char* szText, std::wstring &wstrVal);
	bool WcharFromChar(unsigned int CodePage, const char* szText, std::wstring & wstrVal);
};

class CFTPInterface
{
public:
	CURL * m_pCurl;
	CFTPInterface();   // 标准构造函数
	virtual ~CFTPInterface();
	int LoadFTPConfig(CString& strFTPURL, CString &strFTPPort, CString &strFTPUserName, CString &strFTPPasswd);
	BOOL Delete(CString localfile, CString ftpurl, CString ftpport, CString relative_dir, CString filename, CString strUserName, CString strPasswd);
	BOOL Connect(CString strURL, CString strPort, CString strUserName, CString strPasswd);
	BOOL UpLoad(CString localfile, CString ftpurl, CString ftpport, CString relative_dir, CString filename);
	BOOL DownLoad(CString localfile, CString ftpurl, CString ftpport, CString relative_dir, CString filename, CString strUserName, CString strPasswd);
};