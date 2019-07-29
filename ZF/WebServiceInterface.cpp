#include "stdafx.h"
#include "WebServiceInterface.h"
#include "stdsoap2.h"
#include "soapH.h"

#include "curl.h"
#include "curlver.h"
#include <string>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include "UtilityFunction.h"
#include "WindChillSetting.h"
#include <atlbase.h>
#ifdef COMPANY
#include "Service1Soap.nsmap"
#endif // COMPANY

#ifdef LOCALE
#include "SoapBinding.nsmap"
#endif // LOCALE

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

static size_t read_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t read = fread(ptr, size, nmemb, (FILE *)stream);
	return read;
}

CWebServiceInterface::CWebServiceInterface()
{
}

CWebServiceInterface::~CWebServiceInterface()
{
}


CString CWebServiceInterface::checkLogin(CString strUserName, CString strPasswd)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);

	sp.userid=strUserName.GetBuffer();
	sp.passwd=strPasswd.GetBuffer();

	soap_set_mode(&sp, SOAP_C_UTFSTRING);

	ns1__checkLogin trans;
	char *Collection = "";
	
	//int iRet =soap_call_ns1__ping(&sp,NULL,NULL,s);
	CWaitCursor wait;

	int iRet= soap_call_ns1__checkLogin(&sp,NULL,NULL,Collection);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		
		soap_end(&sp);
		soap_done(&sp);

		return NULL;
	}
	else
	{
		result = Collection;
	}
	soap_end(&sp);
	soap_done(&sp);

	return result.c_str();
#endif

#ifdef COMPANY
	struct soap sp;
	std::string result;
	soap_init(&sp);
	_tempuri__checkLogin trans;
	_tempuri__checkLoginResponse response;

	trans.soap = &sp;

	std::string parttmp = strUserName.GetBuffer();
	int size = MultiByteToWideChar(CP_ACP, 0, parttmp.c_str(), -1, NULL, 0);
	
	wchar_t *ch = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, parttmp.c_str(), -1, ch, size);

	std::wstring part = ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str;
	str = new char[dwNum + 1];
	WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);

	trans.userName = str;

	CWaitCursor wait;
	int iRet = soap_call___tempuri__checkLogin_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		return NULL;
	}
	else
	{
		result = response.checkLoginResult;
	}
	soap_end(&sp);
	soap_done(&sp);

	return result.c_str();
#endif // COMPANY

}

CString CWebServiceInterface::getPartList(CString operType, CString partNumber, CString productName, CString department)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);

	sp.userid=CWindChillSetting::GetStrLoginUser();
	sp.passwd=CWindChillSetting::GetStrLoginPass();

	soap_set_mode(&sp, SOAP_C_UTFSTRING);

	ns1__getPartList trans;
	std::string output;

	char *Collection = "";

	//char ת wchar
	std::string parttmp = partNumber.GetBuffer();
	CWaitCursor wait;
	int size =MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, NULL, 0);
	wait.Restore();
	wchar_t *ch =new wchar_t[size+1];
	MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, ch, size);

	std::wstring part=ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str;
	str = new char[dwNum +1];
	WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);

	int iRet = soap_call_ns1__getPartList(&sp, NULL, NULL,operType.GetBuffer(), str,Collection);

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		 output = Collection;
	}

	// wcharתchar
	std::wstring a;

	WFromUTF8(output.c_str(), a);
	dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum +1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	
	soap_end(&sp);
	soap_done(&sp);
	CString tmp =sabstr;
	//AfxMessageBox(tmp);
	delete []ch;
	delete []sabstr;
	
	return tmp;
#endif

#ifdef COMPANY
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__getPartList trans;
	_tempuri__getPartListResponse response;
	trans.soap = &sp;
	std::string output;

	trans.operType = operType.GetBuffer();
	trans.partNumber = partNumber.GetBuffer();
	trans.productName = productName.GetBuffer();
	trans.department = department.GetBuffer();
	operType.ReleaseBuffer();
	partNumber.ReleaseBuffer();
	productName.ReleaseBuffer();
	department.ReleaseBuffer();

	CWaitCursor wait;
	int iRet = soap_call___tempuri__getPartList_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.getPartListResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	
	soap_end(&sp);
	soap_done(&sp);

	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

