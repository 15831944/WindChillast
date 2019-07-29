#pragma once


// CDlgPDFExport 对话框

class CDlgPDFExport : public CDialog
{
	DECLARE_DYNAMIC(CDlgPDFExport)

public:
	CDlgPDFExport(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPDFExport();

// 对话框数据
	enum { IDD = IDD_WINDCHILL_PDFEXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedWINDCHILLProcruleBtnPdfpath();
	afx_msg void OnBnClickedLast();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void setFilePath(CString sPath);
	int getMode();
	void getOutputFilePath(CString &sOutputFilePath);

private:
	CString m_sPath;
	CString m_strPath;
	int m_iMode;	//0:取消  1:完成  2:上一步
	CString m_sOutputFilePath;
};
