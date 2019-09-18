// DlgPBOMEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "APSProp.h"
#include "Solid.h"
#include "Actionbase.h"
#include "AsmStep.h"
#include "CProduct.h"
#include "apsprocess.h"
#include "Equips.h"
#include "APSModel.h"
#include "UtilityFunction.h"
#include "KmZipUtil.h"

#include "common.h"
#include "WindChillSetting.h"
#include "WebServiceInterface.h"

#include "DlgPBOMEdit.h"
#include "Partinfo.h"
#include "DBOperation.h"
#include "WindChillXml.h"
#include "PBOMUtil.h"
#include "common.h"
#include "PBomCreate.h"

#include "CreatePbom.h"


#define  MSG_FILE_NEW WM_USER + 2089

// CDlgPBOMEdit �Ի���
//using namespace std;




IMPLEMENT_DYNAMIC(CDlgPBOMEdit, CDialog)

CDlgPBOMEdit::CDlgPBOMEdit(CAPSModel * pModel, QueryType nType, IAPSApplication* pApplication, CWnd* pParent /*=NULL*/)
: CDialog(CDlgPBOMEdit::IDD, pParent)
{
	m_pModel = pModel;
	m_nQueryType = nType;
	partNumber = "";
	m_pApplication = pApplication;
}

CDlgPBOMEdit::~CDlgPBOMEdit()
{
}

void CDlgPBOMEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WINDCHILL_RESULTLIST, m_Resultlist);
}


BEGIN_MESSAGE_MAP(CDlgPBOMEdit, CDialog)
	ON_BN_CLICKED(IDC_WINDCHILL_SELECT, OnQuery)
	ON_BN_CLICKED(IDC_WINDCHILL_OK, OnOK)
	ON_NOTIFY(NM_DBLCLK, IDC_WINDCHILL_RESULTLIST, OnNMDblclk)
	ON_WM_SIZE()
	ON_WM_SIZING()
END_MESSAGE_MAP()




//�Զ�����λ��
BEGIN_EASYSIZE_MAP(CDlgPBOMEdit)
	EASYSIZE(IDC_WINDCHILL_SELECT, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_OK, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_STATIC, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_STATICRESULT, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_WINDCHILL_RESULTLIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


void CDlgPBOMEdit::SetCaption(CString Caption)
{
	m_Caption = Caption;
}

BOOL CDlgPBOMEdit::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	InitList();
	SetWindowText(m_Caption);
	INIT_EASYSIZE;
	/*
	if (m_nQueryType == QueryType_EBOM)
	{
		auto pwhd = GetDlgItem(IDC_WINDCHILL_TYPEEDIT);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_SHOW);
		}

		pwhd = GetDlgItem(IDC_WINDCHILL_DESINGEDIT);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_HIDE);
		}

		pwhd = GetDlgItem(IDC_STATICDESING);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_HIDE);
		}

		pwhd = GetDlgItem(IDC_STATICTYPE);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		auto pwhd = GetDlgItem(IDC_WINDCHILL_NUMEDIT);
		if (pwhd)
		{
			auto pw = GetDlgItem(IDC_WINDCHILL_SELECT);
			if (pw)
			{
				CRect rc;
				pw->GetWindowRect(&rc);
				ScreenToClient(&rc);

				CRect rt;
				pwhd->GetWindowRect(&rt);
				ScreenToClient(&rt);
				pwhd->MoveWindow(rt.left, rc.top, rc.left - 80, rc.bottom - 10);
			}
		}
	}
	*/

	auto pwhd = GetDlgItem(IDC_WINDCHILL_NUMEDIT);
	if (pwhd)
	{
		auto pw = GetDlgItem(IDC_WINDCHILL_SELECT);
		if (pw)
		{
			CRect rc;
			pw->GetWindowRect(&rc);
			ScreenToClient(&rc);

			CRect rt;
			pwhd->GetWindowRect(&rt);
			ScreenToClient(&rt);
			pwhd->MoveWindow(rt.left, rc.top, rc.left - 80, rc.bottom - 10);
		}
	}
	return TRUE;
}


void CDlgPBOMEdit::InitList()
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


