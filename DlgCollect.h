#pragma once
#include "Resource.h"
#include <afxwin.h>
#include <map>
#include <afxcmn.h>

class CDlgCollect :
	public CDialog
{
public:
	CDlgCollect(CWnd* pParent = NULL);
	virtual ~CDlgCollect();

	// ����
	// �Ի�������
	enum {IDD = IDD_WINDCHILL_COLLECT};

protected:
	void InitCombox();
	void InitTab();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();

	
private:
	CComboBox m_combox;
	std::map<CString, CString> m_Type;
	std::map<CString, CTabCtrl*> m_tab;

public:
	// //tab�ؼ��� ����λ�þ���˿ؼ���ͬ
	CTabCtrl m_TabCtrl;
};

