
// windchillDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "tinyxml.h"
#include "Partinfo.h"
#include "EasySize.h"
#include "Resource.h"
#include "WebServiceInterface.h"
// CDlgMaterialEdit �Ի���
class CDlgMaterialEdit : public CDialog
{
	DECLARE_EASYSIZE
// ����
public:
	CDlgMaterialEdit(CAPSModel * pModel, CWnd* pParent = NULL);	// ��׼���캯��
	~CDlgMaterialEdit();
// �Ի�������
	enum { IDD = IDD_WINDCHILL_MATERIALEDIT };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	// ���ɵ���Ϣӳ�亯��
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
	CListCtrl m_Partlist;			//list�ؼ�
	CTreeCtrl m_materialtypeTree;	//�ؼ�
	CEdit m_editlist;				//�༭�ؼ�
	int m_row;						//��ǰѡ�е���
	int m_col;						//��ǰѡ�е���
	
	CString pBomNumber;
	CString pBomName;
	TiXmlDocument m_pPBOMxml;			//webserverce���ص�xml

	TiXmlDocument MaterialEditXml;  //webserverce���ص�xml
	TiXmlDocument PBOMXml;	//webserverce���ص�xml
	
	TiXmlDocument AddMaterialXml;	//webserverce���ص�xml

	std::vector<PartInfo> partInfo;		//��part��������
	
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
