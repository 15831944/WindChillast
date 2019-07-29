#pragma once

#include "afxcmn.h"
#include"tinyxml.h"
#include "EasySize.h"
#include "Resource.h"
#include <string>
#include <vector>
#include <map>
#include "WebServiceInterface.h"
#include "common.h"


// CDlgAddMaterial 对话框

class CDlgAddMaterial : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddMaterial)
	DECLARE_EASYSIZE
public:
	CDlgAddMaterial(ResourceType type, UINT ID =0,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAddMaterial();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// 对话框数据
	enum { IDD = IDD_WINDCHILL_SELECTACCESSORY };
	TiXmlDocument GetXMl() { return AddMaterialxml; };
protected:	
	void InitList();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnQuery();
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	DECLARE_MESSAGE_MAP()
private:
	std::map<std::string, std::string> KeyWords;
	std::vector<std::string> Rank;
	TiXmlDocument AddMaterialxml;
	CListCtrl m_Resultlist;
	HICON m_hIcon;
	CWebServiceInterface m_WebServiceInterface;
	CFTPInterface m_FTPInterface;

	ResourceType m_type;
	UINT m_ID;
public:
	afx_msg void OnBnClickedAddmaterialOk();
};
