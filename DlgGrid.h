#pragma once
#include "afxcmn.h"

// CDlgGrid �Ի���
#include "PDFData.h"

#include "PDFConfig.h"
#include "afxwin.h"

struct ListData
{
	RowData * ParentConfig;
	RowData * ChildConfig;
};

class CDlgGrid : public CDialog
{
	DECLARE_DYNAMIC(CDlgGrid)

public:
	CDlgGrid(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgGrid();

// �Ի�������

	enum { IDD = IDD_WINDCHILL_GRID };
	void InitPage(ConfigGrid *config, std::vector<GridData*>data);

	void InitListCtrl();
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	afx_msg void OnEnKillfocusWindchillEditlist();
	afx_msg void OnNMDblclkWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_GridListCtrl;

private:
	ConfigGrid *m_config;
	std::vector<GridData*>m_data;
private:
	int m_col;
	int m_row;

private:
	int m_GridConfigNum;
	int m_ChildGridConfigNum;

public:
	CEdit m_EditList;
};