CString CWebServiceInterface::getBom(CString  operType, CString partNumber)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);

	sp.userid=CWindChillSetting::GetStrLoginUser();
	sp.passwd=CWindChillSetting::GetStrLoginPass();

	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	ns1__getBom trans;
	char *Collection = "";
	std::string output;

	//USES_CONVERSION;
	//char ת wchar
	std::string parttmp = partNumber.GetBuffer();
	int size =MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, NULL, 0);
	wchar_t *ch =new wchar_t[size+1];
	MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, ch, size);

	std::wstring part=ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str;
	str = new char[dwNum +1];
	WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);
	CWaitCursor wait;
	int iRet = soap_call_ns1__getBom(&sp, NULL, NULL,operType.GetBuffer(),str ,Collection);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output =Collection;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	if (ch)
	{
		delete[]ch;
		ch = NULL;
	}
	CString tmp =sabstr;
	if (sabstr)
	{
		delete[]sabstr;
		sabstr = NULL;
	}

	if (str)
	{
		delete[]str;
		str = NULL;
	}
	return tmp;
#endif

#ifdef COMPANY
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__getBom trans;
	_tempuri__getBomResponse response;
	trans.soap = &sp;
	std::string output;

	trans.operType = operType.GetBuffer();
	trans.partNumber = partNumber.GetBuffer();

	operType.ReleaseBuffer();
	partNumber.ReleaseBuffer();

	CWaitCursor wait;
	int iRet = soap_call___tempuri__getBom_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.getBomResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

CString CWebServiceInterface::getDocInfo(CString  operType, CString docNumber)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);

	sp.userid=CWindChillSetting::GetStrLoginUser();
	sp.passwd=CWindChillSetting::GetStrLoginPass();

	soap_set_mode(&sp, SOAP_C_UTFSTRING);

	ns1__getPartList trans;
	std::string output;

	char *Collection = "";

	//char ת wchar
	std::string parttmp = docNumber.GetBuffer();
	CWaitCursor wait;
	int size =MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, NULL, 0);
	wait.Restore();
	wchar_t *ch =new wchar_t[size+1];
	MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, ch, size);

	std::wstring part=ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str;
	str = new char[dwNum +1];
	WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);

	int iRet = soap_call_ns1__getDocInfo(&sp, NULL, NULL,operType.GetBuffer(),str ,Collection);
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = Collection;
	}

	// wcharתchar
	std::wstring a;

	WFromUTF8(output.c_str(), a);
	dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum +1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);

	soap_end(&sp);
	soap_done(&sp);
	CString tmp =sabstr;
	//AfxMessageBox(tmp);
	delete []ch;
	delete []sabstr;

	return tmp;
#endif

#ifdef COMPANY
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__getDocInfo trans;
	_tempuri__getDocInfoResponse response;
	trans.soap = &sp;
	std::string output;

	trans.operType = operType.GetBuffer();
	trans.docNumbe = docNumber.GetBuffer();

	operType.ReleaseBuffer();
	docNumber.ReleaseBuffer();

	CWaitCursor wait;
	int iRet = soap_call___tempuri__getDocInfo_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.getDocInfoResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

CString CWebServiceInterface::getBOMOfHsf(CString operType, CString partNumber)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);

	sp.userid=CWindChillSetting::GetStrLoginUser();
	sp.passwd=CWindChillSetting::GetStrLoginPass();

	std::string output;

	char * Collection ="";

	ns1__getBOMOfHsf trans;

	std::string parttmp = partNumber.GetBuffer();
	int size =MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, NULL, 0);
	wchar_t *ch =new wchar_t[size+1];
	MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, ch, size);

	std::wstring part=ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str;
	str = new char[dwNum +1];
	WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);

	CWaitCursor wait;
	int iRet = soap_call_ns1__getBOMOfHsf(&sp, NULL, NULL,operType.GetBuffer(),str,Collection);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = Collection;
	}

	if (ch)
	{
		delete[]ch;
		ch = NULL;
	}



	if (str)
	{
		delete[]str;
		str = NULL;
	}

	return  output.c_str();
