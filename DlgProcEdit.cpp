// DlgProcEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EasySize.h"
#include "KmZipUtil.h"
#include "Common.h"

#include "WebServiceInterface.h"
#include "tinyxml.h"
#include "WindChill.h"
#include "DlgProcEdit.h"

#include "WindChillSetting.h"
#include "UtilityFunction.h"
#include "WindChillXml.h"
#define MSG_APS_RESTORE_AUTOSAVE  WM_USER+2071
// CDlgProcEdit �Ի���


extern std::vector<std::string> split(std::string str, std::string pattern);
IMPLEMENT_DYNAMIC(CDlgProcEdit, CDialog)

CDlgProcEdit::CDlgProcEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcEdit::IDD, pParent)
{

}


BOOL CDlgProcEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	InitList();
	INIT_EASYSIZE;
	return TRUE;
}


BOOL CDlgProcEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN &&pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN &&pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CDlgProcEdit::InitList()
{
	DWORD_PTR dwStyle = m_Resultlist.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_Resultlist.SetExtendedStyle(dwStyle);

	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting)
	{
		for (int i = 0; i < pSetting->m_strQueryMatch.GetCount(); i++)
		{
			CString strTmp = pSetting->m_strQueryMatch.GetAt(i).m_strName;
			if (!strTmp.IsEmpty())
			{
				m_Resultlist.InsertColumn(i, strTmp, LVCFMT_LEFT, 0);
				m_Resultlist.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
			}
		}
	}
	m_Resultlist.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}


CDlgProcEdit::~CDlgProcEdit()
{
}

void CDlgProcEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROC_RESULTLIST, m_Resultlist);
}


BEGIN_MESSAGE_MAP(CDlgProcEdit, CDialog)
	ON_BN_CLICKED(ID_PROCSELECT, OnQuery)
	ON_NOTIFY(NM_DBLCLK, IDC_PROC_RESULTLIST, OnNMDblclk)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(ID_PROCOK, &CDlgProcEdit::OnBnClickedProcok)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CDlgProcEdit)
	EASYSIZE(IDC_PROCSELECTRESULT, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_PROCEDIT_STATIC, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_PROC_RESULTLIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

// CDlgProcEdit ��Ϣ�������


void CDlgProcEdit::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

void CDlgProcEdit::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	static CRect rect;
	if (rect.Width() == 0 && rect.Height() == 0)
	{
		GetWindowRect(&rect);
	}
	EASYSIZE_MINSIZE(rect.Width(), rect.Height(), fwSide, pRect);
}


void CDlgProcEdit::OnQuery()
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting == NULL)
		return;

	m_Resultlist.DeleteAllItems();
	CString Designedit;
	auto pwhd =((CEdit *)GetDlgItem(IDC_PROCEDIT));
	if (pwhd ==NULL)
	{
		return;
	}

	pwhd->GetWindowText(Designedit);
	if (Designedit != "")
	{
		CString operType = "4";
		CString docNumbe = Designedit;
		CString Xmlcontent = m_WebServiceInterface.getDocInfo(operType, docNumbe);
		TiXmlDocument doc;
		doc.Parse(Xmlcontent.GetBuffer());
		Xmlcontent.ReleaseBuffer();

		TiXmlElement *root = doc.FirstChildElement();
		if (root == NULL || root->FirstChildElement() == NULL)
		{
			MessageBox("������Ϣ����", APS_MSGBOX_TITIE, MB_OK);
			doc.Clear();
			return;
		}

		auto elem = root->FirstChildElement()->FirstChildElement();
		int row = -1, col;
		while (elem != NULL)
		{
			auto PartContent = elem->FirstChildElement();
			++row;
			m_Resultlist.InsertItem(row, _T(""));
			col = -1;

			while (PartContent != NULL)
			{
				auto key = PartContent->Value();
				auto word = PartContent->GetText();

				CString strValue = _T("");
				bool bFind = KmWindChillCommon::FindPropDataInfo(pSetting->m_strQueryMatch, key, strValue);
				if (!bFind)
				{
					PartContent = PartContent->NextSiblingElement();
					continue;
				}
				else
				{
					col = KmWindChillCommon::FindPropDataIndex(pSetting->m_strQueryMatch, key);
					if (col != -1)
						m_Resultlist.SetItemText(row, col, word);
				}
				PartContent = PartContent->NextSiblingElement();
			}

			elem = elem->NextSiblingElement();
		}
		m_Resultlist.SetRedraw(FALSE);
		CHeaderCtrl *pheader =m_Resultlist.GetHeaderCtrl();
		auto colcount =pheader->GetItemCount();
		for (int i=0;i<colcount;++i)
		{
			m_Resultlist.SetColumnWidth(i,LVSCW_AUTOSIZE);
			auto colwidth =m_Resultlist.GetColumnWidth(i);
			m_Resultlist.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
			auto headwidth =m_Resultlist.GetColumnWidth(i);
			m_Resultlist.SetColumnWidth(i,max(colwidth,headwidth));
		}
		m_Resultlist.SetRedraw(TRUE);
		doc.Clear();
	}
}

