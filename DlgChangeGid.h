#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "Resource.h"
#include "PDFData.h"

#include "PDFConfig.h"
#include "BCGPGridCtrl.h "

// CDlgChangeGid �Ի���

class CDlgChangeGid : public CDialog
{
	DECLARE_DYNAMIC(CDlgChangeGid)
public:
	CDlgChangeGid(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgChangeGid();

// �Ի�������
	void InitListCtrl();
	enum { IDD = IDD_WINDCHILL_CHANGGRID };
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	bool ISChangeGrid(GridData *data);
	void InitPage(ConfigPage *config, std::vector<PageData*> data);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnNMDblclkWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedWindchillAddrow();
	afx_msg void OnBnClickedWindchillDelrow();

	afx_msg LRESULT OnGridtemChanged(WPARAM uProListId, LPARAM pProp);

public:
	ConfigPage *m_config;
	std::vector<PageData*> m_data;
private:
	int m_col;
	int m_row;

private:
	
	CBCGPGridCtrl m_BCGGridCtrl;
};
