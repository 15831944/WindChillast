#pragma once

#include "APSProp.h"
#include "EditListCtrl.h"
#include "..\ResourceDef\APM_Res.h"
#include "AccessoryPage.h"
#include "ResourcePage.h"
#include "..\KmCommonCtrl\KmPropertyGridCtrl.h"
#include "KmPropertySheet.h"
#include "..\KmCommonCtrl\TreeListCtrl.h"
#include "GridPage.h"

#include "EasySize.h"
class CProcess;
class CAPManager;

class CProcessBasicPage : public CKmPropertyPage
{
	DECLARE_DYNAMIC(CProcessBasicPage)
	DECLARE_EASYSIZE
public:
	CProcessBasicPage();
	virtual ~CProcessBasicPage();

	// 对话框数据
	enum { IDD = IDD_APM_PROCESS_BASIC_PROP };

protected:
	CAPManager * m_pManager;
	CProcess * m_pProcess;
private:
	bool m_bDirtyName;
	bool m_bDirtyContent;
	bool m_bDirtyExport;
	bool m_bVis;

protected:
	CEdit	m_editContent;
	CEdit	m_editName;

public:
	void ReadProp(void);
	void WriteProp(void);
	void SetAPManager(CAPManager* pManager);
	void Update(CProcess * pProcess);
	bool IsVisPage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnKillFocusProcessName();
	afx_msg void OnKillFocusAsmStepName();
	afx_msg void OnKillFocusAsmStepContent();
	afx_msg void OnClickSpecialSymbolBtn();
	afx_msg void OnClickKmResBase();
	afx_msg void OnClickKmGrapText();
	afx_msg void OnBnClickedButtonModifly();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnEnChangeEditAsmstepName();
	afx_msg void OnEnChangeEditAsmstepContent();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCheckExport();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};


class CActionBase;
class CAPManager;

/////////////////////////////////////////////////////////////////////////////
// CProcessCustomPage dialog
class CPropListCtrl;
class CProcessCustomPage : public CKmPropertyPage
{
	DECLARE_DYNCREATE(CProcessCustomPage)
	DECLARE_EASYSIZE
	// Construction
public:
	CProcessCustomPage();
	~CProcessCustomPage();

	// Dialog Data
	//{{AFX_DATA(CStepCustomPage)
	enum { IDD = IDD_APM_STEP_CUSTOM_PROP };
	// NOTE - ClassWizard will add data members here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	KmAPSTreeListCtrl::CTreeListCtrl m_listDefine;
	//KmAPSTreeListCtrl::CTreeListCtrl * m_pTreeList;
	CPropListCtrl *m_pPropListCtrl;
	//}}AFX_DATA

private:
	CAPManager*		m_pManager;
	//CAsmStep*		m_pAsmStep;         //步骤
	CProcess * m_pProcess;
	CAPSPropList	m_useList;
	//CAPSPropList	m_baseList;         //步骤属性的临时备份

	CString m_strUsePropName;   // 正在编辑的自定义属性名,吴克兵 2009-10-23
protected:
	CAPSPropList m_ColumnItems;
	CArray<CAPSPropList> m_BackUpResDatas;
	bool m_bDoModel;
	bool m_bModified;
public:
	CImageList m_ImageList;
	int m_nItemIndex;
	int m_nSubItemIndex;
private:
	//void SetListStyle(void);
	void SetBaseList(CAPSPropList& list);
	void SetUseList(CAPSPropList& list);
	void ShowTheList(int nCol);

	void SetModify(bool bModify);
	void UpdateItem(int nItem, const CAPSProp & Prop);
public:
	void ReadProp(void);
	void WriteProp(void);
	void WriteProp(int nRow);  // 吴克兵 2009-10-23
	void SetAPManager(CAPManager* pManager);
	void Update(CProcess * pProcess);
	void FormatStrByPropType(int type, CString & strInOut);
	void InsertUsePropItem(const CAPSProp & prop);
	void InsertPropListItem(CAPSPropList & RefPropList);

	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonC();
	afx_msg void OnBnClickedButtonG();
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStepCustomPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
public:
	bool CanEdit(int nRow,int nCol);
	CString GetFreePropName();
	BOOL IsFreePropName(const CAPSPropList & PropList, const CString & name, int nIgnoreIndex/* = -1*/);
	int ConvertType(LPCTSTR szType);
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStepCustomPage)
	//'virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnKillFocusListMultiValue();
	afx_msg void OnKillFocusEditMultiValue();
	afx_msg void OnKillFocusCustomList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBeginLabelEdit(WPARAM wParam, LPARAM lParam);
	//afx_msg void OnRclickListAsmstepDefine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListMutileValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddUseprop();
	afx_msg void OnDeleteUseprop();
	afx_msg void OnUpdateUseprop();
	afx_msg void OnClickAsmObjList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonMutileValue();
	afx_msg void OnBnClickedButtonMutileValueDel();

	afx_msg void OnKeyDownInMutileValue(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnKillFocus(CWnd* pNewWnd);

	afx_msg LRESULT OnBCGGridSelChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnBCGGridRButtonUp(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//virtual BOOL OnKillActive();
private:
	void UpdateMutileList();
	bool IsMutileType(int nIndex);
	int m_nRowSel;

	void AdjustPlace();

	void OnKillFocusSave();

	void SetMutileEditShow(bool bShow);
};


class CProcessMainPropSheet : public CKmPropertySheet
{
	DECLARE_DYNAMIC(CProcessMainPropSheet)

// Construction
public:
	CProcessMainPropSheet(CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CProcessMainPropSheet();

protected:
	CAPManager * m_pAPManager;
	CProcessBasicPage *	m_pBasicPage;
	CProcessCustomPage * m_pCustomPage;
	//CProcessAccessoryPage* m_pAcessPage;
	CProcessResourcePage* m_pResourcePage;

	//实现动态添加网格页面 add by cx 2014.07.04
	CArray<CProcessGridPage*> m_ProcessGridPageAry;

// Operations
public:
	CKmPropertyPage * GetPageByProp(CString strPageName);

	void SetAPManager(CAPManager * pManager);
	void Update(CProcess * pProcess);
	CGridPage* GetPageByName(CString strName);

protected:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg LRESULT OnUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
};