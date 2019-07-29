#pragma once


// CDlgPDFExport �Ի���

class CDlgPDFExport : public CDialog
{
	DECLARE_DYNAMIC(CDlgPDFExport)

public:
	CDlgPDFExport(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPDFExport();

// �Ի�������
	enum { IDD = IDD_WINDCHILL_PDFEXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	int m_iMode;	//0:ȡ��  1:���  2:��һ��
	CString m_sOutputFilePath;
};
