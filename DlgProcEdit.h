#pragma once


// CDlgProcEdit �Ի���

class CDlgProcEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcEdit)
	DECLARE_EASYSIZE
public:
	CDlgProcEdit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcEdit();

// �Ի�������
	enum { IDD = IDD_WINDCHILL_PROCEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void InitList();
	virtual BOOL OnInitDialog();

	BOOL GetDataFromWebService(int nRow);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnQuery();
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);

private:
	CListCtrl m_Resultlist;
	HICON m_hIcon;
	CWebServiceInterface m_WebServiceInterface;
	CFTPInterface m_FTPInterface;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedProcok();
};
