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

	// 构造
	// 对话框数据
	enum {IDD = IDD_WINDCHILL_COLLECT};

protected:
	void InitCombox();
	void InitTab();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();

	
private:
	CComboBox m_combox;
	std::map<CString, CString> m_Type;
	std::map<CString, CTabCtrl*> m_tab;

public:
	// //tab控件的 风格和位置均与此控件相同
	CTabCtrl m_TabCtrl;
};

