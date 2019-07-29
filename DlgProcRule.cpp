// DlgExportPDF.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "UtilityFunction.h"
#include "APSProp.h"
#include "Solid.h"
#include "Actionbase.h"
#include "AsmStep.h"
#include "CProduct.h"
#include "apsprocess.h"

#include "common.h"
#include "WebServiceInterface.h"
#include "WindChillSetting.h"
#include "DlgProcRule.h"
#include "DirectoryDialog.h"


#define PLUGIN_PAGETYPE "ҳ������"
#define PLUGIN_PAGENAME "ҳ������"
#define PLUGIN_PROCRULEXML "���չ��ģ��.xml"
// CDlgProcRule �Ի���

IMPLEMENT_DYNAMIC(CDlgProcRule, CDialog)

BEGIN_MESSAGE_MAP(CDlgProcRule, CDialog)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_SAVE, OnSave)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_CANCEL, OnClose)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_MOVEUP, OnBnClickedMoveUp)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_MOVEDOWN, OnBnClickedMoveDown)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_DELETE, OnBnClickedDelete)
	//ON_BN_CLICKED(IDC_PROCRULE_BTN_EDIT, OnBnClickedEdit)
	ON_BN_CLICKED(IDC_WINDCHILL_PROCRULE_BTN_PDFPATH, OnDir)
	//ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, OnItemChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, OnSelChanged)
	ON_WM_SIZE()
	
END_MESSAGE_MAP()


/*BEGIN_MESSAGE_MAP(CBCGPGridCtrlX, CBCGPGridCtrl)

END_MESSAGE_MAP()*/

CDlgProcRule::CDlgProcRule(CWnd* pParent /*=NULL*/)
: CDialog(CDlgProcRule::IDD, pParent)
{
	m_bExportAction = true;
	m_iType = -1;
	//m_ConfigProcPDFName = new CStringArray;
	m_ConfigProcPDFName = new std::vector<std::map<CString,UINT>>;
	m_PdfConfigDatas = NULL;
	m_ProcList = NULL;
	AfxEnableControlContainer();
	m_bCtrlSucc = false;
	//CShellManager *pShellManager = new CShellManager;

}

CDlgProcRule::~CDlgProcRule()
{
	if (m_ConfigProcPDFName)
	{
		delete m_ConfigProcPDFName;
		m_ConfigProcPDFName = NULL;
	}
	if (m_ProcList)
	{
		delete m_ProcList;
		m_ProcList = NULL;
	}
}
int CDlgProcRule::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	//test pdf review 64λ����
	/*RECT ParRect;
	GetClientRect(&ParRect);

	//ParRect.left += 300;
	if (!m_view.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, ParRect, this, 0))
	{
		return FALSE;
	}
	m_view.LoadFile("f:\\a.pdf");*/


	return 0;
}
void CDlgProcRule::setFilePath(CString sPath)
{
	m_sPath = sPath;
}

void CDlgProcRule::setFileName(CString sName)
{
	m_sName = sName;
}
void CDlgProcRule::setTemplateName(CString sName)
{
	m_sTemplateName = sName;
}
void CDlgProcRule::setTemplateType(int iType)
{
	m_iType = iType;
}


void CDlgProcRule::getPath(CString &sOutputFilePath, CString &sLoadFilePath)
{
	sOutputFilePath = m_sOutputFilePath;
	sLoadFilePath = m_sLoadFilePath;
}


bool CDlgProcRule::getExportAction()
{
	return m_bExportAction;
}

