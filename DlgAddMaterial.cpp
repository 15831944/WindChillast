// CDlgAddMaterial.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "DlgAddMaterial.h"
#include <fstream>
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

using KmWindChillCommon::PropData;
using namespace std;
// CDlgAddMaterial �Ի���

IMPLEMENT_DYNAMIC(CDlgAddMaterial, CDialog)

#define MSG_IMPORT_EQUIES				WM_USER + 2090

BEGIN_MESSAGE_MAP(CDlgAddMaterial, CDialog)
	ON_BN_CLICKED(IDC_SELECTACCESSORY_SELECT, OnQuery)
	ON_NOTIFY(NM_DBLCLK, IDC_SELECTACCESSORY_RESULTLIST, OnNMDblclk)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_ADDMATERIAL_OK, &CDlgAddMaterial::OnBnClickedAddmaterialOk)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CDlgAddMaterial)
	EASYSIZE(IDC_ADDMATERIAL_SELECT, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_ADDMATERIAL_OK, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_SELECTACCESSORY_RESULTLIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_SELECTPBOM_STATIC, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_SELECTPBOM_STATICRESULT, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

CDlgAddMaterial::CDlgAddMaterial(ResourceType type, UINT ID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddMaterial::IDD, pParent)
{
	m_ID = ID;
	m_type = type;
}

CDlgAddMaterial::~CDlgAddMaterial()
{
}

void CDlgAddMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SELECTACCESSORY_RESULTLIST, m_Resultlist);
}

BOOL CDlgAddMaterial::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	InitList();
	INIT_EASYSIZE;

	return TRUE;
}

void CDlgAddMaterial::InitList()
{
	DWORD_PTR dwStyle = m_Resultlist.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_Resultlist.SetExtendedStyle(dwStyle);

	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting)
	{
		if (m_type==ResourceType_EQUIPTOOL)
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
			auto comBox =(CComboBox*)GetDlgItem(IDC_ADDMATERIAL_COMBOX);
			if (comBox)
			{
				comBox->ShowWindow(SW_SHOW);

				CEdit *pEdit = (CEdit *)comBox->GetWindow(GW_CHILD);
				if (pEdit)
				{
					//pEdit->EnableWindow(FALSE);
					pEdit->SetReadOnly(TRUE);
				}

			}

			for (auto i = 0; i < pSetting->m_strResourceTypeMatch.GetCount(); ++i)
			{
				CString strTmp = pSetting->m_strResourceTypeMatch.GetAt(i).m_strName;
				comBox->AddString(strTmp);
			}
			comBox->SetCurSel(0);
		}
		else
		{
			auto comBox = (CComboBox*)GetDlgItem(IDC_ADDMATERIAL_COMBOX);
			if (comBox)
			{
				comBox->ShowWindow(SW_HIDE);
			}
			
			for (int i = 0; i < pSetting->m_strAccessoryMatch.GetCount(); i++)
			{
				CString strTmp = pSetting->m_strAccessoryMatch.GetAt(i).m_strName;
				if (!strTmp.IsEmpty())
				{
					m_Resultlist.InsertColumn(i, strTmp, LVCFMT_LEFT, 0);
					m_Resultlist.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
				}
			}
		}
		
		
	}
	m_Resultlist.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

// CDlgAddMaterial ��Ϣ�������

BOOL CDlgAddMaterial::PreTranslateMessage(MSG* pMsg)
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

