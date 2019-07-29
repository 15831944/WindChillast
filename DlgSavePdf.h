#pragma once
// CDlgSavePdf 对话框

class CDlgSavePdf : public CDialog
{
	DECLARE_DYNAMIC(CDlgSavePdf)

public:
	CDlgSavePdf(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSavePdf();

// 对话框数据
	enum { IDD = IDD_WINDCHILL_CHICKIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	void SetPath(CString path);
	CString GetpdfPath() { return pdfpath; };
	CString GetkmapxPath() { return kmapxpath; };
private:
	CString m_strPath;
	CString kmapxpath;
	CString pdfpath;
public:
	afx_msg void OnBnClickedAwindchillButtonKmapxpath();
	afx_msg void OnBnClickedAwindchillButtonPdfpath();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonPdfCancel();
};
