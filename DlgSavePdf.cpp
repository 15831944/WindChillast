// DlgSavePdf.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"

#include "WindChill.h"
#include "DlgSavePdf.h"
#include "UtilityFunction.h"
#include "DirectoryDialog.h"

// CDlgSavePdf �Ի���

IMPLEMENT_DYNAMIC(CDlgSavePdf, CDialog)

CDlgSavePdf::CDlgSavePdf(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSavePdf::IDD, pParent)
{
	m_strPath = "";
	kmapxpath = "";
	pdfpath = "";
}

CDlgSavePdf::~CDlgSavePdf()
{

}

void CDlgSavePdf::SetPath(CString path)
{
	kmapxpath = path;
}

BOOL CDlgSavePdf::OnInitDialog()
{
	CDialog::OnInitDialog();
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PDFEDIT_KMAPXPATH);
	if (pEdit == NULL)
	{
		return FALSE;
	}
	/*char pBuf[MAX_PATH];

	SHGetSpecialFolderPath(this->GetSafeHwnd(), pBuf, CSIDL_PERSONAL, 0);
	CString path = CString(pBuf) + "\\1.xml";*/
	pEdit->SetWindowText(kmapxpath);
	pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PDFEDIT_PDFPATH);
	if (pEdit == NULL)
	{
		return FALSE;
	}
	CString path = GetFilePath(kmapxpath);
	CString name = GetMainFileName(kmapxpath);
	pdfpath = path + name + ".pdf";
	pEdit->SetWindowText(pdfpath);

	return TRUE;
}


void CDlgSavePdf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSavePdf, CDialog)
	ON_BN_CLICKED(IDC_AWINDCHILL_BUTTON_KMAPXPATH, &CDlgSavePdf::OnBnClickedAwindchillButtonKmapxpath)
	ON_BN_CLICKED(IDC_AWINDCHILL_BUTTON_PDFPATH, &CDlgSavePdf::OnBnClickedAwindchillButtonPdfpath)
	ON_BN_CLICKED(IDC_OK, &CDlgSavePdf::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_PDF_CANCEL, &CDlgSavePdf::OnBnClickedButtonPdfCancel)
END_MESSAGE_MAP()


// CDlgSavePdf ��Ϣ�������


void CDlgSavePdf::OnBnClickedAwindchillButtonKmapxpath()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PDFEDIT_KMAPXPATH);
	if (pEdit ==NULL)
	{
		return;
	}

	CString strPathName;
	pEdit->GetWindowText(strPathName);
	
	CString path = GetFilePath(strPathName);
	CFileDialog FileDalog(TRUE, _T("kmapx"), NULL, OFN_READONLY, _T("(*.kmapx)|*.kmapx|�����ļ�(*.*)|*.*|"), this);
	FileDalog.m_ofn.lpstrInitialDir = path;
	if (FileDalog.DoModal() != IDOK)
	{
		return;
	}

	CString File = FileDalog.GetPathName();
	kmapxpath = File;
	/*CDirectoryDialog dirDlg(GetSafeHwnd());
	auto strPath = dirDlg.ShowDirectoryTree();

	if (strPath != "")
	{
		CString sName = strPath + "1.xml";
		pEdit->SetWindowText(sName);
		m_sName = sName;
	}*/
	pEdit->SetWindowText(kmapxpath);
}


void CDlgSavePdf::OnBnClickedAwindchillButtonPdfpath()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PDFEDIT_PDFPATH);
	if (!pEdit)
	{
		return;
	}

	CString strPathName;
	pEdit->GetWindowText(strPathName);
	CString path = GetFilePath(strPathName);
	CFileDialog FileDalog(TRUE, _T("pdf"), NULL, OFN_READONLY, _T("(*.pdf)|*.pdf|�����ļ�(*.*)|*.*|"), this);
	FileDalog.m_ofn.lpstrInitialDir = path;
	
	if (FileDalog.DoModal() != IDOK)
	{
		return;
	}

	CString File = FileDalog.GetPathName();
	pdfpath = File;
	/*CDirectoryDialog dirDlg(GetSafeHwnd());
	auto strPath = dirDlg.ShowDirectoryTree();

	if (strPath != "")
	{
		CString sName = strPath + "1.xml";
		pEdit->SetWindowText(sName);
		m_sName = sName;
	}*/
	
	pEdit->SetWindowText(pdfpath);
}


void CDlgSavePdf::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	EndDialog(IDOK);
}


void CDlgSavePdf::OnBnClickedButtonPdfCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	EndDialog(IDCANCEL);
}
