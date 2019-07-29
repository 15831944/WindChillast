#pragma once

#include"EasySize.h"
#include "tinyxml.h"
#include "WebServiceInterface.h"



// CDlgPBOMEdit 对话框


class CDlgPBOMEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgPBOMEdit)
	DECLARE_EASYSIZE
public:
	CDlgPBOMEdit(CAPSModel * pModel, QueryType nType, IAPSApplication* pApplication, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPBOMEdit();

// 对话框数据
	enum { IDD = IDD_WINDCHILL_PBOMEDIT };
public:
	void SetCaption(CString Caption);
	TiXmlDocument GetXmlPtr() { return m_PBOMxml; }
	CString GetPartNumber()  {return partNumber;}

	afx_msg void OnQuery();
	afx_msg void OnOK();
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void InitList();
	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()

	BOOL GetDataFromWebService(int nRow);
private:
	CAPSModel * m_pModel;
	CString m_Caption;
	CListCtrl m_Resultlist;

	IAPSApplication* m_pApplication;

	CString partNumber;
	TiXmlDocument m_PBOMxml;
	HICON m_hIcon;
	CWebServiceInterface m_WebServiceInterface;
	CFTPInterface m_FTPInterface;
	QueryType m_nQueryType;

};

