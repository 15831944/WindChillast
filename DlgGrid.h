#pragma once
#include "afxcmn.h"

// CDlgGrid 对话框
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
	CDlgGrid(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgGrid();

// 对话框数据

	enum { IDD = IDD_WINDCHILL_GRID };
	void InitPage(ConfigGrid *config, std::vector<GridData*>data);

	void InitListCtrl();
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