// CDlgPBOMEdit ��Ϣ�������
void CDlgPBOMEdit::OnQuery()
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting == NULL)
		return;

	m_Resultlist.DeleteAllItems();
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString Designedit, Numedit, Typeedit;
	((CEdit *)GetDlgItem(IDC_WINDCHILL_DESINGEDIT))->GetWindowText(Designedit);
	((CEdit *)GetDlgItem(IDC_WINDCHILL_NUMEDIT))->GetWindowText(Numedit);
	((CEdit *)GetDlgItem(IDC_WINDCHILL_TYPEEDIT))->GetWindowText(Typeedit);
	if (Designedit != "" || Typeedit != "" || Numedit != "")  //�����༭����ͬʱΪ��
	{
#ifdef DEV_TMP
		AfxMessageBox(_T("����webserver PBOMģ����ѯ�ӿ�"));
#endif
		CString operType = "2";//PBOM�༭��ѯ��PBOM��ѯ
		if (m_nQueryType == QueryType_EBOM)//PBOM������ѯ
		{
			operType = "1";
		}
		else if (m_nQueryType == QueryType_Material_PBOM)
		{
			operType = "3";
		}
		else if (m_nQueryType == QueryType_RULE)
		{
			operType = "4";
		}

		CString XmlContent = m_WebServiceInterface.getPartList(operType, Numedit, Typeedit, Designedit);

		TiXmlDocument doc;
		doc.Parse(XmlContent.GetBuffer());
		XmlContent.ReleaseBuffer();
		TiXmlElement *root = doc.FirstChildElement();
		if (root == NULL || root->FirstChildElement() == NULL)
		{
			MessageBox("��ѯ����xml�ļ���Ϣ����,����������Ϣ", APS_MSGBOX_TITIE, MB_OK);
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
					{
						m_Resultlist.SetItemText(row, col, word);
					}
				}

				PartContent = PartContent->NextSiblingElement();
			}

			elem = elem->NextSiblingElement();
		}

		m_Resultlist.SetRedraw(FALSE);

		CHeaderCtrl *pheader = m_Resultlist.GetHeaderCtrl();
		auto colcount = pheader->GetItemCount();
		for (int i = 0; i < colcount; ++i)
		{
			m_Resultlist.SetColumnWidth(i, LVSCW_AUTOSIZE);
			auto colwidth = m_Resultlist.GetColumnWidth(i);
			m_Resultlist.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
			auto headwidth = m_Resultlist.GetColumnWidth(i);
			m_Resultlist.SetColumnWidth(i, max(colwidth, headwidth));
		}
		m_Resultlist.SetRedraw(TRUE);
		doc.Clear();
	}
	else
	{
		MessageBox("�������ѯ����", APS_MSGBOX_TITIE, MB_OK);
	}
}

void CDlgPBOMEdit::OnOK()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	auto row = m_Resultlist.GetNextItem(-1, LVIS_SELECTED);

	if (row != -1)
	{
#ifdef DEV_TMP
		CString test;
		test.Format(_T("ѡ�е�%d��  ����webserver PBOM��ϸ��Ϣ��ȡ�ӿ�"), row);
		AfxMessageBox(test);
#endif
		auto Issuccess = GetDataFromWebService(row);
		if (Issuccess)
		{
			CDialog::OnOK();
		}
	}
}

void CDlgPBOMEdit::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		auto Issuccess = GetDataFromWebService(pNMListView->iItem);
		if (Issuccess)
		{
			CDialog::OnOK();
		}
	}
	*pResult = 0;
}


