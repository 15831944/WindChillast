#pragma once
// CDlgSavePdf �Ի���

class CDlgSavePdf : public CDialog
{
	DECLARE_DYNAMIC(CDlgSavePdf)

public:
	CDlgSavePdf(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSavePdf();

// �Ի�������
	enum { IDD = IDD_WINDCHILL_CHICKIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