#endif	

#ifdef COMPANY
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__getBOMOfHsf trans;
	_tempuri__getBOMOfHsfResponse response;
	trans.soap = &sp;
	std::string output;

	CWaitCursor wait;
	int iRet = soap_call___tempuri__getBOMOfHsf_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.getBOMOfHsfResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif
}

CString CWebServiceInterface::getDocInfoOfContent(CString  operType, CString docNumber)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);

	sp.userid=CWindChillSetting::GetStrLoginUser();
	sp.passwd=CWindChillSetting::GetStrLoginPass();

	std::string output;

	char * Collection ="";

	ns1__getBOMOfHsf trans;

	std::string parttmp = docNumber.GetBuffer();
	int size =MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, NULL, 0);
	wchar_t *ch =new wchar_t[size+1];
	MultiByteToWideChar(CP_ACP, 0,parttmp.c_str(), -1, ch, size);

	std::wstring part=ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str;
	str = new char[dwNum +1];
	WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);

	CWaitCursor wait;
	int iRet = soap_call_ns1__getDocInfoOfContent(&sp, NULL, NULL,operType.GetBuffer(),str,Collection);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = Collection;
	}

	if (ch)
	{
		delete[]ch;
		ch = NULL;
	}

	if (str)
	{
		delete[]str;
		str = NULL;
	}

	return  output.c_str();
#endif	

#ifdef COMPANY
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__getDocInfoOfContent trans;
	_tempuri__getDocInfoOfContentResponse response;
	trans.soap = &sp;
	std::string output;

	trans.operType = operType.GetBuffer();
	trans.docNumbe = docNumber.GetBuffer();

	operType.ReleaseBuffer();
	docNumber.ReleaseBuffer();


	CWaitCursor wait; 
	int iRet = soap_call___tempuri__getDocInfoOfContent_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.getDocInfoOfContentResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

CString CWebServiceInterface::CheckInDoc(CString  operType, CString inputxml)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);

	sp.userid=CWindChillSetting::GetStrLoginUser();
	sp.passwd=CWindChillSetting::GetStrLoginPass();
	
	std::string output;

	char * Collection ="";

	ns1__checkInPbom trans;

	std::string xmltmp = inputxml.GetBuffer();
	int size =MultiByteToWideChar(CP_ACP, 0,xmltmp.c_str(), -1, NULL, 0);
	wchar_t *ch =new wchar_t[size+1];
	MultiByteToWideChar(CP_ACP, 0,xmltmp.c_str(), -1, ch, size);

	std::wstring content=ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str= new char[dwNum +1];
	WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, str, dwNum, NULL, FALSE);
	
	CWaitCursor wait;
	int iRet =soap_call_ns1__checkInDoc(&sp,NULL,NULL,operType.GetBuffer(),str,Collection);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = Collection;
	}

	if (ch)
	{
		delete[]ch;
		ch = NULL;
	}

	

	if (str)
	{
		delete[]str;
		str = NULL;
	}

	return  output.c_str();
#endif

#ifdef COMPANY

	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__CheckInDoc trans;
	_tempuri__CheckInDocResponse response;
	trans.soap = &sp;
	std::string output;
	trans.fileName = inputxml.GetBuffer();

	inputxml.ReleaseBuffer();


	CWaitCursor wait;
	int iRet = soap_call___tempuri__CheckInDoc_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.CheckInDocResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