void CDlgPBOMEdit::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
	/*
	if (m_nQueryType == QueryType_EBOM)
	{
		auto pwhd = GetDlgItem(IDC_WINDCHILL_TYPEEDIT);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_SHOW);
		}

		pwhd = GetDlgItem(IDC_WINDCHILL_DESINGEDIT);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_SHOW);
		}

		pwhd = GetDlgItem(IDC_STATICDESING);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_SHOW);
		}

		pwhd = GetDlgItem(IDC_STATICTYPE);
		if (pwhd)
		{
			pwhd->ShowWindow(SW_SHOW);
		}

		
	}
	else
	{
		auto pwhd = GetDlgItem(IDC_WINDCHILL_NUMEDIT);
		if (pwhd)
		{
			auto pw = GetDlgItem(IDC_WINDCHILL_SELECT);
			if (pw)
			{
				CRect rc;
				pw->GetWindowRect(&rc);
				ScreenToClient(&rc);

				CRect rt;
				pwhd->GetWindowRect(&rt);
				ScreenToClient(&rt);
				pwhd->MoveWindow(rt.left, rc.top, rc.left - 80, rc.bottom - 10);
			}
		}
	}
	*/
	auto pwhd = GetDlgItem(IDC_WINDCHILL_NUMEDIT);
	if (pwhd)
	{
		auto pw = GetDlgItem(IDC_WINDCHILL_SELECT);
		if (pw)
		{
			CRect rc;
			pw->GetWindowRect(&rc);
			ScreenToClient(&rc);

			CRect rt;
			pwhd->GetWindowRect(&rt);
			ScreenToClient(&rt);
			pwhd->MoveWindow(rt.left, rc.top, rc.left - 80, rc.bottom - 10);
		}
	}
}

void CDlgPBOMEdit::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	static CRect rect;
	if (rect.Width() == 0 && rect.Height() == 0)
	{
		GetWindowRect(&rect);
	}
	EASYSIZE_MINSIZE(rect.Width(), rect.Height(), fwSide, pRect);
}


BOOL CDlgPBOMEdit::PreTranslateMessage(MSG* pMsg)
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

