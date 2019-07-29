#pragma once

#include "Resource.h"
// CDlgLogIn �Ի���
#include "WebServiceInterface.h"
#include "afxwin.h"

class CDlgLogIn : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogIn)

public:
	CDlgLogIn(bool is_login=false,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLogIn();
// �Ի�������
	enum { IDD = IDD_WINDCHILL_LOGIN };
	bool GetloginStae() { return b_isLogin; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnLoginWindChill();
	DECLARE_MESSAGE_MAP()
	
private:
	CEdit m_edit_name;
	CEdit m_edit_pass;
	bool b_isLogin;
	HICON m_hIcon;
};