void CDlgProcRule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CDlgProcRule::OnInitDialog()
{
	CDialog::OnInitDialog();
	BOOL bExist = PathFileExists(m_sTemplateName);
	if (!bExist)
	{
		//KmMessageBox(ResourceString(IDS_APM_PDF_NO_TEMPLATE_FILE, _T("�Ҳ���PDFģ���ļ�����ȷ��.")),APS_MSGBOX_TITIE,MB_ICONERROR);
		//AfxMessageBox("�Ҳ���PDFģ���ļ�����ȷ��!");
		MessageBox("�Ҳ���PDFģ���ļ�����ȷ�ϣ�",APS_MSGBOX_TITIE,MB_OK);
		EndDialog(S_FALSE);
		return FALSE;
	}
	m_ProcPDFArray.RemoveAll();
	m_StepPDFArray.RemoveAll();
	auto flag =ParseConfigFile(m_sTemplateName, m_ProcPDFArray, m_StepPDFArray);
	if (!flag)
	{
		MessageBox("ģ���ļ�������ȷ�ϣ�", APS_MSGBOX_TITIE, MB_OK);
		EndDialog(S_FALSE);
		return FALSE;
	}
	for (int i5 = 0; i5 < m_pProcess->GetAsmSteps()->GetCount(); i5++)
	{
		CAsmStep * pStep = m_pProcess->GetAsmSteps()->GetAt(i5);
		//m_pPageTypeMap.insert(std::make_pair(pStep->GetName(NULL), pStep->GetID()));
		//CString a = pStep->GetName(NULL);
		CString val;
		CAPSPropList * pRrop = pStep->GetBasePropList();
		POSITION Pos = pRrop->GetHeadPosition();
		while (Pos)
		{
			CAPSProp&  prop = pRrop->GetNext(Pos);

				if (prop.GetName() == "#prop_name")
				{
					val = prop.ConvertTo();
					break;
				}
		}
		UINT b = pStep->GetID();
		m_pPageTypeMap.insert(std::make_pair(val, b));
	}
	m_pPageTypeMap.insert(std::make_pair("����", -1));
	
	//�����������ҳ��ؼ�
	m_ProcList = new CMyGridCtrl;
	m_ProcList->CreateCtrl(this);
	CRect parrect;
	GetClientRect(&parrect);
	CFont* Font = GetFont();
	CRect rect;
	m_ProcList->GetClientRect(&rect);
	
#if 1
	//�˴��߼�
	//if (process�����ݲ�Ϊ��)
	//	��ȡprocess������
	//else
	//����Ĭ�ϵĳ�ʼ����
	CArray<CAPSGridPropData*>* pPdfConfigDatas = m_pProcess->GetProcessPdfConfigDatas();

	CAPSGridPropData* pPropData = NULL;
	bool bStored = false;
	for (int i3 = 0; i3 < pPdfConfigDatas->GetCount(); i3++)
	{
		pPropData = pPdfConfigDatas->GetAt(i3);
		CString strName = pPropData->GetGridName();
		if ((m_iType == 0 && strName.CompareNoCase("#proc_line") == 0) ||
			(m_iType == 1 && strName.CompareNoCase("#proc_rule") == 0))
		{
			bStored = true;
			break;
		}
	}
	if (bStored)  //���չ�������ģ�巽��
	{
		//����·�߿���ֱ��copy ���չ����е�ģ�巽�������չ�̻�Ҫ�����Ƿ�����ɾ���˲������޸�ģ�巽����
		if (m_iType == 0)
			//SetProcTempConfig(pPdfConfigDatas);
			m_PdfConfigDatas = pPropData;
		else
		{
			//�����Ѵ�������ɾ���Ĳ��裬����Ӧ��ģ�巽����ɾ���ò����µ�ҳ�����á�
			int nRowCnt = pPropData->GetRowCount();
			for (int j = 0; j < nRowCnt; ++j)
			{
				CAPSProp prop;
				pPropData->GetCell(nRowCnt - j - 1, 0, prop);
				_variant_t vtd = prop.GetName();  //����ID�����ǹ�����=-1
				int iStepID = (int)vtd;
				if (iStepID != -1 && m_pProcess->GetAsmStep(iStepID) == NULL)  //������ɾ��
				{
					pPropData->DelRow(nRowCnt - j - 1);
				}
			}
			//Ȼ�������ǰ���������в��裬��ģ�巽�����бȶԣ���ȡ������ӵĲ����б�ͬʱ��¼ǰ�ò��裩������ʹ�á�
			//�÷�����׼ȷ������ģ�巽����ɾ���Ĳ����ֱ�ͳ�Ƴ���
			std::map<int, int> map;
			int iCurStepID = -1;
			int iPrevStepID = -1;
			/*for (int ii = 0; ii < m_pProcess->GetAsmSteps()->GetCount(); ii++)
			{
				CAsmStep * pStep = m_pProcess->GetAsmSteps()->GetAt(ii);
				iCurStepID = pStep->GetID();
				if (!IsStepInTempPlan(iCurStepID, pPropData))
				{
					map[iCurStepID] = iPrevStepID;
				}
			}*/
			CStringArray StrStepTemp;
			if (map.size() > 0) //������˲���
			{
				GetStepTemp(StrStepTemp);
			}
			//����ģ�巽������������ĳ���²����ǰ�ò���ʱ�������Ĭ��ģ�巽�������ã�����ǰ�ò��������²����ģ�塣
			for (std::map<int, int>::iterator it = map.begin(); it != map.end(); ++it)
			{
				iCurStepID = it->first;
				iPrevStepID = it->second;
				nRowCnt = pPropData->GetRowCount();
				int iPrevCnt = 0;
				for (int idx = 0; idx < nRowCnt; idx++)
				{
					CAPSProp prop;
					pPropData->GetCell(idx, 0, prop);
					_variant_t vtd = prop.GetName();  //����ID�����ǹ�����=-1
					int iStepID = (int)vtd;
					if (iStepID == -1 && !(idx == nRowCnt - 1 && iPrevStepID == -1)) //������ģ���Թ�
						continue;
					if (iPrevStepID == -1) //��ʾ�����ӵĲ����ǵ�һ������
					{
						//��Ĭ���������ȡ�������ϵ�ģ��
						for (int i2 = 0; i2 < StrStepTemp.GetCount(); i2++)
						{
							CAPSPropList * pPropList = new CAPSPropList();
							pPropList->RemoveAll();
							CAPSProp * a = new CAPSProp();
							CString tmp;
							tmp.Format("%d", iCurStepID);
							a->SetName(tmp);
							CString val;
							CAPSPropList * pRrop = m_pProcess->GetAsmStep(iCurStepID)->GetBasePropList();
							POSITION Pos = pRrop->GetHeadPosition();
							while (Pos)
							{
								CAPSProp&  prop1 = pRrop->GetNext(Pos);

								if (prop1.GetName() == "#prop_name")
								{
									val = prop1.ConvertTo();
									break;
								}
							}
							//a->SetDisplayName(m_pProcess->GetAsmStep(iCurStepID)->GetName(NULL));
							a->SetDisplayName(val);
							pPropList->AddTail(*a);
							a->SetName(StrStepTemp.GetAt(i2)+".pdf");
							a->SetDisplayName(StrStepTemp.GetAt(i2));
							pPropList->AddTail(*a);
							if (iStepID == -1 && (idx == nRowCnt - 1 && iPrevStepID == -1))
								pPropData->AddRow(*pPropList);
							else
							pPropData->InsertRow(idx++, *pPropList);
						}
						break;
					}
					else
					{
						if (iPrevStepID == iStepID)  //�ҵ���ǰ�ò���
						{
							//��ȡǰ�ò������õ�ģ������
							iPrevCnt++;
							continue;
						}
						else if (iPrevCnt != 0)
						{
							//��Ĭ���������ȡ�������ϵ�ģ��
							for (int i2 = 0; i2 < StrStepTemp.GetCount(); i2++)
							{
								CAPSPropList * pPropList = new CAPSPropList();
								pPropList->RemoveAll();
								CAPSProp * a = new CAPSProp();
								CString tmp;
								tmp.Format("%d", iCurStepID);
								a->SetName(tmp);
								CString val;
								CAPSPropList * pRrop = m_pProcess->GetAsmStep(iCurStepID)->GetBasePropList();
								POSITION Pos = pRrop->GetHeadPosition();
								while (Pos)
								{
									CAPSProp&  prop1 = pRrop->GetNext(Pos);

									if (prop1.GetName() == "#prop_name")
									{
										val = prop1.ConvertTo();
										break;
									}
								}
								//a->SetDisplayName(m_pProcess->GetAsmStep(iCurStepID)->GetName(NULL));
								a->SetDisplayName(val);
								pPropList->AddTail(*a);
								a->SetName(StrStepTemp.GetAt(i2) + ".pdf");
								a->SetDisplayName(StrStepTemp.GetAt(i2));
								pPropList->AddTail(*a);
								pPropData->InsertRow(idx++, *pPropList);
							}
							break;
						}
					}
				}
			}
			//SetProcTempConfig(pPdfConfigDatas);   //???�洢�����޸Ĺ�����
			m_PdfConfigDatas = pPropData;
		}
	}
	else  //��ȡĬ������
	{
		pPropData = new CAPSGridPropData();
		//������������
		if (m_iType == 0)
			pPropData->SetGridName("#proc_line");
		else if (m_iType == 1)
			pPropData->SetGridName("#proc_rule");
		CAPSPropList * pColList = new CAPSPropList();
		//���õ�һ������
		CAPSProp * a = new CAPSProp();
		a->SetName(PLUGIN_PAGETYPE);
		a->SetDisplayName(PLUGIN_PAGETYPE);
		pColList->AddTail(*a);
		//���õڶ�������
		a->SetName(PLUGIN_PAGENAME);
		a->SetDisplayName(PLUGIN_PAGENAME);
		pColList->AddTail(*a);
		pPropData->SetTableHeadList(*pColList);

		CString strIni = GetModuleDir() + _T("\\PDFTemplatePlan.ini");
		TCHAR chSectionnames[2048] = { 0 };
		char * pSectionName;
		int i;
		int j = 0;
		int count = 0;
		::GetPrivateProfileSectionNames(chSectionnames, 2048, strIni);
		for (i = 0; i < 2048; i++, j++)
		{
			if (chSectionnames[0] == '\0')
				break;
			if (chSectionnames[i] == '\0')
			{
				pSectionName = &chSectionnames[i - j];
				count++;
				j = -1;
				if ((m_iType == 1 && strcmp(pSectionName, _T("���չ��")) != 0) || (m_iType == 0 && strcmp(pSectionName, _T("����·��")) != 0))
					continue;
				CStringArray arKeys;
				EnumIniKeys(strIni, pSectionName, arKeys);
				CAPSPropList * pPropList = new CAPSPropList();
				for (INT_PTR ii = 0; ii < arKeys.GetSize(); ii++)
				{
					CString strVal;
					CString strKey = arKeys.GetAt(ii);
					GetPrivateProfileString(pSectionName, strKey, _T(""), strVal.GetBuffer(512), 512, strIni);
					strVal.ReleaseBuffer();
					CStringArray arrCellVal;
					GetArrayFromString(strVal, _T(";"), arrCellVal);
					for (int j1 = 0; j1 < arrCellVal.GetCount(); j1++)
					{
						CString TemplateAry = _T("PDFTemplate");
						CString strFullPath;//KmAppSettings::GetConfigFileFullPath(TemplateAry);
						strFullPath.Format(_T("%s%s%s"), GetModuleDir(), _T("\\Resources\\Chs\\"), TemplateAry);

						strFullPath.Append(_T("\\"));
						strFullPath.Append(arrCellVal.GetAt(j1));
						strFullPath.Append(_T(".pdf"));
						bExist = PathFileExists(strFullPath);
						if (bExist)
						{
							if (strKey.CompareNoCase(_T("����")) == 0)
							{
								pPropList->RemoveAll();
								a->SetName("-1");
								a->SetDisplayName("����");
								pPropList->AddTail(*a);
								a->SetName(arrCellVal.GetAt(j1) + ".pdf");  //pdf��ȫ·��
								a->SetDisplayName(arrCellVal.GetAt(j1));
								pPropList->AddTail(*a);
								pPropData->AddRow(*pPropList);
							}

							if (strKey.CompareNoCase(_T("����")) == 0)
							{
								for (int i1 = 0; i1 < m_pProcess->GetAsmSteps()->GetCount(); i1++)
								{
									CAsmStep * pStep = m_pProcess->GetAsmSteps()->GetAt(i1);
									CAPSProp * b = new CAPSProp();
									CString tmp;
									tmp.Format("%d", pStep->GetID());
									b->SetName(tmp);
									CString val;
									CAPSPropList * pRrop = pStep->GetBasePropList();
									POSITION Pos = pRrop->GetHeadPosition();
									while (Pos)
									{
										CAPSProp&  prop = pRrop->GetNext(Pos);

										if (prop.GetName() == "#prop_name")
										{
											val = prop.ConvertTo();
											break;
										}
									}
									//b->SetDisplayName(pStep->GetName(NULL));
									b->SetDisplayName(val);
									pPropList->RemoveAll();
									pPropList->AddTail(*b);
									b->SetName(arrCellVal.GetAt(j1) + ".pdf");
									b->SetDisplayName(arrCellVal.GetAt(j1));
									pPropList->AddTail(*b);
									pPropData->AddRow(*pPropList);
								}
							}
						}
					}
				}

				if (chSectionnames[i + 1] == 0)
				{
					break;
				}
			}
		}
		//SetProcTempConfig(pPdfConfigDatas);   //???�洢�����޸Ĺ�����
		m_PdfConfigDatas = pPropData;
	}

	//for (int i1 = 0; i1 < m_PdfConfigDatas.GetCount(); i1++)
	{
		CAPSGridPropData * pTableProp = m_PdfConfigDatas;
		int nRowCount = pTableProp->GetRowCount();
		int nColCount = pTableProp->GetColumnCount();

		// ������
		for (int j2 = 0; j2 < nColCount; j2++)
		{
			CAPSProp prop;
			pTableProp->GetColumn(j2, prop);
			m_ProcList->InsertColumn(j2, prop.GetName(), (rect.right - rect.left) / 2 - 5);
			m_ProcList->SetHeaderAlign(j2, HDF_LEFT);
			m_ProcList->SetColumnAlign(j2, HDF_LEFT);
			m_ProcList->SetFont(Font);
		}
		//������
		for (int jj = 0; jj < nRowCount; jj++) //��
		{
			CBCGPGridRow* pNewRow = m_ProcList->CreateRow(nColCount);
			if (!pNewRow)
			{
				continue;
			}
			for (int icol = 0; icol < nColCount; icol++)
			{
				CBCGPGridItem* pItem = pNewRow->GetItem(icol);
				CAPSProp prop;
				pTableProp->GetCell(jj, icol, prop);
				_variant_t vt = prop.GetDisplayName();
				_variant_t vtd = prop.GetName();
				CBCGPGridItem* pItem1 = new CBCGPGridItem("");
				if (icol == 0)
				{
					pItem1->AddOption("����", 1);
					for (int iitem = 0; iitem < m_pProcess->GetAsmSteps()->GetCount(); iitem++)
					{
						CAsmStep * pStep = m_pProcess->GetAsmSteps()->GetAt(iitem);
						CString val;
						CAPSPropList * pRrop = pStep->GetBasePropList();
						POSITION Pos = pRrop->GetHeadPosition();
						while (Pos)
						{
							CAPSProp&  prop1 = pRrop->GetNext(Pos);

							if (prop1.GetName() == "#prop_name")
							{
								val = prop1.ConvertTo();
								break;
							}
						}
						//pItem1->AddOption(pStep->GetName(NULL), 1, pStep->GetID());
						pItem1->AddOption(val, 1, pStep->GetID());
					}
				}
				else  //�ڶ���
				{
					_variant_t vt1 = pNewRow->GetItem(0)->GetValue();
					CString stmp = vt1;
					if (stmp == "����") //��ȡ���еĹ�����ģ��
					{
						for (int k = 0; k < m_ProcPDFArray.GetSize(); k++)
						{
							pItem1->AddOption(m_ProcPDFArray.GetAt(k), 1);
						}
					}
					else
					{
						for (int k = 0; k < m_StepPDFArray.GetSize(); k++)
						{
							pItem1->AddOption(m_StepPDFArray.GetAt(k), 1);
						}
					}
				}
				pNewRow->ReplaceItem(icol, pItem1);
				pItem1->SetReadOnly(TRUE);
				pItem1->SetValue(vt);
				if (icol == 0)
					pItem1->SetData((int)vtd);
			}
			m_ProcList->AddRow(pNewRow);
		}
	}
	if (m_iType == 0)
	{
		m_ProcList->CanHideColumn(0);  //����·��ֻ��ʾһ��
		m_ProcList->SetColumnVisible(0, FALSE);
		m_ProcList->SetColumnWidth(1,rect.right-rect.left-5);
	}
#endif

	//�����м伸����ť��λ��
	CRect rcAdd(parrect.Width() / 2 - 11, parrect.Height() / 2 - 96, parrect.Width() / 2 + 11, parrect.Height() / 2 - 74);
	CWnd* pAdd = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_ADD);
	pAdd->MoveWindow(rcAdd);
	pAdd->Invalidate();

	CRect rcDelete(parrect.Width() / 2 - 11, parrect.Height() / 2 - 66, parrect.Width() / 2 + 11, parrect.Height() / 2 - 44);
	CWnd* pDelete = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_DELETE);
	pDelete->MoveWindow(rcDelete);
	pDelete->Invalidate();

	CRect rcMUp(parrect.Width() / 2 - 11, parrect.Height() / 2 - 36, parrect.Width() / 2 + 11, parrect.Height() / 2 - 14);
	CWnd* pMUp = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_MOVEUP);
	pMUp->MoveWindow(rcMUp);
	pMUp->Invalidate();

	CRect rcMDown(parrect.Width() / 2 - 11, parrect.Height() / 2 - 6, parrect.Width() / 2 + 11, parrect.Height() / 2 + 16);
	CWnd* pMDown = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_MOVEDOWN);
	pMDown->MoveWindow(rcMDown);
	pMDown->Invalidate();

	//�Ҳ�Ԥ����
	CRect PreviewRect(parrect.Width() / 2 + 15, 40, parrect.Width() - 20, parrect.bottom - 80);
	//sxq 20190110 ��ע����в��� ��װ��Adobe Reader�ٴ���Ԥ���ؼ�
	HKEY hKey;
	long lr, lr1;
	lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SoftWare\\Wow6432Node\\Adobe\\Acrobat Reader"), 0, KEY_QUERY_VALUE, &hKey);

	lr1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SoftWare\\Adobe\\Acrobat Reader"), 0, KEY_QUERY_VALUE, &hKey);

	
	if (lr == ERROR_SUCCESS || lr1 == ERROR_SUCCESS)
	{
		if (!m_ctrl.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, PreviewRect, this, 0))
		{
			//AfxMessageBox("��ȷ����װ��Adobe Reader XI64,����ģ���ļ��޷�Ԥ��");
			//MessageBox("��ȷ����װ��Adobe Reader XI64,����ģ���ļ��޷�Ԥ����",APS_MSGBOX_TITIE,MB_OK);
			//return FALSE;
		}
		else
			m_bCtrlSucc = true;
	}

	/*BOOL a = m_ctrl.LoadFile(_T("f:\\a.pdf"));
	CString t = m_ctrl.get_src();
	m_ctrl.gotoFirstPage();
	m_ctrl.ShowWindow(TRUE);
	//m_ctrl.setLayoutMode(_T("read"));
	//m_ctrl.setShowToolbar(FALSE);
	m_ctrl.setPageMode(_T("none"));*/

	//���ҳ��Ԥ����λ��
	CRect rcStaticPreview(parrect.Width() / 2 + 15, 10, parrect.Width() / 2 + 75, 30);
	CWnd* pStaticPreview = GetDlgItem(IDC_WINDCHILL_PROCRULE_STATIC_PREVIEW);
	pStaticPreview->MoveWindow(rcStaticPreview);
	pStaticPreview->Invalidate();

	//���pdf�ļ����λ��
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

	//���棬������ȡ����ť��λ��
	CRect rcSave(parrect.Width() - 300, parrect.bottom - 42, parrect.Width() - 250, parrect.bottom - 20);
	CWnd* pSave = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_SAVE);
	pSave->MoveWindow(rcSave);
	pSave->Invalidate();
	CRect rcExt(parrect.Width() - 200, parrect.bottom - 42, parrect.Width() - 150, parrect.bottom - 20);
	CWnd* pExt = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_EXPORT); 
	pExt->MoveWindow(rcExt);
	pExt->Invalidate();
	CRect rcCancel(parrect.Width() - 100, parrect.bottom - 42, parrect.Width() - 50, parrect.bottom - 20);
	CWnd* pCancel = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_CANCEL);
	pCancel->MoveWindow(rcCancel);
	pCancel->Invalidate();
	
	//��������ļ����е�Ĭ��ֵ
	//begin add xieliang in 2018.09.28 for bug 70163
	CString strPath;
	CString strPathTmp = GetFileNamePath(m_sPath);
	if (strPathTmp.GetLength() > 0)
	{
		strPath = strPathTmp;
	}

	if (m_strPath.IsEmpty())
	{
		CString sName;
		if (CWindChillSetting::m_strpartFirstName.IsEmpty())
			sName = GetMainFileName(m_sPath);
		else
			sName = CWindChillSetting::m_strpartFirstName;

		sName=changePartName(sName);
		m_strPath = strPath + sName + _T(".pdf");
	}
	
	CEdit* pEditPath = (CEdit*)GetDlgItem(IDC_WINDCHILL_PROCRULE_OUTPUTPATH);
	pEditPath->SetWindowText(m_strPath);
	return TRUE;
}