CString CWebServiceInterface::checkInPbom(CString inputxml)
{

#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);

	sp.userid=CWindChillSetting::GetStrLoginUser();
	sp.passwd=CWindChillSetting::GetStrLoginPass();

	std::string output;

	char * Collection ="";

	ns1__checkInPbom trans;

	std::string xmltmp = inputxml.GetBuffer();
	int size =MultiByteToWideChar(CP_ACP, 0,xmltmp.c_str(), -1, NULL, 0);
	wchar_t *ch =new wchar_t[size+1];
	MultiByteToWideChar(CP_ACP, 0,xmltmp.c_str(), -1, ch, size);

	std::wstring content=ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str= new char[dwNum +1];
	WideCharToMultiByte(CP_UTF8, 0, content.c_str(), -1, str, dwNum, NULL, FALSE);

	CWaitCursor wait;
	int iRet = soap_call_ns1__checkInPbom(&sp, NULL, NULL,str,Collection);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = Collection;
	}

	if (ch)
	{
		delete[]ch;
		ch = NULL;
	}

	if (str)
	{
		delete[]str;
		str = NULL;
	}

	return  output.c_str();
#endif

#ifdef COMPANY

	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__checkInPbom trans;
	_tempuri__checkInPbomResponse response;
	trans.soap = &sp;
	std::string output;
	trans.fileName = inputxml.GetBuffer();

	inputxml.ReleaseBuffer();


	CWaitCursor wait;
	int iRet = soap_call___tempuri__checkInPbom_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.checkInPbomResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

CString CWebServiceInterface::SearchResource(CString  operType, CString resourceNumber)
{
#ifdef LOCALE
	struct soap sp;
	std::string result;
	soap_init(&sp);

	sp.userid = CWindChillSetting::GetStrLoginUser();
	sp.passwd = CWindChillSetting::GetStrLoginPass();

	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	ns1__getBom trans;
	char *Collection = "";
	std::string output;

	//USES_CONVERSION;
	//char ת wchar
	std::string parttmp = resourceNumber.GetBuffer();
	int size = MultiByteToWideChar(CP_ACP, 0, parttmp.c_str(), -1, NULL, 0);
	wchar_t *ch = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, parttmp.c_str(), -1, ch, size);

	std::wstring part = ch;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	char * str;
	str = new char[dwNum + 1];
	WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);
	CWaitCursor wait;
	int iRet = soap_call_ns1__SearchResource(&sp, NULL, NULL, operType.GetBuffer(), str, Collection);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = Collection;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	
	if (ch)
	{
		delete[]ch;
		ch = NULL;
	}

	if (sabstr)
	{
		delete[]sabstr;
		sabstr = NULL;
	}

	if (str)
	{
		delete[]str;
		str = NULL;
	}
	return tmp;
#endif

#ifdef COMPANY
	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__SearchResource trans;
	_tempuri__SearchResourceResponse response;
	trans.soap = &sp;
	std::string output;

	trans.operType = operType.GetBuffer();
	trans.resourceNumber= resourceNumber.GetBuffer();

	operType.ReleaseBuffer();
	resourceNumber.ReleaseBuffer();


	CWaitCursor wait; 
	int iRet = soap_call___tempuri__SearchResource_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();

	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.SearchResourceResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

