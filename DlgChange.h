#pragma once

#include "BCGPPropList.h"
#include "BCGPTreeCtrl.h"
#include "Output3DPdf.h"
#include "PDFConfig.h"

struct ChangeInfo
{
	CString plant;
	CString genggaidan;
	CString pronum;
	CString lzjname;
	CString	lzjnum;
	CString	ffdw;
	CString	grouptotal;
	CString	groupcurrent;
	CString	dwg;
};

// CDlgChange 对话框
class CDlgChange : public CDialog
{
	DECLARE_DYNAMIC(CDlgChange)
public:
	CDlgChange(IAPSApplication* pApplication, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgChange();

// 对话框数据
	enum { IDD = IDD_WINDCHILL_CHANGE };

	virtual BOOL OnInitDialog();
	void InitGridCtrl();
	void InitTreeCtrl();

	void InsertChange();
	void DeleteChange();

	void InitInfo();
	void SaveData();

	void GetPdfConfig();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

	afx_msg void OnContextMenu(CWnd*, CPoint);
	afx_msg LRESULT OnItemChanged(WPARAM uProListId, LPARAM pProp);

	afx_msg LRESULT OnGridItemChanged(WPARAM uProListId, LPARAM pProp);
	afx_msg LRESULT OnProChanged(WPARAM uProListId, LPARAM pProp);
	IAPSApplication* m_pApplication;
	COutPut3DPdf *m_putpdf;
	ConfigGroup *m_pChangeConfig;

	
private:
	CBCGPPropList m_BCGPropList;
	CBCGPTreeCtrlEx m_wndTree;
	std::map<CString, ChangeInfo*> m_XmlData;
public:
	afx_msg void OnBnClickedSavechange();
};
