#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Resource.h"
#include "tinyxml.h"
#include "EasySize.h"
#include "DirectoryDialog.h"
#include <string>
#include<vector>
#include<map>
#include "WebServiceInterface.h"

// CDlgMaterialQuery �Ի���

class CDlgMaterialQuery : public CDialog
{
	DECLARE_DYNAMIC(CDlgMaterialQuery)
	DECLARE_EASYSIZE
public:
	CDlgMaterialQuery(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMaterialQuery();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// �Ի�������
	enum { IDD = IDD_WINDCHILL_MATERIALEDITSELECT };

	TiXmlDocument GetXmlPtr() { return m_pPBOMxml; }
protected:
	
	void InitList();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnQuery();
	afx_msg void OnOK();
	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()
private:

	CWebServiceInterface m_WebServiceInterface;
	CFTPInterface m_FTPInterface;
	
	int m_row;
	int m_col;
	
	CListCtrl m_Resultlist;
	CString partNumber;

public:
	CString GetPartNumber() {return partNumber;};

	TiXmlDocument Materialxml;
	TiXmlDocument PBOMxml;

	TiXmlDocument m_pPBOMxml;			//webserverce���ص�xml
};