bool CDlgProcRule::IsStepInTempPlan(int iStepID, CAPSGridPropData * pPropData)
{
	int nRowCnt = pPropData->GetRowCount();
	for (int j = 0; j < nRowCnt; ++j)
	{
		CAPSProp prop;
		pPropData->GetCell(nRowCnt - j - 1, 0, prop);
		_variant_t vtd = prop.GetName();  //����ID�����ǹ�����=-1
		int iID = (int)vtd;
		if (iStepID == iID)  
		{
			return TRUE;
		}
	}
	return FALSE;
}
bool CDlgProcRule::ParseConfigFile(CString strxml, CStringArray & PDFTempLateProcArray, CStringArray & PDFTempLateStepArray)
{
	MSXML2::IXMLDOMDocument2Ptr pXmlDoc;
	//pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
	::CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void **)&pXmlDoc);
	if (!pXmlDoc)
	{
		ASSERT(FALSE);
		return false;
	}

	IXMLDOMParseErrorPtr pErr = NULL;
	pXmlDoc->setProperty("ProhibitDTD", false);
	if (pXmlDoc->load(strxml.GetBuffer()) == VARIANT_FALSE)
	{
		BSTR str;
		pErr = pXmlDoc->GetparseError();
		pErr->get_reason(&str);
		pXmlDoc.Release();
		return false;
	}

	long lCode = 0;
	pErr = pXmlDoc->validate();
	pErr->get_errorCode(&lCode);
	if (lCode != 0)
	{
		BSTR str;
		pErr->get_reason(&str);
		pXmlDoc.Release();
		return false;
	}


	// ��ȡXML�ĵ����ڵ�
	MSXML2::IXMLDOMNodeListPtr pNodeList = pXmlDoc->GetchildNodes();
	for (int i = 0; i < pNodeList->Getlength(); ++i)
	{
		MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
		CString strName = pNode->GetnodeName();
		if (strName.CompareNoCase(_T("PDFOutput")) != 0)
		{
			continue;
		}

		MSXML2::IXMLDOMNodeListPtr pGroupList = pNode->GetchildNodes();
		for (int j = 0; j < pGroupList->Getlength(); ++j)
		{
			MSXML2::IXMLDOMNodePtr pGroupNode = pGroupList->Getitem(j);
			CString strSubName = pGroupNode->GetnodeName();
			if (strSubName.CompareNoCase(_T("PageGroup")) != 0)
			{
				continue;
			}
			MSXML2::IXMLDOMNamedNodeMapPtr pNodeMap = pGroupNode->Getattributes();
			CString tmp1,tmp2;
			for (int k = 0; k < pNodeMap->Getlength(); ++k)
			{
				MSXML2::IXMLDOMNodePtr pNode1 = pNodeMap->Getitem(k);
				CString strName1 = pNode1->GetnodeName();
				CString strVal1 = pNode1->GetnodeValue();
				if (strName1.CompareNoCase(_T("PageFirst")) == 0)
				{
					tmp1 = strVal1;
				}
				else if (strName1.CompareNoCase(_T("Foreach")) == 0)
				{
					tmp2 = strVal1;
				}
				//else if (strName.CompareNoCase(_T("PageSecond")) == 0)
				//{
				//	strNoramlName = strVal;
				//}
			}
			tmp1 = tmp1.Left(tmp1.GetLength()-4);
			if (tmp2.CompareNoCase(_T("Proc")) == 0)
				m_ProcPDFArray.Add(tmp1);
			else if (tmp2.CompareNoCase(_T("Step")) == 0)
				m_StepPDFArray.Add(tmp1);
		}
	}
	pXmlDoc.Release();
	return true;
}

