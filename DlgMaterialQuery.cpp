// CDlgMaterialQuery.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "DlgMaterialQuery.h"
#include "WindChillSetting.h"
#include "EasySize.h"
#include "KmZipUtil.h"
#include "UtilityFunction.h"
#include "WindChillXml.h"

using namespace std;
// CDlgMaterialQuery �Ի���
extern vector<string> split(string str, string pattern);

IMPLEMENT_DYNAMIC(CDlgMaterialQuery, CDialog)

CDlgMaterialQuery::CDlgMaterialQuery(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMaterialQuery::IDD, pParent)
{
	partNumber ="";
}

CDlgMaterialQuery::~CDlgMaterialQuery()
{
}

bool WcharFromChar(unsigned int CodePage, const char* szText, std::wstring & wstrVal)
{
	WCHAR *lpwszVal(NULL);
	int nLength = 0;
	size_t dwSize = 0;
	nLength = ::MultiByteToWideChar(CodePage, 0, szText, -1, NULL, 0);
	if (nLength <= 0)
	{
		return false;
	}
	dwSize = nLength * sizeof(WCHAR);
	lpwszVal = (WCHAR *)malloc(dwSize);
	if (lpwszVal == NULL) return false;
	memset(lpwszVal, 0x0, dwSize);
	nLength = ::MultiByteToWideChar(CodePage, 0, szText, -1, lpwszVal, nLength);
	if (nLength <= 0)
	{
		free(lpwszVal);
		lpwszVal = NULL;
		return false;
	}
	wstrVal.assign(lpwszVal);
	free(lpwszVal);
	lpwszVal = NULL;

	return true;
}


bool WFromUTF8(const char* szText, std::wstring &wstrVal)
{
	return WcharFromChar(CP_UTF8, szText, wstrVal);
}


BEGIN_MESSAGE_MAP(CDlgMaterialQuery, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_MATERIALEDITSELECT_RESULTLIST, OnNMDblclk)
	ON_BN_CLICKED(IDC_MATERIALEDITSELECT_SELECT, OnQuery)
	ON_BN_CLICKED(IDC_MATERIALEDITSELECT_OK, OnOK)
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CDlgMaterialQuery)
	EASYSIZE(IDC_MATERIALEDITSELECT_SELECT, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_MATERIALEDITSELECT_OK, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_MATERIALEDITSELECT_RESULTLIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)

	EASYSIZE(IDC_MATERIALEDITSELECT_STATIC, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_MATERIALEDITSELECT_STATICRESULT, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP
// CDlgMaterialQuery ��Ϣ�������


BOOL CDlgMaterialQuery::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	InitList();
	m_row = -1;
	m_col = -1;
	INIT_EASYSIZE;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDlgMaterialQuery::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MATERIALEDITSELECT_RESULTLIST, m_Resultlist);
}


void CDlgMaterialQuery::InitList()
{
	DWORD_PTR dwStyle = m_Resultlist.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_Resultlist.SetExtendedStyle(dwStyle);

	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting)
	{
		for (int i = 0;i < pSetting->m_strQueryMatch.GetCount();i++)
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

void CDlgMaterialQuery::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;

	OnOK();
}

BOOL CDlgMaterialQuery::PreTranslateMessage(MSG* pMsg)
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

void CDlgMaterialQuery::OnQuery()
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting == NULL)
		return;

	m_Resultlist.DeleteAllItems();
	CString Designedit;
	((CEdit *)GetDlgItem(IDC_MATERIALEDITSELECT_EDIT))->GetWindowText(Designedit);

	if (Designedit != "" ) 
	{
#ifdef DEV_TMP
		AfxMessageBox(_T("����webserver PBOMģ����ѯ�ӿ�"));
#endif

		CString operType = "1";
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
					if (col!=-1)
					{
						m_Resultlist.SetItemText(row, col, word);
					}
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

void CDlgMaterialQuery::OnOK()
{

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
#if 1 

	auto row = m_Resultlist.GetNextItem(-1, LVIS_SELECTED);
	auto index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strQueryMatch, "���");
	
	if (row != -1)
	{
		CString operType = "1";
		CString docNumber = m_Resultlist.GetItemText(row,index);
		partNumber =docNumber.Right(docNumber.GetLength()-docNumber.FindOneOf("_")-1);
		CString Xmlcontent =m_WebServiceInterface.getDocInfoOfContent(operType,docNumber);
		auto path = CWindChillXml::GetPath(Xmlcontent);
		
		CWindChillSetting::m_strpartFirstName =partNumber;

		if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
		{
			MessageBox("ftp����ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
			return;
		}

		CString strFTPPath, strFTPName/*, strModelName*/;
		
		strFTPPath ="/downloadContent/";
		strFTPName = path;
		//strModelName = "SubAss.CATProduct";
		TCHAR TempDir[1024];
		GetTempPath(1024, TempDir);
		CString sTempDir(TempDir);
		sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");
		if (!ExistDir(sTempDir))
		{
			CreateDirectory(sTempDir,NULL);
		}

		CString strlocal = sTempDir + strFTPName;
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
				strDir = strDir + GetMainFileName(path)+"\\";
				auto isFind =find.FindFile(strDir+_T("*.xml"));
				while(isFind)
				{
					isFind =find.FindNextFile();
					if (find.IsDots())
					{
						continue;
					}
					else
					{
						CString fullpath =strDir+find.GetFileName();
						auto name =GetMainFileName(fullpath);    
						if (name.CompareNoCase("Pbom") ==0)    
						{
							CFile file;
							file.Open(fullpath,CFile::modeRead);
							
							
							auto dwFilelen=file.GetLength();
							char *buf;
							buf =new char[dwFilelen+1];
							buf[dwFilelen] =0;
							
							file.Read(buf,dwFilelen);
							
							file.Close();
							std::wstring a;

							WFromUTF8(buf, a);
							auto dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
							char * sabstr;
							sabstr = new char[dwNum +1];
							WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);
							//AfxMessageBox(sabstr);
							CString s =sabstr;
							auto n=s.FindOneOf(">")+1;
							auto tmp =s.Right(s.GetLength()-n);
							if (!PBOMxml.Parse(tmp.GetBuffer()))
							{
								continue;
							}	
						
							delete []buf;
							delete []sabstr;
						}
						else
						{
							CFile file;
							file.Open(fullpath,CFile::modeRead);

							auto dwFilelen=file.GetLength();
							char *buf;
							buf =new char[dwFilelen+1];
							buf[dwFilelen] =0;
							file.Read(buf,dwFilelen);
							file.Close();
							
							if (!Materialxml.Parse(buf))
							{
								continue;
							}	
							
							delete []buf;
						}
						fullpath.ReleaseBuffer();

					}
				}
			}
			else
			{
				MessageBox("��ѹʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
				return;
			}
		}
		
		CDialog::OnOK();
	}
#endif
}

void CDlgMaterialQuery::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	UPDATE_EASYSIZE;
}

void CDlgMaterialQuery::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO:  �ڴ˴������Ϣ����������
	static CRect rect;
	if (rect.Width() == 0 && rect.Height() == 0)
	{
		GetWindowRect(&rect);
	}

	EASYSIZE_MINSIZE(rect.Width(), rect.Height(), fwSide, pRect);
}

