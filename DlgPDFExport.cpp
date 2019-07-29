// DlgPDFExport.cpp : 实现文件
//

#include "stdafx.h"
#include "Windchill.h"
#include "DlgPDFExport.h"
#include "DirectoryDialog.h"
#include "UtilityFunction.h"
#include "WindchillSetting.h"
#include "common.h"
// CDlgPDFExport 对话框

IMPLEMENT_DYNAMIC(CDlgPDFExport, CDialog)

CDlgPDFExport::CDlgPDFExport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPDFExport::IDD, pParent)
{
	m_sPath = "";
	m_strPath = "";
	m_iMode = -1;
	m_sOutputFilePath = "";
}

CDlgPDFExport::~CDlgPDFExport()
{
}

void CDlgPDFExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPDFExport, CDialog)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_PDFPATH, &CDlgPDFExport::OnBnClickedWINDCHILLProcruleBtnPdfpath)
	ON_BN_CLICKED(IDLAST, &CDlgPDFExport::OnBnClickedLast)
	ON_BN_CLICKED(IDOK, &CDlgPDFExport::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgPDFExport::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CDlgPDFExport::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect parrect;
	GetClientRect(&parrect);
	//输出pdf文件框的位置
	CRect rcStaticText(10, parrect.bottom - 70, 66, parrect.bottom - 48);
	CWnd* pStaticText = GetDlgItem(IDC_WINDCHILL_PROCRULE_STATIC_TEXT);
	pStaticText->MoveWindow(rcStaticText);
	pStaticText->Invalidate();
	CRect rcText(70, parrect.bottom - 72, parrect.right - 50, parrect.bottom - 50);
	CWnd* pText = GetDlgItem(IDC_WINDCHILL_PROCRULE_OUTPUTPATH);
	pText->MoveWindow(rcText);
	pText->Invalidate();
	CRect rcPath(parrect.right - 37, parrect.bottom - 72, parrect.right - 15, parrect.bottom - 50);
	CWnd* pPath = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_PDFPATH);
	pPath->MoveWindow(rcPath);
	pPath->Invalidate();

	//上一步，完成，取消按钮的位置
	CRect rcLast(parrect.Width() - 300, parrect.bottom - 42, parrect.Width() - 250, parrect.bottom - 20);
	CWnd* pLast = GetDlgItem(IDLAST);
	pLast->MoveWindow(rcLast);
	pLast->Invalidate();
	CRect rcExt(parrect.Width() - 200, parrect.bottom - 42, parrect.Width() - 150, parrect.bottom - 20);
	CWnd* pExt = GetDlgItem(IDOK);
	pExt->MoveWindow(rcExt);
	pExt->Invalidate();
	CRect rcCancel(parrect.Width() - 100, parrect.bottom - 42, parrect.Width() - 50, parrect.bottom - 20);
	CWnd* pCancel = GetDlgItem(IDCANCEL);
	pCancel->MoveWindow(rcCancel);
	pCancel->Invalidate();

	//设置输出文件框中的默认值
	CString strPath;
	CString strPathTmp = GetFileNamePath(m_sPath);
	if (strPathTmp.GetLength() > 0)
	{
		strPath = strPathTmp;
	}

	if (m_strPath.IsEmpty())
	{
		CString sName;
		if(CWindChillSetting::m_strpartFirstName.IsEmpty())
			sName = GetMainFileName(m_sPath);
		else
		{
			auto temp =CWindChillSetting::m_strpartFirstName;
			sName = changePartName(temp);
		}

		m_strPath = strPath + sName + _T(".pdf");
	}
	CEdit* pEditPath = (CEdit*)GetDlgItem(IDC_WINDCHILL_PROCRULE_OUTPUTPATH);
	pEditPath->SetWindowText(m_strPath);
	
	return TRUE;
}

// CDlgPDFExport 消息处理程序


void CDlgPDFExport::OnBnClickedWINDCHILLProcruleBtnPdfpath()
{
	//先获取文件名
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PROCRULE_OUTPUTPATH);

	CString strPathName;
	pEdit->GetWindowText(strPathName);

	CString strFileName = GetFileName(strPathName);
	CString strPath = GetFileNamePath(strPathName);

	CDirectoryDialog dirDlg(GetSafeHwnd());
	strPath = dirDlg.ShowDirectoryTree(strPath);

	CString sName = strPath + strFileName;
	pEdit->SetWindowText(sName);
}


void CDlgPDFExport::OnBnClickedLast()
{
	m_iMode = 2;
	CDialog::OnCancel();
}


void CDlgPDFExport::OnBnClickedOk()
{
	CEdit* pEditPath = (CEdit*)GetDlgItem(IDC_WINDCHILL_PROCRULE_OUTPUTPATH);
	pEditPath->GetWindowText(m_sOutputFilePath);

	CFileFind finder;
	CString sOutput = m_sOutputFilePath;

	int nPos = sOutput.Find(_T("\\"));
	CString sOutputTemplate;
	while (nPos > 0)
	{
		if (sOutputTemplate.IsEmpty())
			sOutputTemplate = sOutput.Left(nPos);
		else
			sOutputTemplate += _T("\\") + sOutput.Left(nPos);

		sOutput.Replace(sOutput.Left(nPos + 1), "");
		nPos = sOutput.Find(_T("\\"));
	}
	BOOL bWay = finder.FindFile(sOutputTemplate);
	BOOL bRoot = PathIsRoot(sOutputTemplate + "\\");
	nPos = m_sOutputFilePath.Find(_T(".pdf"));
	if (nPos < 0 || (!bWay && !bRoot))
	{
		MessageBox("输出路径错误！",APS_MSGBOX_TITIE,MB_OK);
		return;
	}

	bool bOpenFile = false;
	CFile file;
	file.Open(m_sOutputFilePath, CFile::modeReadWrite);
	if (file.m_hFile != CFile::hFileNull)
	{//文件打开？
		bOpenFile = false;
	}
	else
	{
		bOpenFile = true;
	}
	BOOL bExist = PathFileExists(m_sOutputFilePath);
	if (bExist)
	{
		if (IDNO == MessageBox("当前路径存在同名的文件,是否覆盖?", APS_MSGBOX_TITIE, MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}
		else
		{
			if (bOpenFile)
			{
				MessageBox("文件已打开，请关闭!", APS_MSGBOX_TITIE, MB_ICONWARNING);
				return;
			}
		}
	}

	m_iMode = 1;
	CDialog::OnOK();
}


void CDlgPDFExport::OnBnClickedCancel()
{
	m_iMode = 0;
	CDialog::OnCancel();
}

void CDlgPDFExport::setFilePath(CString sPath)
{
	m_sPath = sPath;
}

int CDlgPDFExport::getMode()
{
	return m_iMode;
}

void CDlgPDFExport::getOutputFilePath(CString &sOutputFilePath)
{
	sOutputFilePath = m_sOutputFilePath;
}