void CDlgProcRule::SetProcess(CProcess *pProcess)
{
	m_pProcess = pProcess;
}
void CDlgProcRule::SetProcTempConfig(CArray<CAPSGridPropData*> * pPdfConfigDatas)
{
	/*for (int i = 0; i < pPdfConfigDatas->GetCount(); i++)
	{
		CAPSGridPropData * tmp = pPdfConfigDatas->GetAt(i);
		m_PdfConfigDatas.Add(tmp);
	}*/

}
void CDlgProcRule::GetProcTempConfig(CArray<CAPSGridPropData*> * pPdfConfigDatas)
{
	/*for (int i = 0; i < m_PdfConfigDatas.GetCount();i++)
	{
		CAPSGridPropData * tmp = m_PdfConfigDatas.GetAt(i);
		pPdfConfigDatas->Add(tmp);
	}*/
}

void CDlgProcRule::OnSave()
{
	SaveConfig();
	MessageBox("ҳ�������ѱ���",APS_MSGBOX_TITIE,MB_OK);
}

void CDlgProcRule::SaveConfig()
{
	//���湤����ģ�������
	m_ConfigXMLName = m_sTemplateName;
	m_ConfigProcPDFName->clear();
	CAPSGridPropData* pData = new CAPSGridPropData();
	pData->SetGridName(m_PdfConfigDatas->GetGridName());
	
	CAPSPropList ColList;
	m_PdfConfigDatas->GetTableHeadList(ColList);
	pData->SetTableHeadList(ColList);
	//CString sGridName = m_PdfConfigDatas.GetAt(0)->GetGridName(); 
	m_PdfConfigDatas = NULL;

	//��ȡGrid������
	int iCol = m_ProcList->GetColumnCount();
	int iRow = m_ProcList->GetRowCount();
	
	for (int ii = 0; ii < iRow; ++ii)
	{
		CAPSPropList * pPropList = new CAPSPropList();
		for (int jj = 0; jj < iCol; ++jj)
		{
			CBCGPGridRow* pRow = m_ProcList->GetRow(ii);
			CBCGPGridItem* pItem = (CBCGPGridItem*)pRow->GetItem(jj);
			
			CAPSProp * a = new CAPSProp();
			if (jj == 0)
			{
				//pData->SetGridName(PLUGIN_PAGETYPE);
				CString tmp;
				tmp.Format("%d", pItem->GetData());
				a->SetName(tmp);
			}
			else
			{
				//pData->SetGridName(PLUGIN_PAGENAME);
				CString tmp = pItem->GetValue();
				tmp.Append(_T(".pdf"));
				a->SetName(tmp);
			}
			a->SetDisplayName(pItem->GetValue());
			a->SetShow(0);
			pPropList->AddTail(*a);
		}
		pData->AddRow(*pPropList);
	}
	m_PdfConfigDatas = pData;

	//m_PdfConfigDatas������process�����ݽṹ��
	CArray<CAPSGridPropData*>* pPdfConfigDatas = m_pProcess->GetProcessPdfConfigDatas();

	CAPSGridPropData* pPropData = NULL;
	for (int i = 0; i < pPdfConfigDatas->GetCount(); i++)
	{
		pPropData = pPdfConfigDatas->GetAt(i);
		CString strName = pPropData->GetGridName();
		if ((strName.CompareNoCase(m_PdfConfigDatas->GetGridName()) == 0))
		{
			pPdfConfigDatas->RemoveAt(i,1);
			break;
		}
	}
	pPdfConfigDatas->Add(m_PdfConfigDatas);
	//m_pProcess->SetProcessPdfConfigDatas(pPdfConfigDatas);
	//EndDialog(IDOK);
}


