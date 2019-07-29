
// windchillDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "tinyxml.h"
#include "Partinfo.h"
#include "EasySize.h"
#include "Resource.h"
#include "WebServiceInterface.h"
// CDlgMaterialEdit 对话框
class CDlgMaterialEdit : public CDialog
{
	DECLARE_EASYSIZE
// 构造
public:
	CDlgMaterialEdit(CAPSModel * pModel, CWnd* pParent = NULL);	// 标准构造函数
	~CDlgMaterialEdit();
// 对话框数据
	enum { IDD = IDD_WINDCHILL_MATERIALEDIT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	// 生成的消息映射函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNMDblclkWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusWindchillEditlist();
	afx_msg void OnNMClickWindchillMaterualtype(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void PBOMQureyDlg();
	afx_msg void MaterialQueryDlg();
	afx_msg void AddMaterialDlg();
	afx_msg void Calculate();
	afx_msg void Import();
	afx_msg void Upload();
	afx_msg void SaveDlg();
	
	afx_msg void OnCbnKillfocusCombo1();
	afx_msg void OnTvnSelchangedWindchillMaterualtype(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndScrollWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	
	DECLARE_MESSAGE_MAP()

	void InitPartList();
	void InitMaterialtypeTree();
	void SelectPartition();
	void SelectItem(CString item);
	void ModifyPartInfo(CString str);
	bool saveXML(CString path);
private:
	HICON m_hIcon;

	CAPSModel * m_pModel;
	CListCtrl m_Partlist;			//list控件
	CTreeCtrl m_materialtypeTree;	//控件
	CEdit m_editlist;				//编辑控件
	int m_row;						//当前选中的行
	int m_col;						//当前选中的列
	
	CString pBomNumber;
	CString pBomName;
	TiXmlDocument m_pPBOMxml;			//webserverce返回的xml

	TiXmlDocument MaterialEditXml;  //webserverce返回的xml
	TiXmlDocument PBOMXml;	//webserverce返回的xml
	
	TiXmlDocument AddMaterialXml;	//webserverce返回的xml

	std::vector<PartInfo> partInfo;		//将part放入其内
	
	std::vector<int> Relation;

	CString m_path;
	CImageList imageList;

	CWebServiceInterface m_WebServiceInterface;
	CFTPInterface m_FTPInterface;
	std::vector<CString > MaterialType;
public:
	CComboBox m_ComboBox;
	bool isSaved;
};