void CDlgAddMaterial::OnQuery()
{

	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if(pSetting == NULL)
		return;

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_Resultlist.DeleteAllItems();
	
	CString EditText;
	auto ptr = static_cast<CEdit *>(GetDlgItem(IDC_ADDMATERIAL_EDIT));
	if (ptr)
	{
		ptr->GetWindowText(EditText);
	}
	

	if (EditText!="")
	{
		TiXmlDocument doc;
		CString operType = "1";
		CString resourceNumber = EditText;


		if (ResourceType_ADDMaterial == m_type)
		{
			operType = "1";
			CString XmlContent = m_WebServiceInterface.SearchResource(operType, resourceNumber);
			doc.Parse(XmlContent.GetBuffer());
			XmlContent.ReleaseBuffer();

			TiXmlElement *root = doc.FirstChildElement();
			if (root == NULL)
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
					bool bFind = KmWindChillCommon::FindPropDataInfo(pSetting->m_strAccessoryMatch, key, strValue);
					if (!bFind)
					{
						PartContent = PartContent->NextSiblingElement();
						continue;
					}
					else
					{
						col = KmWindChillCommon::FindPropDataIndex(pSetting->m_strAccessoryMatch, key);
						m_Resultlist.SetItemText(row, col, word);

						auto width =m_Resultlist.GetColumnWidth(col);
						m_Resultlist.SetColumnWidth(col, LVSCW_AUTOSIZE);

						auto width1 = m_Resultlist.GetColumnWidth(col);
						if (width>width1)
						{
							m_Resultlist.SetColumnWidth(col, width);
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
		else
		{
			auto comBox = (CComboBox*)GetDlgItem(IDC_ADDMATERIAL_COMBOX);
			if (comBox)
			{
				auto selitem =comBox->GetCurSel();
				operType =CWindChillSetting::m_strResourceTypeMatch.GetAt(selitem).m_strValue;
			}

			CString XmlContent = m_WebServiceInterface.SearchResource(operType, resourceNumber);
			doc.Parse(XmlContent.GetBuffer());
			XmlContent.ReleaseBuffer();

			TiXmlElement *root = doc.FirstChildElement();
			if (root == NULL)
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
						m_Resultlist.SetItemText(row, col, word);

						auto width =m_Resultlist.GetColumnWidth(col);
						m_Resultlist.SetColumnWidth(col, LVSCW_AUTOSIZE);

						auto width1 = m_Resultlist.GetColumnWidth(col);
						if (width>width1)
						{
							m_Resultlist.SetColumnWidth(col, width);
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
}


void CDlgAddMaterial::ImportEquies(int row)
{
	CString operType  = "5";

	CString resourceNumber = m_Resultlist.GetItemText(row, 0);
	auto  Xmlcontent = m_WebServiceInterface.getDocInfoOfContent(operType, resourceNumber);

	if (Xmlcontent.IsEmpty())
	{
		return;
	}

	auto path = CWindChillXml::GetPath(Xmlcontent);
	//����ѹ��������  ��Ҫ��ftp����

	//CWindChillSetting::m_strpartFirstName = path;

	if (path.IsEmpty())
	{
		MessageBox("�˹���װ����ʵ��ģ��", APS_MSGBOX_TITIE, MB_OK);
		//��ȡѡ���е�����  Name Number Type
		//��ȡѡ���е�����  Name Number Type
		std::vector<PropData> prop;

		PropData temp;
		temp.m_strName = "ID";
		temp.m_strValue = std::to_string(static_cast<long long>(m_ID)).c_str();

		prop.push_back(temp);

		temp.m_strName = "Path";
		temp.m_strValue = "";
		prop.push_back(temp);

		auto comBox = (CComboBox*)GetDlgItem(IDC_ADDMATERIAL_COMBOX);
		if (comBox)
		{
			comBox->ShowWindow(SW_SHOW);
			CString str;

			comBox->GetLBText(comBox->GetCurSel(), str);
			temp.m_strName = "CATEGORY";
			temp.m_strValue = str;
			prop.push_back(temp);

			temp.m_strName = "����";
			temp.m_strValue = str;
			prop.push_back(temp);
		}


		temp.m_strName = "KEY";
		temp.m_strValue = resourceNumber;
		prop.push_back(temp);



		CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
		for (int i = 0; i < pSetting->m_strQueryMatch.GetCount(); i++)
		{
			CString strName = pSetting->m_strQueryMatch.GetAt(i).m_strName;
			CString strValue = m_Resultlist.GetItemText(row, i);
			if (!strName.IsEmpty())
			{
				if (strName.CompareNoCase(_T("����"))==0)
				{
					continue;
				}
				temp.m_strName = strName;
				temp.m_strValue = strValue;

				prop.push_back(temp);
			}
		}

		::SendMessage(GetParent()->GetSafeHwnd(), MSG_IMPORT_EQUIES, NULL, LPARAM(&prop));
		CDialog::OnOK();
		return;
	}
	else
	{
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

				CFileFind find;

				auto partdir = strDir+GetMainFileName(path)+_T("\\");
				auto isFind = find.FindFile(partdir + _T("*.catpart"));

				if (isFind)
				{
					isFind = find.FindNextFile();
					auto partpath = find.GetFilePath();

					if (GetParent())
					{
						//��ȡѡ���е�����  Name Number Type
						std::vector<PropData> prop;

						PropData temp;
						temp.m_strName = "ID";
						temp.m_strValue= std::to_string(static_cast<long long>(m_ID)).c_str();

						prop.push_back(temp);

						temp.m_strName = "Path";
						temp.m_strValue = partpath;
						prop.push_back(temp);

						auto comBox = (CComboBox*)GetDlgItem(IDC_ADDMATERIAL_COMBOX);
						if (comBox)
						{

							CString str;

							comBox->GetLBText(comBox->GetCurSel(), str);
							temp.m_strName = "CATEGORY";
							temp.m_strValue = str;
							prop.push_back(temp);

							temp.m_strName = "����";
							temp.m_strValue = str;
							prop.push_back(temp);
						}


						temp.m_strName = "KEY";
						temp.m_strValue = resourceNumber;
						prop.push_back(temp);


						CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
						for (int i = 0; i < pSetting->m_strQueryMatch.GetCount(); i++)
						{
							CString strName = pSetting->m_strQueryMatch.GetAt(i).m_strName;
							CString strValue = m_Resultlist.GetItemText(row, i);
							if (!strName.IsEmpty())
							{
								temp.m_strName = strName;
								temp.m_strValue = strValue;

								prop.push_back(temp);
							}
						}

						::SendMessage(GetParent()->GetSafeHwnd(), MSG_IMPORT_EQUIES, NULL, LPARAM(&prop));
						CDialog::OnOK();
						return;
					}
				}
			}

		}
		else
		{
			MessageBox("��ѹʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
			return;
		}
	}
}

void CDlgAddMaterial::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	auto row = m_Resultlist.GetNextItem(-1, LVIS_SELECTED);
	if (row != -1)
	{
		
		if (ResourceType_ADDMaterial == m_type)
		{
			CString operType = "1";

			CString resourceNumber = m_Resultlist.GetItemText(row, 0);
			auto  XmlContent = m_WebServiceInterface.SearchResource(operType, resourceNumber);
			AddMaterialxml.Parse(XmlContent.GetBuffer());
			CDialog::OnOK();
		}
		else
		{
			ImportEquies(row);
		}
	} 
	*pResult = 0;
}

void CDlgAddMaterial::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO:  �ڴ˴������Ϣ����������
	UPDATE_EASYSIZE;
}

void CDlgAddMaterial::OnSizing(UINT fwSide, LPRECT pRect)
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
void CDlgAddMaterial::OnBnClickedAddmaterialOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	auto row = m_Resultlist.GetNextItem(-1, LVIS_SELECTED);
	if (row != -1)
	{
		if (ResourceType_ADDMaterial == m_type)
		{
			CString operType = "1";
			CString resourceNumber = m_Resultlist.GetItemText(row, 0);
			auto  XmlContent = m_WebServiceInterface.SearchResource(operType, resourceNumber);
			AddMaterialxml.Parse(XmlContent.GetBuffer());
			
		}
		else
		{
			ImportEquies(row);
		}

		CDialog::OnOK();
	}
}