void CDlgProcRule::OnClose()
{
	EndDialog(IDCANCEL);
}

BOOL CDlgProcRule::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			//begin delete xieliang in 2018.09.28 for bug 70170
			//OnSave();
			//end delete xieliang in 2018.09.28 for bug 70170
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			OnClose();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
void CDlgProcRule::OnBnClickedAdd()
{
	CBCGPGridRow* pNewRow = m_ProcList->CreateRow(2);
	if (!pNewRow)
	{
		return;
	}
	for (int icol = 0; icol < 2; icol++)
	{
		CBCGPGridItem* pItem = pNewRow->GetItem(icol);
		CBCGPGridItem* pItem1 = new CBCGPGridItem("");
		if (icol == 0)
		{
			pItem1->AddOption("����", 1);
			for (int iitem = 0; iitem < m_pProcess->GetAsmSteps()->GetCount(); iitem++)
			{
				CAsmStep * pStep = m_pProcess->GetAsmSteps()->GetAt(iitem);
				CString val;
				CAPSPropList * pRrop = pStep->GetBasePropList();
				POSITION Pos = pRrop->GetHeadPosition();
				while (Pos)
				{
					CAPSProp&  prop = pRrop->GetNext(Pos);

					if (prop.GetName() == "#prop_name")
					{
						val = prop.ConvertTo();
						break;
					}
				}
				//pItem1->AddOption(pStep->GetName(NULL), 1);
				pItem1->AddOption(val, 1);
			}
			pItem1->SetValue("����");
			pItem1->SetData(-1);
		}
		else  //�ڶ���
		{
			_variant_t vt1 = pNewRow->GetItem(0)->GetValue();
			CString stmp = vt1;
			if (stmp == "����") //��ȡ���еĹ�����ģ��
			{
				for (int k = 0; k < m_ProcPDFArray.GetSize(); k++)
				{
					pItem1->AddOption(m_ProcPDFArray.GetAt(k), 1);
				}
				_variant_t tmp = m_ProcPDFArray.GetAt(0);
				pItem1->SetValue(tmp);
			}
			else
			{
				for (int k = 0; k < m_StepPDFArray.GetSize(); k++)
				{
					pItem1->AddOption(m_StepPDFArray.GetAt(k), 1);
				}
			}
		}
		pNewRow->ReplaceItem(icol, pItem1);
		pItem1->SetReadOnly(TRUE);
	}
	m_ProcList->AddRow(pNewRow);
}