CString CWebServiceInterface::SearchResourceOfContent(CString  operType, CString resourceNumber)
{
#ifdef LOCALE
	//struct soap sp;
	//std::string result;
	//soap_init(&sp);

	//sp.userid = CWindChillSetting::GetStrLoginUser();
	//sp.passwd = CWindChillSetting::GetStrLoginPass();

	//soap_set_mode(&sp, SOAP_C_UTFSTRING);
	//ns1__getBom trans;
	//char *Collection = "";
	//std::string output;

	////USES_CONVERSION;
	////char ת wchar
	//std::string parttmp = resourceNumber.GetBuffer();
	//int size = MultiByteToWideChar(CP_ACP, 0, parttmp.c_str(), -1, NULL, 0);
	//wchar_t *ch = new wchar_t[size + 1];
	//MultiByteToWideChar(CP_ACP, 0, parttmp.c_str(), -1, ch, size);

	//std::wstring part = ch;
	//DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, NULL, 0, NULL, FALSE);
	//char * str;
	//str = new char[dwNum + 1];
	//WideCharToMultiByte(CP_UTF8, 0, part.c_str(), -1, str, dwNum, NULL, FALSE);
	//CWaitCursor wait;
	//int iRet = soap_call_ns1__SearchResourceOfContent(&sp, NULL, NULL, operType.GetBuffer(), str, Collection);
	//wait.Restore();

	//if (sp.error)
	//{
	//	result = sp.error;
	//	soap_end(&sp);
	//	soap_done(&sp);
	//	return NULL;
	//}
	//else
	//{
	//	output = Collection;
	//}

	//std::wstring a;
	//WFromUTF8(output.c_str(), a);
	//dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	//char * sabstr;
	//sabstr = new char[dwNum + 1];
	//WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	//soap_end(&sp);
	//soap_done(&sp);
	//CString tmp = sabstr;
	//
	//if (ch)
	//{
	//	delete[]ch;
	//	ch = NULL;
	//}

	//if (sabstr)
	//{
	//	delete[]sabstr;
	//	sabstr = NULL;
	//}

	//if (str)
	//{
	//	delete[]str;
	//	str = NULL;
	//}
	//return tmp;
	//return "";
	return "";
#endif

#ifdef COMPANY

	struct soap sp;
	std::string result;
	soap_init(&sp);
	soap_set_mode(&sp, SOAP_C_UTFSTRING);
	_tempuri__SearchResourceOfContent trans;
	_tempuri__SearchResourceOfContentResponse response;
	trans.soap = &sp;
	std::string output;

	trans.operType = operType.GetBuffer();
	trans.resourceNumber = resourceNumber.GetBuffer();

	operType.ReleaseBuffer();
	resourceNumber.ReleaseBuffer();

	CWaitCursor wait;
	int iRet = soap_call___tempuri__SearchResourceOfContent_(&sp, CWindChillSetting::m_strWebURL, NULL, &trans, response);
	wait.Restore();
	if (sp.error)
	{
		result = sp.error;
		soap_end(&sp);
		soap_done(&sp);
		return NULL;
	}
	else
	{
		output = response.SearchResourceOfContentResult;
	}

	std::wstring a;
	WFromUTF8(output.c_str(), a);
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
	soap_end(&sp);
	soap_done(&sp);
	CString tmp = sabstr;
	return tmp;
#endif // COMPANY
}

bool CWebServiceInterface::WFromUTF8(const char* szText, std::wstring &wstrVal)
{
	return WcharFromChar(CP_UTF8, szText, wstrVal);
}

bool CWebServiceInterface::WcharFromChar(unsigned int CodePage, const char* szText, std::wstring & wstrVal)
{
	WCHAR *lpwszVal(NULL);
	int nLength = 0;
	size_t dwSize = 0;
	nLength = ::MultiByteToWideChar(CodePage, 0, szText, -1, NULL, 0);
	if (nLength <= 0)
	{
		return false;
	}
	dwSize = nLength * sizeof(WCHAR);
	lpwszVal = (WCHAR *)malloc(dwSize);
	if (lpwszVal == NULL) return false;
	memset(lpwszVal, 0x0, dwSize);
	nLength = ::MultiByteToWideChar(CodePage, 0, szText, -1, lpwszVal, nLength);
	if (nLength <= 0)
	{
		free(lpwszVal);
		lpwszVal = NULL;
		return false;
	}
	wstrVal.assign(lpwszVal);
	free(lpwszVal);
	lpwszVal = NULL;

	return true;
}

CFTPInterface::CFTPInterface()
{
	m_pCurl = NULL;
}