void CDlgProcEdit::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	auto row = m_Resultlist.GetNextItem(-1, LVIS_SELECTED);
	
	CString  operType = "4";
	auto index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strQueryMatch, "���");
	if (index == -1||row ==-1)
		return ;

	CString docNumber = m_Resultlist.GetItemText(row, index);
	CWindChillSetting::m_strpartFirstName= docNumber.Right(docNumber.GetLength()-docNumber.FindOneOf("_")-1);

	CString Xmlcontent = m_WebServiceInterface.getDocInfoOfContent(operType, docNumber);
	if (Xmlcontent.IsEmpty())
	{
		return;
	}

	auto path = CWindChillXml::GetPath(Xmlcontent);
	//����ѹ��������  ��Ҫ��ftp����
	
	//CWindChillSetting::m_strpartFirstName = path;
	CString strFTPPath, strFTPName;
	//�����ļ���·��
	strFTPPath = "/downloadContent/";
	strFTPName = path;
	

	TCHAR TempDir[1024];
	GetTempPath(1024, TempDir);
	CString sTempDir(TempDir);
	sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");
	CString strlocal = sTempDir + strFTPName;
	if (!ExistDir(sTempDir))
	{
		if (!::CreateDirectory(sTempDir, NULL))
		{
		}
	}
	if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
	{
		MessageBox("ftp����ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);

		return;
	}

	BOOL bSucc = m_FTPInterface.DownLoad(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
	if (!bSucc)
	{
		CString strErrMsg = _T("�����ļ�ʧ�ܣ�");
		//strErrMsg.Format(_T("�����ļ���%s��ʧ�ܣ�"), strFTPName);
		MessageBox(strErrMsg, APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	if (strFTPName.Find(_T(".zip")) >= 0)  //���ݵ�ѹ���ļ���
	{
		int nP = strFTPName.ReverseFind(_T('.'));
		CString strDir = sTempDir + strFTPName.Left(nP) + "\\";  //ѹ���ļ���ѹ����ļ���
		KmZipUtil kmZipUtil;
		if (kmZipUtil.OpenZipFile(strlocal, KmZipUtil::ZipOpenForUncompress))
		{
			if (!ExistDir(strDir))
			{
				if (!::CreateDirectory(strDir, NULL))
				{
				}
			}
			kmZipUtil.UnZipAll(strDir);
			kmZipUtil.CloseZipFile();
			//DeleteFile(strlocal);
			CFileFind find;

			auto  changexmldir= strDir + GetMainFileName(path) + _T("\\");
			auto isFind = find.FindFile(changexmldir + _T("*.aofo"));
			if (isFind)    //�и��ĵ����߸���������ȡ���ĵ���Ϣ
			{
				isFind = find.FindNextFile();
				auto  changexmlpath = find.GetFilePath();
				CWindChillXml::ParesrChangexml(changexmlpath);

			}
			auto kmapxdir = strDir+GetMainFileName(path)+_T("\\");
			isFind = find.FindFile(kmapxdir + _T("*.kmapx"));
			int i = 0;
			if (isFind)
			{
				isFind = find.FindNextFile();
				auto kmapxpath = find.GetFilePath();
				if (GetParent())
				{
					::SendMessage(GetParent()->GetSafeHwnd(), MSG_APS_RESTORE_AUTOSAVE, (WPARAM)0,
						(LPARAM)kmapxpath.GetBuffer());

					
						CDialog::OnOK();
						return;
				}
				else
				{
					MessageBox("���PBOMʧ��!", APS_MSGBOX_TITIE, MB_OK);
					return;
				}
			}
			else
			{
				MessageBox("������Ϣ����", APS_MSGBOX_TITIE, MB_OK);
				return;
			}
		}
		else
		{
			MessageBox("��ѹʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
			return;
		}
	}


	//�Ƿ�ɾ��ftp�ļ�
#ifdef	DeleteFTPFILE
	if (bSucc)
	{
		bSucc = m_FTPInterface.Delete(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
		if (!bSucc)
		{
			CString strErrMsg = _T("ɾ��FTP�ļ�ʧ�ܣ�");
			MessageBox(strErrMsg, APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
		}
	}
#endif

	*pResult = 0;
	return;
}


BOOL CDlgProcEdit::GetDataFromWebService(int nRow)
{
	return FALSE;
}


void CDlgProcEdit::OnBnClickedProcok()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	auto row = m_Resultlist.GetNextItem(-1, LVIS_SELECTED);

	CString  operType = "4";
	auto index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strQueryMatch, "���");
	if (index == -1 || row == -1)
		return;

	CString docNumbe = m_Resultlist.GetItemText(row, index);
	CString Xmlcontent = m_WebServiceInterface.getDocInfoOfContent(operType, docNumbe);
	if (Xmlcontent.IsEmpty())
	{
		return;
	}

	auto path = CWindChillXml::GetPath(Xmlcontent);
	//����ѹ��������  ��Ҫ��ftp����


	CString strFTPPath, strFTPName;
	//�����ļ���·��
	strFTPPath = "/downloadContent/";
	strFTPName = path;


	TCHAR TempDir[1024];
	GetTempPath(1024, TempDir);
	CString sTempDir(TempDir);
	sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");
	CString strlocal = sTempDir + strFTPName;
	if (!ExistDir(sTempDir))
	{
		if (!::CreateDirectory(sTempDir, NULL))
		{
		}
	}
	if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
	{
		MessageBox("ftp����ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);

		return;
	}

	BOOL bSucc = m_FTPInterface.DownLoad(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
	if (!bSucc)
	{
		CString strErrMsg = _T("�����ļ�ʧ�ܣ�");
		//strErrMsg.Format(_T("�����ļ���%s��ʧ�ܣ�"), strFTPName);
		MessageBox(strErrMsg, APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	if (strFTPName.Find(_T(".zip")) >= 0)  //���ݵ�ѹ���ļ���
	{
		int nP = strFTPName.ReverseFind(_T('.'));
		CString strDir = sTempDir + strFTPName.Left(nP) + "\\";  //ѹ���ļ���ѹ����ļ���
		KmZipUtil kmZipUtil;
		if (kmZipUtil.OpenZipFile(strlocal, KmZipUtil::ZipOpenForUncompress))
		{
			if (!ExistDir(strDir))
			{
				if (!::CreateDirectory(strDir, NULL))
				{
				}
			}
			kmZipUtil.UnZipAll(strDir);
			kmZipUtil.CloseZipFile();
			//DeleteFile(strlocal);
			CFileFind find;
			auto kmapxdir = strDir + GetMainFileName(path) + _T("\\");
			auto isFind = find.FindFile(kmapxdir + _T("*.kmapx"));
			int i = 0;
			if (isFind)
			{
				isFind = find.FindNextFile();
				auto kmapxpath = find.GetFilePath();
				if (GetParent())
				{
					::SendMessage(GetParent()->GetSafeHwnd(), MSG_APS_RESTORE_AUTOSAVE, (WPARAM)0,
						(LPARAM)kmapxpath.GetBuffer());
					CDialog::OnOK();
					return;
				}
				else
				{
					MessageBox("���PBOMʧ��!", APS_MSGBOX_TITIE, MB_OK);
					return;
				}
			}
			else
			{
				MessageBox("������Ϣ����", APS_MSGBOX_TITIE, MB_OK);
				return;
			}
		}
		else
		{
			MessageBox("��ѹʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
			return;
		}
	}

	//�Ƿ�ɾ��ftp�ļ�
#ifdef	DeleteFTPFILE
	if (bSucc)
	{
		bSucc = m_FTPInterface.Delete(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
		if (!bSucc)
		{
			CString strErrMsg = _T("ɾ��FTP�ļ�ʧ�ܣ�");
			MessageBox(strErrMsg, APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
	
		}
	}
#endif
	return;
}