void CDlgProcRule::OnBnClickedDelete()
{
	CList<CBCGPGridItem*, CBCGPGridItem*> lstItem;
	m_ProcList->GetSelectedItems(lstItem);
	//begin add xieliang in 2018.09.28 for bug 70165
	if (lstItem.GetCount() == 0)
		return;
	//end add xieliang in 2018.09.28 for bug 70165
	std::set<int> setRowId;
	for(int iIndex=0;iIndex<lstItem.GetCount();iIndex++)
	{
		CBCGPGridItem* pGridItem = lstItem.GetAt(lstItem.FindIndex(iIndex));
		if (!pGridItem)
		{
			continue;
		}
		CBCGPGridRow* pGridRow = pGridItem->GetParentRow();
		if (!pGridRow)
		{
			continue;
		}
		//m_ProcList->RemoveRow(pGridRow->GetRowId());
		setRowId.insert(pGridRow->GetRowId());
	}

	if(setRowId.size()<=0)
		return;

	std::set<int>::iterator itr;
	for(itr=setRowId.end();itr!=setRowId.begin();)
	{
		itr--;
		m_ProcList->RemoveRow(*itr);
	}
}
void CDlgProcRule::OnBnClickedMoveUp()
{
	bool bRedraw = 1;
	CBCGPGridRow* pCurRow = NULL;
	int iMax = m_ProcList->GetRowCount(0) - 1;
	int iCount = m_ProcList->GetSelectionCount();
	if (iCount == 1)
	{
		pCurRow = m_ProcList->GetCurSel();
	}
	if (pCurRow)
	{
		int iRow = pCurRow->GetRowId();
		if (iRow == 0)
		{
			return;
		}
		CBCGPGridRow* pNewRow = new CBCGPGridRow;
		m_ProcList->InsertRowBefore(iRow - 1, pNewRow, bRedraw);
		int iItemCount = pCurRow->GetItemCount();
		for (int i = 0; i < iItemCount; ++i)
		{
			CBCGPGridItem* pNewItem = pNewRow->CreateItem(iRow - 1, i);
			pNewRow->AddItem(pNewItem);

			CBCGPGridItem* pItem = pCurRow->GetItem(i);
			int iOptionCount = pItem->GetOptionCount();
			_variant_t varVal = pItem->GetValue();
			if (iOptionCount > 0)
			{
				for (int j = 0; j < iOptionCount; ++j)
				{
					LPCTSTR szOptionData = pItem->GetOption(j);
					pNewItem->AddOption(szOptionData);
				}
				pNewItem->SetValue((_bstr_t)varVal);
				if (i == 0)
					pNewItem->SetData(pItem->GetData());
			}
			else
			{
				pNewItem->SetValue(varVal, bRedraw);
				if (i == 0)
					pNewRow->ReplaceItem(0, new CBCGPGridCheckItem(varVal));
			}
		}
		m_ProcList->RemoveRow(iRow + 1, bRedraw);
		m_ProcList->AdjustLayout();
		m_ProcList->SetCurSel(iRow - 1);
	}

}

void CDlgProcRule::OnBnClickedMoveDown()
{
	bool bRedraw = 1;
	CBCGPGridRow* pCurRow = NULL;
	int iMax = m_ProcList->GetRowCount(0) - 1;
	int iCount = m_ProcList->GetSelectionCount();
	if (iCount == 1)
	{
		pCurRow = m_ProcList->GetCurSel();
	}
	if (pCurRow)
	{
		int iRow = pCurRow->GetRowId();
		if (iRow == iMax)
		{
			return;
		}
		CBCGPGridRow* pNewRow = new CBCGPGridRow;
		m_ProcList->InsertRowAfter(iRow + 1, pNewRow, bRedraw);
		int iItemCount = pCurRow->GetItemCount();
		for (int i = 0; i < iItemCount; ++i)
		{
			CBCGPGridItem* pNewItem = pNewRow->CreateItem(iRow + 1, i);
			pNewRow->AddItem(pNewItem);

			CBCGPGridItem* pItem = pCurRow->GetItem(i);
			int iOptionCount = pItem->GetOptionCount();
			_variant_t varVal = pItem->GetValue();
			if (iOptionCount > 0)
			{
				for (int j = 0; j < iOptionCount; ++j)
				{
					LPCTSTR szOptionData = pItem->GetOption(j);
					pNewItem->AddOption(szOptionData);
				}
				pNewItem->SetValue((_bstr_t)varVal);
				if (i == 0)
					pNewItem->SetData(pItem->GetData());

			}
			else
			{
				pNewItem->SetValue(varVal, bRedraw);
				if (i == 0)
					pNewRow->ReplaceItem(0, new CBCGPGridCheckItem(varVal));
			}
		}
		m_ProcList->RemoveRow(iRow, bRedraw);
		m_ProcList->AdjustLayout();
		m_ProcList->SetCurSel(iRow + 1);
	}
}