CString GetModuleDir()
{
	HINSTANCE hModule = AfxGetInstanceHandleHelper();
	if (!hModule)
	{
		hModule = GetModuleHandle(_T("WindChill.dll"));
	}

	TCHAR lpBuffer[MAX_PATH];
	ZeroMemory(lpBuffer, sizeof(lpBuffer));
	GetModuleFileName(hModule, lpBuffer, MAX_PATH);
	ASSERT(_tcslen(lpBuffer) > 0);

	CString strPath(lpBuffer);

	int nIndex = strPath.ReverseFind(_T('\\'));
	ASSERT(nIndex != -1);

	return strPath.Mid(0, nIndex);
}

int CFTPInterface::LoadFTPConfig(CString& strFTPURL, CString &strFTPPort, CString &strFTPUserName, CString &strFTPPasswd)
{
	CString strIni = GetModuleDir() + _T("\\WindChillConfig.ini");
	TCHAR chSectionnames[2048] = { 0 };
	char * pSectionName;
	int i;
	int j = 0;
	int count = 0;
	::GetPrivateProfileSectionNames(chSectionnames, 2048, strIni);
	for (i = 0; i < 2048; i++, j++)
	{
		if (chSectionnames[0] == '\0')
			break;
		if (chSectionnames[i] == '\0')
		{
			pSectionName = &chSectionnames[i - j];
			count++;
			j = -1;
			if (strcmp(pSectionName, _T("FTP��������")) != 0)
				continue;
			CStringArray arKeys;
			EnumIniKeys(strIni, pSectionName, arKeys);
			for (INT_PTR ii = 0; ii < arKeys.GetSize(); ii++)
			{
				CString strVal;
				CString strKey = arKeys.GetAt(ii);
				GetPrivateProfileString(pSectionName, strKey, _T(""), strVal.GetBuffer(512), 512, strIni);
				strVal.ReleaseBuffer();
				if (strKey.CompareNoCase("URL") == 0)
					strFTPURL = strVal;
				if (strKey.CompareNoCase("Port") == 0)
					strFTPPort = strVal;
				if (strKey.CompareNoCase("UserName") == 0)
					strFTPUserName = strVal;
				if (strKey.CompareNoCase("Passwd") == 0)
					strFTPPasswd = strVal;
			}

			if (chSectionnames[i + 1] == 0)
			{
				break;
			}
		}
	}
	return count;
}

CFTPInterface::~CFTPInterface()
{
	curl_global_cleanup();
	m_pCurl = NULL;
}

BOOL CFTPInterface::Connect(CString strURL, CString strPort, CString strUserName, CString strPasswd)
{
	CURLcode errorCode = CURLE_OK;
	m_pCurl = curl_easy_init();
	if (!m_pCurl) return false;

	char userpwd[200];
	memset(userpwd, 0, 200);
	_snprintf_s(userpwd, 200, "%s:%s", strUserName, strPasswd);

	curl_easy_setopt(m_pCurl, CURLOPT_USERPWD, userpwd);
	//curl_easy_setopt(m_pCurl, CURLOPT_URL, strURL);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, strURL);
	curl_easy_setopt(m_pCurl, CURLOPT_PORT, _ttoi(strPort));
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, 600);
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 600);
	CWaitCursor wait;
	errorCode = curl_easy_perform(m_pCurl);
	wait.Restore();
	if (errorCode != CURLE_OK)
	{
		return false;
	}
	//curl_easy_cleanup(m_pCurl);
	//m_pCurl = NULL;
	return true;
}

