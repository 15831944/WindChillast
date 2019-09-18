#pragma once
#include <afxwin.h>

#include "BCGPPropList.h"
#include "Resource.h"
class CDlgInfo :
	public CDialog
{

	DECLARE_DYNAMIC(CDlgInfo)
	
public:
	// 对话框数据
	enum { IDD = IDD_WINDCHILL_INFO };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void InitPropList();
	DECLARE_MESSAGE_MAP()
public:
	CDlgInfo(CWnd* pParent = NULL);
	~CDlgInfo();
private:
//	CMFCPropertyGridCtrl m_PropGridCtrl;
	CBCGPPropList m_list;
};