// CDlgProcRule ��Ϣ�������
void CDlgProcRule::OnExport()
{
	SaveConfig();
	
	int iRow = m_ProcList->GetRowCount();
	if (iRow<=0)
	{
		//AfxMessageBox("ҳ�����ò���Ϊ��!");
		MessageBox("ҳ�����ò���Ϊ�գ�",APS_MSGBOX_TITIE,MB_OK);
		return;
	}

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
		//AfxMessageBox("���·������!");
		MessageBox("���·������",APS_MSGBOX_TITIE,MB_OK);
		return;
	}

	bool bOpenFile = false;
	CFile file;
	file.Open(m_sOutputFilePath, CFile::modeReadWrite);
	if (file.m_hFile != CFile::hFileNull)
	{//�ļ��򿪣�
		bOpenFile = false;
	}
	else
	{
		bOpenFile = true;
	}
	BOOL bExist = PathFileExists(m_sOutputFilePath);
	if (bExist)
	{
#if 0
		//begin add xieliang in 2018.09.28 for bug 70168
		AfxMessageBox("��ǰ·������ͬ�����ļ�,����������!");
		//end add xieliang in 2018.09.28 for bug 70168
		return;
#else
		if (IDNO == MessageBox("��ǰ·������ͬ�����ļ�,�Ƿ񸲸�?", APS_MSGBOX_TITIE, MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}
		else
		{
			if (bOpenFile)
			{
				MessageBox("�ļ��Ѵ򿪣���ر�!", APS_MSGBOX_TITIE, MB_ICONWARNING);
				return;
			}
		}
#endif
	}
	m_sLoadFilePath = m_sTemplateName;
	//CWindChillSetting::SetPdfPath_Route(m_sOutputFilePath);

	//CButton *pCheck = (CButton *)GetDlgItem(IDC_CHECK_ACTION);
	//if (pCheck->GetCheck())
	//	m_bExportAction = true;
	//else
	//	m_bExportAction = false;

	EndDialog(IDOK);
}

