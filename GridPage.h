/////网格属性页面 兼容之前的辅料 资源
/////create by cx 2014.07.04

#pragma once

#include "APSProp.h"
#include "EditListCtrl.h"
#include "..\ResourceDef\APM_Res.h"
#include "..\KmCommonCtrl\KmPropertyGridCtrl.h"
#include "..\include\BCG\BCGCBProInc.h"

class CActionBase;
class CAPManager;
class CBCGPGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// CResourcePage dialog
class CBCGPGridRow;
class CGridPage : public CKmPropertyPage
{
private:
	CGridPage();
// Construction
public:
	CGridPage(UINT uID, UINT uCaptionID);
	~CGridPage();

public:	
	virtual void ReadProp(void) = 0;
	virtual void WriteProp(void) = 0;
	virtual void InitColumn() = 0;
	virtual int GetActType() = 0;

	bool GetColShow(int nCol);
	// 根据控件列号获取真正的列号
	int GetColByCtrlCol(int nCtrlCol);
	void InitListCtrl();
	void SetAPManager(CAPManager* pManager);
	inline void SetVis(BOOL BVis) {m_BVis = BVis;}
	CString IsHaveResMgrPath(CProcess* pProcss,CActionBase* pAction,CString strGridName,CString strPropName);

	CBCGPGridCtrl* GetGridCtrl();
	void AdjustPlace();

	// Implementation
protected:
	CString GetFreePropName();
	BOOL IsFreePropName(const CString & name, int nIgnoreIndex = -1);
	BOOL IsSamePropList(CAPSPropList & PropList1, CAPSPropList & PropList2);
	CBCGPGridRow *InsertPropListItem(CAPSPropList& RefPropList, int iPos = -1);
	//void GetResMgrInfo(LPCTSTR lpstrFullPath, CString & strBuffer, bool bForePart = true);
	void SaveData();
	void RemoveAllSelRows();
	std::vector<int> GetSelectedRows();

	static void FormatStrByPropType(int type, CString & strInOut);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CResourcePage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	bool m_bOpenKmRefManager;
	bool m_bModified;
	bool m_bSave;
	BOOL m_BVis;

	CAPManager * m_pManager;
	CBCGPGridCtrl* m_pGridCtrl;
	CImageList m_ImageList;


	CAPSPropList m_ColumnItems;
	CArray<CAPSPropList, CAPSPropList> m_BackUpDatas;

public:
	CButton m_DeleteItemBtn;
	CButton m_AddItemBtn;
	CButton m_MoveUpBtn;
	CButton m_MoveDownBtn;
	CButton m_CancelBtn;
	CButton m_SaveBtn;

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedApmResDelete();
	afx_msg void OnBnClickedApmResAdd();
	afx_msg void OnBnClickedApmResMoveUp();
	afx_msg void OnBnClickedApmResMoveDown();
	afx_msg void OnBnClickedApmResCancel();
	afx_msg void OnBnClickedApmResSave();

	LRESULT OnBCGGridSelChanged(WPARAM wp, LPARAM lp);//add by dm
	
	DECLARE_MESSAGE_MAP()
};

class CResGridPage : public CGridPage
{
	// Construction
public:
	CResGridPage();

	enum { IDD = IDD_APM_RESOURCE_PROP };

public:
	virtual void InitColumn();
};

class CActionGridPage : public CResGridPage
{
	DECLARE_DYNCREATE(CActionGridPage)
public:
	CActionGridPage();
	~CActionGridPage();
public:	
	virtual void ReadProp(void);
	virtual void WriteProp(void);
	virtual int  GetActType();

	virtual void InitColumn();
	virtual void GetPropColumn(CActionBase* pAction, CAPSPropList& columnItems);
	virtual void Update(CActionBase * pAction);

	

	//add by cx 2014.07.07 
	void CActionGridPage::SetPropColumn(const CAPSPropList& proplist);
	inline void SetGridName(CString strName) {m_GridName = strName;}
	inline CString GetGridName() {return m_GridName;}
	inline CActionBase* GetAction() {return m_pAction;}

protected:
	CActionBase * m_pAction;
	CString m_GridName;
	int iAction;

};

/*class CStepGridPage : public CResGridPage
{
	DECLARE_DYNCREATE(CStepGridPage)
public:
	CStepGridPage();
	~CStepGridPage();
public:	
	virtual void ReadProp(void);
	virtual void WriteProp(void);

	virtual void InitColumn();
	virtual void GetPropColumn(CActionBase* pAction, CAPSPropList& columnItems);
	virtual void Update(CActionBase * pAction);

	//add by cx 2014.07.07 
	void CStepGridPage::SetPropColumn(CAPSPropList* proplist);
	inline void SetGridName(CString strName) {m_GridName = strName;};
	inline CString GetGridName() {return m_GridName;};

protected:
	CActionBase * m_pAction;
	CString m_GridName;
};

class CGroupGridPage : public CResGridPage
{
	DECLARE_DYNCREATE(CGroupGridPage)
public:
	CGroupGridPage();
	~CGroupGridPage();
public:	
	virtual void ReadProp(void);
	virtual void WriteProp(void);

	virtual void InitColumn();
	virtual void GetPropColumn(CActionBase* pAction, CAPSPropList& columnItems);
	virtual void Update(CActionBase * pAction);

	//add by cx 2014.07.07 
	void  SetPropColumn(CAPSPropList* proplist);
	inline void SetGridName(CString strName) {m_GridName = strName;};
	inline CString GetGridName() {return m_GridName;};

protected:
	CActionBase * m_pAction;
	CString m_GridName;
};*/


class CProcessGridPage : public CResGridPage
{
	DECLARE_DYNCREATE(CProcessGridPage)
public:
	CProcessGridPage();
	~CProcessGridPage();
public:	
	virtual void ReadProp(void);
	virtual void WriteProp(void);
	virtual int  GetActType();

	virtual void InitColumn();
	virtual void GetPropColumn(CProcess* pProc, CAPSPropList& columnItems);

	virtual void Update(CProcess * pProcess);

	void SetPropColumn(const CAPSPropList& proplist);
	inline void SetGridName(CString strName) {m_GridName = strName;}
	inline CString GetGridName() {return m_GridName;}
	inline CProcess* GetProcess() {return m_pProcess;}

protected:
	CProcess * m_pProcess;
	CString m_GridName;
	int iProcess;

};