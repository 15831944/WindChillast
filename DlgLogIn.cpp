// DlgLogIn.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgLogIn.h"
#include "WindChillXml.h"
#include "WindChillSetting.h"

// CDlgLogIn 对话框

static CString g_strName = _T("");

IMPLEMENT_DYNAMIC(CDlgLogIn, CDialog)

BEGIN_MESSAGE_MAP(CDlgLogIn, CDialog)
	ON_BN_CLICKED(IDC_WINDCHILL_LOGIN, OnLoginWindChill)
END_MESSAGE_MAP()

CDlgLogIn::CDlgLogIn(bool is_login,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogIn::IDD, pParent)
{
	b_isLogin = is_login;
}

CDlgLogIn::~CDlgLogIn()
{
}

void CDlgLogIn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIN_USERNAME, m_edit_name);
	DDX_Control(pDX, IDC_LOGIN_PASSWORD, m_edit_pass);
}

BOOL CDlgLogIn::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_edit_name.SetWindowText(g_strName);

	m_edit_name.SetWindowText(CWindChillSetting::GetStrLoginUser());
	m_edit_pass.SetWindowText(CWindChillSetting::GetStrLoginPass());

	return TRUE;
}


void CDlgLogIn::OnLoginWindChill()
{
	CString strName = _T(""), strPass = _T("");
	m_edit_name.GetWindowText(strName);
	m_edit_pass.GetWindowText(strPass);

	//设置下一次界面启动默认值
	g_strName = strName;

	/*std::string XmlContent = 
		"<wc:COLLECTION xmlns:wc=\"http://www.ptc.com/infoengine/1.0\">" \
		"<Login NAME = \"output\" TYPE = \"Object\" STATUS = \"0\">" \
		"<wc:INSTANCE>" \
		"<MESSAGE>Failue</MESSAGE>"\
		"</wc:INSTANCE>"\
		"</Login>" \
		"</wc:COLLECTION>";*/
	CWebServiceInterface WebServiceInterface;
	CString XmlContent = WebServiceInterface.checkLogin(strName, strPass);
	bool bSucess = CWindChillXml::ParseResult(XmlContent);
	if(bSucess)
	{
		CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
		if (pSetting)
		{
			pSetting->SetLoginInfo(strName, strPass);
		}
		b_isLogin = true;
	}
	else
	{
		MessageBox("账号密码错误,请重新输入", APS_MSGBOX_TITIE, MB_OK);
		b_isLogin = false;
	}
#ifdef DEV_TMP
	b_isLogin = true;
#endif
	if (b_isLogin)
	{
		CDialog::OnOK();
	}
}

BOOL CDlgLogIn::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
				OnLoginWindChill();
					return TRUE;
		}
// 		else if (pMsg->wParam == VK_ESCAPE)
// 		{
// 			CDialog::OnClose();
// 		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
