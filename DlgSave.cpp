// CDlgSave.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "DlgSave.h"


// CDlgSave �Ի���

IMPLEMENT_DYNAMIC(CDlgSave, CDialog)

CDlgSave::CDlgSave(CString Number,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSave::IDD, pParent)
{
	partNumber=Number;
}

CDlgSave::~CDlgSave()
{
}

void CDlgSave::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CDlgSave::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PDFEDIT_FILEPATH);
	char pBuf[MAX_PATH];
	//begin add by xjt in 2018.11.22 for 70561
	SHGetSpecialFolderPath(this->GetSafeHwnd(), pBuf, CSIDL_PERSONAL, 0);
	//end add by xjt in 2018.11.22 for 70561
	//GetCurrentDirectory(MAX_PATH, pBuf);
	CString path = CString(pBuf) + "\\CLXHDEB_"+partNumber+".xml";
	pEdit->SetWindowText(path);

	SetWindowText(m_Caption);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgSave, CDialog)
	ON_BN_CLICKED(IDC_AWINDCHILL_BUTTON_PDFPATH, &CDlgSave::OnBnClickedAwindchillButtonPdfpath)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_BUTTON_PDF_CANCEL, OnCancel)
END_MESSAGE_MAP()


// CDlgSave ��Ϣ�������


void CDlgSave::OnBnClickedAwindchillButtonPdfpath()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�Ȼ�ȡ�ļ���
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PDFEDIT_FILEPATH);

	CString strPathName;
	pEdit->GetWindowText(strPathName);

	CDirectoryDialog dirDlg(GetSafeHwnd());
	auto strPath = dirDlg.ShowDirectoryTree();

	if (strPath !="")
	{
		CString sName = strPath + "CLXHDEB_"+partNumber+".xml";
		pEdit->SetWindowText(sName);
		m_sName = sName;
	}
}


void CDlgSave::OnOk()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_WINDCHILL_PDFEDIT_FILEPATH);
	pEdit->GetWindowText(m_sName);

	EndDialog(IDOK);
}

void CDlgSave::SetCaption(CString Caption)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_Caption = Caption;
}

void CDlgSave::OnCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	EndDialog(IDCANCEL);
}