BOOL CDlgPBOMEdit::GetDataFromWebService(int nRow)
{
	CString  operType = "1";
	
	auto index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strQueryMatch, "���");
	if (index == -1)
		return FALSE;

	partNumber = m_Resultlist.GetItemText(nRow, index);


	CWindChillSetting::m_strpartFirstName = partNumber;

	if (QueryType_EBOM == m_nQueryType)
	{
		operType = "1";
	}
	else if(QueryType_Material_PBOM == m_nQueryType)
	{
		operType = "3";
	}
	else
	{
		operType = "2";
	}

	if (m_nQueryType == QueryType_Material_PBOM)
	{
		CString strXmlcontent = m_WebServiceInterface.getBom(operType, partNumber);
		m_PBOMxml.Parse(strXmlcontent.GetBuffer());
		strXmlcontent.ReleaseBuffer();
		//��ʱ���ô�ģ���ļ�
		return TRUE;
	}
	else
	{
		if (QueryType_EBOM == m_nQueryType)
		{

			CString strXmlcontent = m_WebServiceInterface.getBom(operType, partNumber);
			m_PBOMxml.Parse(strXmlcontent.GetBuffer());
			strXmlcontent.ReleaseBuffer();

			std::vector<PartInfo> partInfo;
			TiXmlElement *root = m_PBOMxml.FirstChildElement();
			if (root == NULL || root->FirstChildElement() == NULL)
			{
				m_PBOMxml.Clear();
				return FALSE;
			}

			auto elem = root->FirstChildElement()->FirstChildElement();
			while (elem != NULL)
			{
				PartInfo part(elem);
				partInfo.push_back(part);
				elem = elem->NextSiblingElement();
			}

			CDBOperation db;
			CWaitCursor wait;
			auto isSuccess = db.ConnToDB();
			wait.Restore();
			if (isSuccess)
			{
				auto Part = lookforpartbyNumber(partInfo, partNumber.GetBuffer());
				if (Part.value.empty())
				{
					MessageBox("��ѯ��Ϣ����", APS_MSGBOX_TITIE, MB_OK);
					return FALSE;
				}

				CString sql = CString("select * from EPMDocumentInfo where PART_NUMBER= \'") + partNumber + "\' and PART_VERSION= \'" + Part.value["Version"].c_str() + "\'";
				auto sqlresult = db.ExecuteWithResSQL(sql.GetBuffer());
				if (sqlresult)
				{
					_variant_t Part_Number, FILENAME, FolderPath, EpmName, isConvert;
					CString tempFileName, tempFolderPath, tempisConvert, tempEpmName;

					while (!sqlresult->adoEOF)
					{
						Part_Number = sqlresult->GetCollect("Part_Number");
						FILENAME = sqlresult->GetCollect("FILENAME");
						EpmName = sqlresult->GetCollect("EPM_NAME");
						FolderPath = sqlresult->GetCollect("FolderPath");
						isConvert = sqlresult->GetCollect("isConvert");

						if (FILENAME.vt != VT_NULL && FolderPath.vt != VT_NULL&&isConvert.vt != VT_NULL)
						{
							tempFileName = (LPCTSTR)(_bstr_t)FILENAME;
							tempFolderPath = (LPCTSTR)(_bstr_t)FolderPath;
							tempisConvert = (LPCTSTR)(_bstr_t)isConvert;
							tempEpmName = (LPCTSTR)(_bstr_t)EpmName;
						}
						sqlresult->MoveNext();
					}

					/*	tempFileName = "20190423031926950.zip";
					tempFolderPath = "/EPMDocumentInfo";
					tempisConvert = "0";
					tempEpmName = "J-GDGII5F-0000.TanSheGuaDanGou.CATProduct";*/

					if (tempFileName.CompareNoCase("") != 0 && tempFolderPath.CompareNoCase("") != 0)
					{
						CString strFTPPath, strFTPName = tempFileName, strModelName = tempEpmName;

						//��ȡxml��ȡ���ļ��������Ʒ����ģ���ļ�
						//ftp����. 

						strFTPPath = tempFolderPath;

						if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
						{
							MessageBox("ftp����ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
							return False;
						}

						TCHAR TempDir[1024];
						GetTempPath(1024, TempDir);
						CString sTempDir(TempDir);
						CString tempdir = sTempDir + _T("KM3DCAPP-AWORK");

						if (!ExistDir(tempdir))
						{
							CreateDir(tempdir);
						}
						sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");
						CString strlocal = sTempDir + strFTPName;

						BOOL bSucc = m_FTPInterface.DownLoad(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
						if (!bSucc)
						{
							CString strErrMsg = _T("�����ļ�ʧ�ܣ�");
							//strErrMsg.Format(_T("�����ļ���%s��ʧ�ܣ�"), strFTPName);
							MessageBox(strErrMsg, APS_MSGBOX_TITIE, MB_OK);

							return FALSE;
						}

						if (strFTPName.Find(_T(".zip")) >= 0 && atoi(tempisConvert.GetBuffer()) == 0)  //���ݵ�ѹ���ļ���  δת��
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


								//ģ��ת��
								CString strModelFullPath = _T("");
								strModelFullPath.Format("%s%s\\%s", strDir, GetMainFileName(strFTPName), strModelName);
								if (!ExistFile(strModelFullPath))
								{
									CString strErrMsg = _T("");
									strErrMsg.Format(_T("δ�ҵ�ģ���ļ���%s"), strModelFullPath);
									MessageBox(strErrMsg, APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
									return FALSE;
								}

								CString strXml = _T("");
								//Get3DFileType(strDir + strtmp, strFileType.GetBuffer(128), 128);
								//�Ȱ�ģ���ļ�ת����xml+hsf
								if (!ConvertModel(strModelFullPath, strXml))
								{
									MessageBox("ģ��ת��ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
									return FALSE;
								}

								//��ȡת�����xml�ļ������Ը��º��뵽ast
								if (PathFileExists(strXml))
								{
									bool isIop=true;
									JudgeIsIopXml(strXml,isIop);


									if (!isIop)//exchange 
									{
										//������д��xml��
										CPBOMUtil util;

										util.CreatePbomExchange(strXml, strXmlcontent, partNumber);
										documentAnsiToutf8(strXml);
									}
									else
									{
										CPBOMUtil util;
										//strXml = "C:\\Users\\Administrator\\Desktop\\39909B84-206E-4BE4-BD29-631CD9B81839\\D6101000.xml";

										util.CreatePbomIop(strXml, strXmlcontent, partNumber);
										documentAnsiToutf8(strXml);
									}
									

									//begin add by xjt in2019.2.19 for 70771
									auto pProduct = m_pModel->GetProduct();
									if (pProduct)
									{
										auto isEmpty = pProduct->GetIsEmpty();
										if (!isEmpty)
										{
											::SendMessage(GetParent()->GetSafeHwnd(), MSG_FILE_NEW, NULL, LPARAM(""));

											IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
											if (!doc)
											{
												ASSERT(FALSE);
												return FALSE;
											}
											m_pModel = (CAPSModel*)doc->GetAPSModel();
										}
									}
									//end add
									//CString strFileType;
									//Get3DFileType(strModelFullPath, strFileType.GetBuffer(128), 128);

									IPSManager* pIProduct = NULL;
									HRESULT hr = ::CoCreateInstance(CLSID_PSManager,
										NULL,
										CLSCTX_INPROC_SERVER,
										IID_IPSManager,
										(void **)&pIProduct);
									pIProduct->Init(1);
									pIProduct->SetOcxModuleState(NULL);
									pIProduct->SetActiveModel((VOID_PTR)m_pModel);
									pIProduct->ImportProduct((_bstr_t)(strXml), (_bstr_t)(""), 0);

									CString strFileName = partNumber + ".kmapx";
									IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
									doc->SetStdFileName(strFileName.AllocSysString());
								}

								return TRUE;
							}
							else
							{
								MessageBox("��ѹģ���ļ�ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
								kmZipUtil.CloseZipFile();
								return FALSE;
							}
						}
						else
						{
							//���ݵ�ѹ���ļ��� �Ѿ�ת��

							if (strFTPName.Find(_T(".zip")) >= 0 && atoi(tempisConvert.GetBuffer()) == 1)
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
								}

								CFileFind find;
								CString strXml;
								
								auto isFind = find.FindFile(strDir + _T("*.xml"));
								while (isFind)
								{
									isFind = find.FindNextFile();
									if (find.IsDots())
									{
										continue;
									}
									else
									{
										strXml = strDir + find.GetFileName();
									}
								}
								
								//strXml="C:\\Users\\Administrator\\AppData\\Local\\Temp\\KM3DCAPP-AWORK\\20190514023547517\\20190514023547517\\J-GDGII5F-0000.TanSheGuaDanGou.xml";
								if (PathFileExists(strXml))
								{
									//��ȡת�����xml�ļ������Ը��º��뵽ast
								
									bool isIop=true;
									JudgeIsIopXml(strXml,isIop);

									if (!isIop)//exchange 
									{
										//������д��xml��
										CPBOMUtil util;

										util.CreatePbomExchange(strXml, strXmlcontent, partNumber);
										documentAnsiToutf8(strXml);
									}
									else
									{
										CPBOMUtil util;
										//strXml = "C:\\Users\\Administrator\\Desktop\\39909B84-206E-4BE4-BD29-631CD9B81839\\D6101000.xml";

										util.CreatePbomIop(strXml, strXmlcontent, partNumber);
										documentAnsiToutf8(strXml);
									}


									auto pProduct = m_pModel->GetProduct();
									if (pProduct)
									{
										auto isEmpty = pProduct->GetIsEmpty();
										if (!isEmpty)
										{
											::SendMessage(GetParent()->GetSafeHwnd(), MSG_FILE_NEW, NULL, LPARAM(""));

											IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
											if (!doc)
											{
												ASSERT(FALSE);
												return FALSE;
											}
											m_pModel = (CAPSModel*)doc->GetAPSModel();
										}
									}


									IPSManager* pIProduct = NULL;
									HRESULT hr = ::CoCreateInstance(CLSID_PSManager,
										NULL,
										CLSCTX_INPROC_SERVER,
										IID_IPSManager,
										(void **)&pIProduct);
									pIProduct->Init(1);
									pIProduct->SetOcxModuleState(NULL);
									pIProduct->SetActiveModel((VOID_PTR)m_pModel);
									pIProduct->ImportProduct((_bstr_t)(strXml), (_bstr_t)"", 0);

									CString strFileName = partNumber + ".kmapx";
									IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
									doc->SetStdFileName(strFileName.AllocSysString());
								}
								return TRUE;
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
								return FALSE;
							}
						}
#endif
					}
					else
						MessageBox("���ݿ��ѯ���Ϊ��", APS_MSGBOX_TITIE, MB_OK);
					sqlresult->Close();
					sqlresult = NULL;
				}
				sql.ReleaseBuffer();
			}
			else
			{
				MessageBox("���ݿ�����ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
			}
		}
		else if (QueryType_PBOM == m_nQueryType || QueryType_RULE == m_nQueryType)
		{

			CString strXmlcontent = m_WebServiceInterface.getBOMOfHsf(operType, partNumber);
			m_PBOMxml.Parse(strXmlcontent.GetBuffer());
			strXmlcontent.ReleaseBuffer();

			auto FileName = CWindChillXml::GetPath(strXmlcontent);

			if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
			{
				MessageBox(_T("ftp����ʧ�ܣ�"), APS_MSGBOX_TITIE, MB_OK);
				return FALSE;
			}

			CString strFTPPath = "/hsfFiles", strFTPName = FileName, strModelName = "";

			TCHAR TempDir[1024];
			GetTempPath(1024, TempDir);
			CString sTempDir(TempDir);
			sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");

			if (!ExistDir(sTempDir))
			{
				if (!::CreateDirectory(sTempDir, NULL))
				{
				}
			}

			CString strlocal = sTempDir + strFTPName;
			BOOL bSucc = m_FTPInterface.DownLoad(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
			if (!bSucc)
			{
				CString strErrMsg = _T("�����ļ�ʧ�ܣ�");
				//strErrMsg.Format(_T("�����ļ���%s��ʧ�ܣ�"), strFTPName);
				MessageBox(strErrMsg, APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
				return FALSE;
			}
			

			if (strFTPName.Find(_T(".zip")) >= 0)  //���ݵ�ѹ���ļ���
			{
				CString strXml = _T("");

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
					strDir =strDir + GetMainFileName(strFTPName)+"\\";
					auto isFind = find.FindFile(strDir + _T("*.xml"));

					while (isFind)
					{
						isFind = find.FindNextFile();
						if (find.IsDots())
						{
							continue;
						}
						else
						{
							CString fullpath = strDir + find.GetFileName();
							auto name = GetMainFileName(fullpath);
							if (name.CompareNoCase("Pbom") == 0)
							{
								CPbomCreate PbomCreate(fullpath, partNumber);

								PbomCreate.CreatePbom();
								strXml =GetFilePath(fullpath)+"files\\Pbom.xml";
								documentAnsiToutf8(strXml);
							}
						}
					}

					if (!strXml.IsEmpty())
					{
						auto pProduct = m_pModel->GetProduct();
						if (pProduct)
						{
							auto isEmpty = pProduct->GetIsEmpty();
							if (!isEmpty)
							{
								::SendMessage(GetParent()->GetSafeHwnd(), MSG_FILE_NEW, NULL, LPARAM(""));

								IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
								if (!doc)
								{
									ASSERT(FALSE);
									return FALSE;
								}
								m_pModel = (CAPSModel*)doc->GetAPSModel();
							}
						}


						IPSManager* pIProduct = NULL;
						HRESULT hr = ::CoCreateInstance(CLSID_PSManager,
							NULL,
							CLSCTX_INPROC_SERVER,
							IID_IPSManager,
							(void **)&pIProduct);
						pIProduct->Init(1);
						pIProduct->SetOcxModuleState(NULL);
						pIProduct->SetActiveModel((VOID_PTR)m_pModel);
						pIProduct->ImportProduct((_bstr_t)(strXml), (_bstr_t)"", 0);

						CString strFileName = partNumber + ".kmapx";
						IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
						doc->SetStdFileName(strFileName.AllocSysString());
					}
					else
						return FALSE;
				}
				return TRUE;
			}
			else
			{
				MessageBox("��ѹģ���ļ�ʧ�ܣ�", APS_MSGBOX_TITIE, MB_OK);
				return FALSE;
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
					return FALSE;
				}
			}
#endif
		}
	}
	return FALSE;
}
