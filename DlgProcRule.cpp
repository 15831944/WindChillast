// DlgExportPDF.cpp : 实现文件
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


#define PLUGIN_PAGETYPE "页面类型"
#define PLUGIN_PAGENAME "页面名称"
#define PLUGIN_PROCRULEXML "工艺规程模板.xml"
// CDlgProcRule 对话框

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
	//test pdf review 64位不行
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
		//KmMessageBox(ResourceString(IDS_APM_PDF_NO_TEMPLATE_FILE, _T("找不到PDF模板文件，请确认.")),APS_MSGBOX_TITIE,MB_ICONERROR);
		//AfxMessageBox("找不到PDF模板文件，请确认!");
		MessageBox("找不到PDF模板文件，请确认！",APS_MSGBOX_TITIE,MB_OK);
		EndDialog(S_FALSE);
		return FALSE;
	}
	m_ProcPDFArray.RemoveAll();
	m_StepPDFArray.RemoveAll();
	auto flag =ParseConfigFile(m_sTemplateName, m_ProcPDFArray, m_StepPDFArray);
	if (!flag)
	{
		MessageBox("模板文件有误，请确认！", APS_MSGBOX_TITIE, MB_OK);
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
	m_pPageTypeMap.insert(std::make_pair("工艺", -1));
	
	//创建左侧的添加页面控件
	m_ProcList = new CMyGridCtrl;
	m_ProcList->CreateCtrl(this);
	CRect parrect;
	GetClientRect(&parrect);
	CFont* Font = GetFont();
	CRect rect;
	m_ProcList->GetClientRect(&rect);
	
#if 1
	//此处逻辑
	//if (process中数据不为空)
	//	获取process中数据
	//else
	//配置默认的初始数据
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
	if (bStored)  //工艺过程里有模板方案
	{
		//工艺路线可以直接copy 工艺过程中的模板方案。工艺规程还要根据是否新增删除了步骤来修改模板方案。
		if (m_iType == 0)
			//SetProcTempConfig(pPdfConfigDatas);
			m_PdfConfigDatas = pPropData;
		else
		{
			//若是已从样例中删除的步骤，则相应从模板方案中删除该步骤下的页面配置。
			int nRowCnt = pPropData->GetRowCount();
			for (int j = 0; j < nRowCnt; ++j)
			{
				CAPSProp prop;
				pPropData->GetCell(nRowCnt - j - 1, 0, prop);
				_variant_t vtd = prop.GetName();  //步骤ID。若是工艺则=-1
				int iStepID = (int)vtd;
				if (iStepID != -1 && m_pProcess->GetAsmStep(iStepID) == NULL)  //步骤已删除
				{
					pPropData->DelRow(nRowCnt - j - 1);
				}
			}
			//然后遍历当前样例的所有步骤，与模板方案进行比对，获取到新添加的步骤列表（同时记录前置步骤）供后续使用。
			//该方法不准确。可能模板方案中删除的步骤又被统计出来
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
			if (map.size() > 0) //新添加了步骤
			{
				GetStepTemp(StrStepTemp);
			}
			//遍历模板方案，当遍历到某个新步骤的前置步骤时，则根据默认模板方案的配置，在其前置步骤后插入新步骤和模板。
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
					_variant_t vtd = prop.GetName();  //步骤ID。若是工艺则=-1
					int iStepID = (int)vtd;
					if (iStepID == -1 && !(idx == nRowCnt - 1 && iPrevStepID == -1)) //工艺类模板略过
						continue;
					if (iPrevStepID == -1) //表示新增加的步骤是第一个步骤
					{
						//从默认配置里获取到步骤上的模板
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
						if (iPrevStepID == iStepID)  //找到了前置步骤
						{
							//获取前置步骤配置的模板数量
							iPrevCnt++;
							continue;
						}
						else if (iPrevCnt != 0)
						{
							//从默认配置里获取到步骤上的模板
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
			//SetProcTempConfig(pPdfConfigDatas);   //???存储的是修改过的吗
			m_PdfConfigDatas = pPropData;
		}
	}
	else  //读取默认配置
	{
		pPropData = new CAPSGridPropData();
		//设置网格名称
		if (m_iType == 0)
			pPropData->SetGridName("#proc_line");
		else if (m_iType == 1)
			pPropData->SetGridName("#proc_rule");
		CAPSPropList * pColList = new CAPSPropList();
		//设置第一列列名
		CAPSProp * a = new CAPSProp();
		a->SetName(PLUGIN_PAGETYPE);
		a->SetDisplayName(PLUGIN_PAGETYPE);
		pColList->AddTail(*a);
		//设置第二列列名
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
				if ((m_iType == 1 && strcmp(pSectionName, _T("工艺规程")) != 0) || (m_iType == 0 && strcmp(pSectionName, _T("工艺路线")) != 0))
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
							if (strKey.CompareNoCase(_T("工艺")) == 0)
							{
								pPropList->RemoveAll();
								a->SetName("-1");
								a->SetDisplayName("工艺");
								pPropList->AddTail(*a);
								a->SetName(arrCellVal.GetAt(j1) + ".pdf");  //pdf的全路径
								a->SetDisplayName(arrCellVal.GetAt(j1));
								pPropList->AddTail(*a);
								pPropData->AddRow(*pPropList);
							}

							if (strKey.CompareNoCase(_T("工序")) == 0)
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
		//SetProcTempConfig(pPdfConfigDatas);   //???存储的是修改过的吗
		m_PdfConfigDatas = pPropData;
	}

	//for (int i1 = 0; i1 < m_PdfConfigDatas.GetCount(); i1++)
	{
		CAPSGridPropData * pTableProp = m_PdfConfigDatas;
		int nRowCount = pTableProp->GetRowCount();
		int nColCount = pTableProp->GetColumnCount();

		// 插入列
		for (int j2 = 0; j2 < nColCount; j2++)
		{
			CAPSProp prop;
			pTableProp->GetColumn(j2, prop);
			m_ProcList->InsertColumn(j2, prop.GetName(), (rect.right - rect.left) / 2 - 5);
			m_ProcList->SetHeaderAlign(j2, HDF_LEFT);
			m_ProcList->SetColumnAlign(j2, HDF_LEFT);
			m_ProcList->SetFont(Font);
		}
		//插入行
		for (int jj = 0; jj < nRowCount; jj++) //行
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
					pItem1->AddOption("工艺", 1);
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
				else  //第二列
				{
					_variant_t vt1 = pNewRow->GetItem(0)->GetValue();
					CString stmp = vt1;
					if (stmp == "工艺") //获取所有的工艺类模板
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
		m_ProcList->CanHideColumn(0);  //工艺路线只显示一列
		m_ProcList->SetColumnVisible(0, FALSE);
		m_ProcList->SetColumnWidth(1,rect.right-rect.left-5);
	}
#endif

	//设置中间几个按钮的位置
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

	//右侧预览框
	CRect PreviewRect(parrect.Width() / 2 + 15, 40, parrect.Width() - 20, parrect.bottom - 80);
	//sxq 20190110 在注册表中查找 安装了Adobe Reader再创建预览控件
	HKEY hKey;
	long lr, lr1;
	lr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SoftWare\\Wow6432Node\\Adobe\\Acrobat Reader"), 0, KEY_QUERY_VALUE, &hKey);

	lr1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SoftWare\\Adobe\\Acrobat Reader"), 0, KEY_QUERY_VALUE, &hKey);

	
	if (lr == ERROR_SUCCESS || lr1 == ERROR_SUCCESS)
	{
		if (!m_ctrl.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, PreviewRect, this, 0))
		{
			//AfxMessageBox("请确保安装了Adobe Reader XI64,否则模板文件无法预览");
			//MessageBox("请确保安装了Adobe Reader XI64,否则模板文件无法预览！",APS_MSGBOX_TITIE,MB_OK);
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

	//输出页面预览的位置
	CRect rcStaticPreview(parrect.Width() / 2 + 15, 10, parrect.Width() / 2 + 75, 30);
	CWnd* pStaticPreview = GetDlgItem(IDC_WINDCHILL_PROCRULE_STATIC_PREVIEW);
	pStaticPreview->MoveWindow(rcStaticPreview);
	pStaticPreview->Invalidate();

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

	//保存，导出，取消按钮的位置
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
	
	//设置输出文件框中的默认值
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
		_variant_t vtd = prop.GetName();  //步骤ID。若是工艺则=-1
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


	// 读取XML文档根节点
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
	MessageBox("页面配置已保存",APS_MSGBOX_TITIE,MB_OK);
}

void CDlgProcRule::SaveConfig()
{
	//保存工序类模板的配置
	m_ConfigXMLName = m_sTemplateName;
	m_ConfigProcPDFName->clear();
	CAPSGridPropData* pData = new CAPSGridPropData();
	pData->SetGridName(m_PdfConfigDatas->GetGridName());
	
	CAPSPropList ColList;
	m_PdfConfigDatas->GetTableHeadList(ColList);
	pData->SetTableHeadList(ColList);
	//CString sGridName = m_PdfConfigDatas.GetAt(0)->GetGridName(); 
	m_PdfConfigDatas = NULL;

	//获取Grid的数据
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

	//m_PdfConfigDatas保存至process的数据结构中
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
			pItem1->AddOption("工艺", 1);
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
			pItem1->SetValue("工艺");
			pItem1->SetData(-1);
		}
		else  //第二列
		{
			_variant_t vt1 = pNewRow->GetItem(0)->GetValue();
			CString stmp = vt1;
			if (stmp == "工艺") //获取所有的工艺类模板
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

// CDlgProcRule 消息处理程序
void CDlgProcRule::OnExport()
{
	SaveConfig();
	
	int iRow = m_ProcList->GetRowCount();
	if (iRow<=0)
	{
		//AfxMessageBox("页面配置不能为空!");
		MessageBox("页面配置不能为空！",APS_MSGBOX_TITIE,MB_OK);
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
		//AfxMessageBox("输出路径错误!");
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
#if 0
		//begin add xieliang in 2018.09.28 for bug 70168
		AfxMessageBox("当前路径存在同名的文件,请重新命名!");
		//end add xieliang in 2018.09.28 for bug 70168
		return;
#else
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
			if (strcmp(pSectionName, _T("工艺规程")) != 0)
				continue;
			CStringArray arKeys;
			EnumIniKeys(strIni, pSectionName, arKeys);
			for (INT_PTR ii = 0; ii < arKeys.GetSize(); ii++)
			{
				CString strVal;
				CString strKey = arKeys.GetAt(ii);
				GetPrivateProfileString(pSectionName, strKey, _T(""), strVal.GetBuffer(512), 512, strIni);
				strVal.ReleaseBuffer();
				if (strKey.CompareNoCase(_T("工序")) != 0)
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
		if (pRow->IsSelected() && pItem1 == NULL)//选中的是整行而不是一个item
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
	if (icol == 0) //修改的是页面类型，要把stepid更新
	{
		//工艺路线只能是工艺类，没有步骤类
		if (m_iType == 0)
		{
			pItem->SetValue("工艺");
			pItem->SetData(-1);
		}
		else
		{
			//根据字符串查找步骤ID
			std::map<CString, int>::const_iterator pIt = m_pPageTypeMap.find(pItem->GetValue());
			if (pIt != m_pPageTypeMap.end())
			{
				pItem->SetData(pIt->second);
			}
			//更新第二列的option的值
			CBCGPGridItem* pItem2 = (CBCGPGridItem*)pRow->GetItem(1);
			pItem2->RemoveAllOptions();
			if (pItem->GetData() == -1) //获取所有的工艺类模板
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
	if (parrect.Width() <= 698)  //添加页面和预览界面各占一半
	{
		//设置添加页面的位置
		CRect rect(10, 40, parrect.Width() / 2 - 15, parrect.bottom - 80);
		rc1 = rect;
	}
	else  //添加页面的位置固定为350
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
	
	//设置中间几个按钮的位置
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

	//右侧预览框
	if (m_bCtrlSucc)
	{
		CRect PreviewRect(rc1.right + 30, 40, parrect.Width() - 20, parrect.bottom - 80);
		if (!m_ctrl)
			return;
		m_ctrl.MoveWindow(PreviewRect);
		m_ctrl.Invalidate();
	}

	//输出页面预览的位置
	CRect rcStaticPreview(rc1.right + 30, 10, rc1.right + 90, 30);
	CWnd* pStaticPreview = GetDlgItem(IDC_WINDCHILL_PROCRULE_STATIC_PREVIEW);
	if (!pStaticPreview)
		return;
	pStaticPreview->MoveWindow(rcStaticPreview);
	pStaticPreview->Invalidate();

	//输出pdf文件框的位置
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

	//保存，导出，取消按钮的位置
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