void CDlgProcRule::OnDir()
{
	//�Ȼ�ȡ�ļ���
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
void CDlgProcRule::GetStepTemp(CStringArray &StrStepTemp)
{
	CString strIni = GetModuleDir() + _T("\\PDFTemplatePlan.ini");
	TCHAR chSectionnames[2048] = { 0 };
	char * pSectionName;
	int i;
	int j = 0;
	int count = 0;
	::GetPrivateProfileSectionNames(chSectionnames, 2048, strIni);
	for (i = 0; i < 2048; i++, j++)
	{
		if (chSectionnames[0] == '\0')
			break;
		if (chSectionnames[i] == '\0')
		{
			pSectionName = &chSectionnames[i - j];
			count++;
			j = -1;
			if (strcmp(pSectionName, _T("���չ��")) != 0)
				continue;
			CStringArray arKeys;
			EnumIniKeys(strIni, pSectionName, arKeys);
			for (INT_PTR ii = 0; ii < arKeys.GetSize(); ii++)
			{
				CString strVal;
				CString strKey = arKeys.GetAt(ii);
				GetPrivateProfileString(pSectionName, strKey, _T(""), strVal.GetBuffer(512), 512, strIni);
				strVal.ReleaseBuffer();
				if (strKey.CompareNoCase(_T("����")) != 0)
					continue;
				CString TemplateAry = _T("PDFTemplate");
				CString strFullPath;// = KmAppSettings::GetConfigFileFullPath(TemplateAry);
				strFullPath.Format(_T("%s%s%s"), GetModuleDir(), _T("\\Resources\\Chs\\"), TemplateAry);
				strFullPath.Append(_T("\\"));
				strFullPath.Append(strVal);
				strFullPath.Append(_T(".pdf"));
				BOOL bExist = PathFileExists(strFullPath);
				if (bExist)
					StrStepTemp.Add(strVal);
			}

			if (chSectionnames[i + 1] == 0)
			{
				break;
			}
		}
	}
}
void CDlgProcRule::OnSelectTemlate()
{
	int a = 0;
}
LRESULT CDlgProcRule::OnSelChanged(WPARAM wp, LPARAM lp)
{
	//BCGPGRID_ITEM_INFO * iteminfo = (BCGPGRID_ITEM_INFO*)lp;
	//int irow = iteminfo->nRow;
	//int icol = iteminfo->nCol;
	//CBCGPGridRow *pRow = m_ProcList.GetCurSel();
	if (m_ProcList == (CWnd *)lp)
	{
		CBCGPGridRow *pRow = m_ProcList->GetCurSel();
		if (!pRow)
			return 0;
		CBCGPGridItem * pItem1 = m_ProcList->GetCurSelItem();
		if (pRow->IsSelected() && pItem1 == NULL)//ѡ�е������ж�����һ��item
		{
			CBCGPGridItem* pItem = (CBCGPGridItem*)pRow->GetItem(1);
			if (pItem)
			{
				CString TemplateAry1 = _T("PDFTemplate\\");
				CString sPath1;// = KmAppSettings::GetConfigFileFullPath(TemplateAry1);
				sPath1.Format(_T("%s%s%s"), GetModuleDir(), _T("\\Resources\\Chs\\"), TemplateAry1);
				CString tmp = pItem->GetValue();
				sPath1.Append(tmp);
				sPath1.Append(".pdf");
				if (PathFileExists(sPath1) && m_bCtrlSucc)
				{
					BOOL a = m_ctrl.LoadFile(sPath1);
					CString t = m_ctrl.get_src();
					m_ctrl.gotoFirstPage();
					m_ctrl.ShowWindow(TRUE);
					//m_ctrl.setLayoutMode(_T("read"));
					//m_ctrl.setShowToolbar(FALSE);
					m_ctrl.setPageMode(_T("fullscreen"));
				}
				//m_ProcList.SetCurSel(pRow);
			}	
		}
	}
	return 0;
}
LRESULT CDlgProcRule::OnItemChanged(WPARAM wp, LPARAM lp)
{
	BCGPGRID_ITEM_INFO * iteminfo = (BCGPGRID_ITEM_INFO*)lp;

	int irow = iteminfo->nRow;
	int icol = iteminfo->nCol;

	CBCGPGridRow* pRow = m_ProcList->GetRow(irow);
	CBCGPGridItem* pItem = (CBCGPGridItem*)pRow->GetItem(icol);
	if (icol == 0) //�޸ĵ���ҳ�����ͣ�Ҫ��stepid����
	{
		//����·��ֻ���ǹ����࣬û�в�����
		if (m_iType == 0)
		{
			pItem->SetValue("����");
			pItem->SetData(-1);
		}
		else
		{
			//�����ַ������Ҳ���ID
			std::map<CString, int>::const_iterator pIt = m_pPageTypeMap.find(pItem->GetValue());
			if (pIt != m_pPageTypeMap.end())
			{
				pItem->SetData(pIt->second);
			}
			//���µڶ��е�option��ֵ
			CBCGPGridItem* pItem2 = (CBCGPGridItem*)pRow->GetItem(1);
			pItem2->RemoveAllOptions();
			if (pItem->GetData() == -1) //��ȡ���еĹ�����ģ��
			{
				for (int k = 0; k < m_ProcPDFArray.GetSize(); k++)
				{
					pItem2->AddOption(m_ProcPDFArray.GetAt(k), 1);
				}
				_variant_t tmp = m_ProcPDFArray.GetAt(0);
				pItem2->SetValue(tmp);
			}
			else
			{
				for (int k = 0; k < m_StepPDFArray.GetSize(); k++)
				{
					pItem2->AddOption(m_StepPDFArray.GetAt(k), 1);
				}
				_variant_t tmp = m_StepPDFArray.GetAt(0);
				pItem2->SetValue(tmp);
			}
		}
	}
	return 0;
}
LRESULT CDlgProcRule::OnItemDblClk(WPARAM wp, LPARAM lp)
{
	return 0;
}

void CDlgProcRule::OnSize(UINT nType, int cx, int cy)
{
	CRect rc1, parrect;
	GetClientRect(&parrect);
	if (parrect.Width() <= 698)  //���ҳ���Ԥ�������ռһ��
	{
		//�������ҳ���λ��
		CRect rect(10, 40, parrect.Width() / 2 - 15, parrect.bottom - 80);
		rc1 = rect;
	}
	else  //���ҳ���λ�ù̶�Ϊ350
	{
		CRect rect(10, 40, 350, parrect.bottom - 80);
		rc1 = rect;
	}

	if (m_ProcList)
	{
		m_ProcList->MoveWindow(rc1);
		if (m_iType == 0)
		{
			m_ProcList->SetColumnWidth(1, rc1.right - rc1.left - 5);
		}
		if (m_iType == 1)
		{
			m_ProcList->SetColumnWidth(0, (rc1.right - rc1.left) / 2 - 5);
			m_ProcList->SetColumnWidth(1, (rc1.right - rc1.left) / 2 - 5);
		}
	}
	
	//�����м伸����ť��λ��
	CRect rcAdd(rc1.right + 4, parrect.Height() / 2 - 96, rc1.right + 26, parrect.Height() / 2 - 74);
	CWnd* pAdd = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_ADD);
	if (!pAdd)
		return;
	pAdd->MoveWindow(rcAdd);
	pAdd->Invalidate();

	CRect rcDelete(rc1.right + 4, parrect.Height() / 2 - 66, rc1.right + 26, parrect.Height() / 2 - 44);
	CWnd* pDelete = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_DELETE);
	if (!pDelete)
		return;
	pDelete->MoveWindow(rcDelete);
	pDelete->Invalidate();

	CRect rcMUp(rc1.right + 4, parrect.Height() / 2 - 36, rc1.right + 26, parrect.Height() / 2 - 14);
	CWnd* pMUp = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_MOVEUP);
	if (!pMUp)
		return;
	pMUp->MoveWindow(rcMUp);
	pMUp->Invalidate();

	CRect rcMDown(rc1.right + 4, parrect.Height() / 2 - 6, rc1.right + 26, parrect.Height() / 2 + 16);
	CWnd* pMDown = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_MOVEDOWN);
	if (!pMDown)
		return;
	pMDown->MoveWindow(rcMDown);
	pMDown->Invalidate();

	//�Ҳ�Ԥ����
	if (m_bCtrlSucc)
	{
		CRect PreviewRect(rc1.right + 30, 40, parrect.Width() - 20, parrect.bottom - 80);
		if (!m_ctrl)
			return;
		m_ctrl.MoveWindow(PreviewRect);
		m_ctrl.Invalidate();
	}

	//���ҳ��Ԥ����λ��
	CRect rcStaticPreview(rc1.right + 30, 10, rc1.right + 90, 30);
	CWnd* pStaticPreview = GetDlgItem(IDC_WINDCHILL_PROCRULE_STATIC_PREVIEW);
	if (!pStaticPreview)
		return;
	pStaticPreview->MoveWindow(rcStaticPreview);
	pStaticPreview->Invalidate();

	//���pdf�ļ����λ��
	CRect rcStaticText(10, parrect.bottom - 70, 66, parrect.bottom - 48);
	CWnd* pStaticText = GetDlgItem(IDC_WINDCHILL_PROCRULE_STATIC_TEXT);
	if (!pStaticText)
		return;
	pStaticText->MoveWindow(rcStaticText);
	pStaticText->Invalidate();
	CRect rcText(70, parrect.bottom - 72, parrect.right - 50 , parrect.bottom - 50);
	CWnd* pText = GetDlgItem(IDC_WINDCHILL_PROCRULE_OUTPUTPATH);
	if (!pText)
		return;
	pText->MoveWindow(rcText);
	pText->Invalidate();
	CRect rcPath(parrect.right - 37, parrect.bottom - 72, parrect.right - 15, parrect.bottom - 50);
	CWnd* pPath = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_PDFPATH);
	if (!pPath)
		return;
	pPath->MoveWindow(rcPath);
	pPath->Invalidate();

	//���棬������ȡ����ť��λ��
	CRect rcSave(parrect.Width() - 300, parrect.bottom - 42, parrect.Width() - 250, parrect.bottom - 20);
	CWnd* pSave = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_SAVE);
	if (!pSave)
		return;
	pSave->MoveWindow(rcSave);
	pSave->Invalidate();
	CRect rcExt(parrect.Width() - 200, parrect.bottom - 42, parrect.Width() - 150, parrect.bottom - 20);
	CWnd* pExt = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_EXPORT);
	if (!pExt)
		return;
	pExt->MoveWindow(rcExt);
	pExt->Invalidate();
	CRect rcCancel(parrect.Width() - 100, parrect.bottom - 42, parrect.Width() - 50, parrect.bottom - 20);
	CWnd* pCancel = GetDlgItem(IDC_WINDCHILL_PROCRULE_BTN_CANCEL);
	if (!pCancel)
		return;
	pCancel->MoveWindow(rcCancel);
	pCancel->Invalidate();

}


