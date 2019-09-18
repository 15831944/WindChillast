#pragma once
#include "Resource.h"
#include <afxwin.h>
#include <map>
#include <afxcmn.h>
#include "Output3DPdf.h"

//#include "EditListCtrl.h"

#include "DlgInfo.h"
#include "DlgGrid.h"
#include "DlgChangeGid.h"

class CDlgCollect :
	public CDialog
{
public:
	CDlgCollect(IAPSApplication* pApplication,CWnd* pParent = NULL);
	virtual ~CDlgCollect();

	// �Ի�������
	enum {IDD = IDD_WINDCHILL_COLLECT};
public:
	void InitCombox();
	void InitTab();
	void GetPdfInfo();

	void GetPdfInfoFromXml();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	std::map<int,CString> griddata; 
private:
	CComboBox m_combox;
	std::map<CString, CString> m_Type;
public:
	//tab�ؼ��� ����λ�þ���˿ؼ���ͬ
	CTabCtrl m_TabCtrl;
	IAPSApplication* m_pApplication;
	COutPut3DPdf *m_putpdf;

	std::map<CString, std::vector<PageData *> > m_PageData;
	std::map<CString, ConfigGroup*> m_PageConfig;
private:
	//��ǰѡ��Ի���
	std::map<int, CDlgGrid *> m_dlgGrid;
	CDlgInfo m_dlgInfo;
	CDlgChangeGid m_dlgChangeGrid;

	CString path;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnSelchangeWindchillCombox();
	afx_msg void OnTcnSelchangeWindchillTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedWindchillCollectSave();
	afx_msg void OnBnClickedWindchillCollectCollect();
};