BOOL CFTPInterface::UpLoad(CString localfile, CString ftpurl, CString ftpport, CString relative_dir, CString filename)
{
	
 	if (_access(localfile, 0) != 0)
	{
		//debug_log("�ļ������ڣ�%s", localfile);
		return false;
	}
	//m_pCurl = curl_easy_init();
	if (!m_pCurl) return false;

	FILE *fp = NULL;
	fp = fopen(localfile, "rb");
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	char dir[200];
	memset(dir, '\0', 200);
	strcpy(dir, relative_dir);
	if (relative_dir[strlen(relative_dir) - 1] != '/')
	{
		strcat(dir, "/");
	}

	char fullpath[512];
	memset(fullpath, 0, 512);
	sprintf(fullpath, "%s:%s/%s%s", ftpurl, ftpport, dir, filename);
	//sprintf(fullpath,"%s%s",m_sFtpUrl.c_str(),filename);

	curl_easy_setopt(m_pCurl, CURLOPT_URL, fullpath);
	curl_easy_setopt(m_pCurl, CURLOPT_READDATA, fp);
	curl_easy_setopt(m_pCurl, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);
	curl_easy_setopt(m_pCurl, CURLOPT_INFILESIZE, size);
	curl_easy_setopt(m_pCurl, CURLOPT_READFUNCTION, read_data);
	CWaitCursor wait;
	CURLcode errorCode = curl_easy_perform(m_pCurl);
 	wait.Restore();
	if (errorCode != CURLE_OK)
	{
		//PrintErrMsg(errorCode);
		return false;
	}
	fclose(fp);

	//curl_easy_cleanup(m_pCurl);
	//m_pCurl = NULL;
	return true;
}

BOOL CFTPInterface::Delete(CString localfile, CString ftpurl, CString ftpport, CString relative_dir, CString filename, CString strUserName, CString strPasswd)
{
	char dir[200];
	memset(dir, '\0', 200);
	strcpy(dir, relative_dir);
	if (relative_dir[strlen(relative_dir) - 1] != '/')
	{
		strcat(dir, "/");
	}

	char fullpath[512];
	memset(fullpath, 0, 512);
	sprintf(fullpath, "%s:%s/%s", ftpurl, ftpport, dir);
	//sprintf(fullpath, "%s:%s", ftpurl, ftpport);

	char userpwd[200];
	memset(userpwd, 0, 200);
	_snprintf_s(userpwd, 200, "%s:%s", strUserName, strPasswd);

	char szCmd[1024] = "Dele ";
	strcat(szCmd, filename);

	//m_pCurl = curl_easy_init();
	if (!m_pCurl) return false;

	curl_easy_setopt(m_pCurl, CURLOPT_URL, fullpath);
	curl_easy_setopt(m_pCurl, CURLOPT_USERPWD, userpwd);
	auto headerlist = curl_slist_append(NULL, szCmd);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTQUOTE, headerlist);


	CWaitCursor wait;
	CURLcode errorCode = curl_easy_perform(m_pCurl);
	wait.Restore();
	if (errorCode != CURLE_OK)
	{
		return false;
	}
	//curl_easy_cleanup(m_pCurl);
	//m_pCurl = NULL;
	return true;
}


BOOL CFTPInterface::DownLoad(CString localfile, CString ftpurl, CString ftpport, CString relative_dir, CString filename, CString strUserName, CString strPasswd)
{
	//m_pCurl = curl_easy_init();
	if (!m_pCurl) return false;

	FILE *fp = NULL;
	fp = fopen(localfile, "wb");
	if (!fp)
	{
		return FALSE;
	}
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	
	

	char dir[200];
	memset(dir, '\0', 200);
	strcpy(dir, relative_dir);
	if (relative_dir[strlen(relative_dir) - 1] != '/')
	{
		strcat(dir, "/");
	}

	char fullpath[512];
	memset(fullpath, 0, 512);
	sprintf(fullpath, "%s:%s%s%s", ftpurl, ftpport, dir, filename);
	//sprintf(fullpath,"%s%s",m_sFtpUrl.c_str(),filename);

	char userpwd[200];
	memset(userpwd, 0, 200);
	_snprintf_s(userpwd, 200, "%s:%s", strUserName, strPasswd);

	curl_easy_setopt(m_pCurl, CURLOPT_USERPWD, userpwd);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, fullpath);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, write_data);
	CWaitCursor wait;
	CURLcode errorCode = curl_easy_perform(m_pCurl);
	wait.Restore();
	if (errorCode != CURLE_OK)
	{
		return false;
	}
	fclose(fp);
	//curl_easy_cleanup(m_pCurl);
	//m_pCurl = NULL;
	return true;
}
