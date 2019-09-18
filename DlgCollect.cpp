
#include "stdafx.h"
#include <direct.h>
#include <afxinet.h>
#include "common.h"

#include "APSProp.h"
#include "Solid.h"
#include "Actionbase.h"
#include "AsmStep.h"
#include "CProduct.h"
#include "apsprocess.h"
#include "Equips.h"
#include "APSModel.h"
#include "AsmInst.h"

#include "WindChill_i.h"
#include "WindChillObject.h"
#include "WindChillSetting.h"

#include "UtilityFunction.h"
#include "APSWaitCursor.h"

#include "DlgCollect.h"

#include "PDFData.h"
#include "PDFConfig.h"

#include "Output3DPdf.h"
#include "tinyxml.h"

#include "PdfDataToXMl.h"
#include "KmAppSettings.h"
CDlgCollect::CDlgCollect(IAPSApplication* pApplication,CWnd* pParent )
	: CDialog(CDlgCollect::IDD, pParent)
{
	m_pApplication = pApplication;
	m_putpdf = NULL;

	// 临时目录
	TCHAR TempDir[1024] = {};
	GetTempPath(1024, TempDir);
	CString sTempDir(TempDir);
	sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");


	path = sTempDir+"save.xml";
}


CDlgCollect::~CDlgCollect()
{
	
	if (m_putpdf)
	{
		delete m_putpdf;
		m_putpdf = NULL;
	}
	
	m_dlgInfo.DestroyWindow();
	for (auto it = m_dlgGrid.begin(); it != m_dlgGrid.end(); ++it)
	{
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}
	m_dlgGrid.clear();
	
	m_dlgChangeGrid.DestroyWindow();
}

void CDlgCollect::InitCombox()
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	CEdit *pEdit = (CEdit *)(m_combox.GetWindow(GW_CHILD));
	pEdit->SetReadOnly(TRUE);
	if (pSetting)
	{
		for (int i = 0; i < pSetting->m_strCollectionMatch.GetCount(); i++)
		{
			CString strName  = pSetting->m_strCollectionMatch.GetAt(i).m_strName;
			CString strValue = pSetting->m_strCollectionMatch.GetAt(i).m_strValue;

			if (!strName.IsEmpty())
			{
				m_combox.AddString(strName);

				m_Type[strName] = strValue;
			}
		}
	}
	
	m_combox.SetCurSel(0);
	
}

void CDlgCollect::InitTab()
{
	m_TabCtrl.DeleteAllItems();

	auto sel = m_combox.GetCurSel();
	if (sel != -1)
	{
		CString text;
		m_combox.GetLBText(sel, text);

		for (auto it = m_PageConfig.begin(); it != m_PageConfig.end(); ++it)
		{
			auto PageName = it->first;

			if (PageName.CompareNoCase(text) == 0)
			{
				auto PageInfo = it->second;
				auto Firstpage = PageInfo->m_pFirstPage;

				m_TabCtrl.InsertItem(0, _T("基本信息"));
				auto Pagedata = m_PageData[PageName];
				m_dlgInfo.InitPage(Firstpage, Pagedata);
				
				
				m_dlgInfo.m_hWnd = m_dlgInfo.GetSafeHwnd();
				if (!m_dlgInfo.GetSafeHwnd())
				{
					m_dlgInfo.Create(IDD_WINDCHILL_INFO, &m_TabCtrl);
				}
				
			
				CRect rect;
				GetClientRect(&rect);

				rect.top += 21;
				rect.left += 1;
				rect.right -= 1;
				rect.bottom -= 1;

			
				m_dlgInfo.MoveWindow(&rect,true);
				m_dlgInfo.ShowWindow(SW_SHOW);
				
				//初始化 网格属性  将所有页面的网格属性汇集到一起
				for (auto i = 0; i < Firstpage->m_arConGrid.GetSize(); ++i)
				{
					//读取网格属性配置  
					auto GridInfo = Firstpage->m_arConGrid.GetAt(i);
					if (GridInfo->m_strType.CompareNoCase("更改标记")==0)
					{
						continue;
					}
					std::vector<GridData *> vGrid;
					
					auto PageInfo =m_PageData[PageName];
					for (auto j=0;j<PageInfo.size();++j)
					{
						auto tempPage =PageInfo[j];
						
						if (i < tempPage->m_arGrid.GetSize())
						{
							auto pgrid = tempPage->m_arGrid.GetAt(i);
							vGrid.push_back(pgrid);
						}
					}

					m_TabCtrl.InsertItem(i + 1,_T("网格属性"));
					auto dlgGrid = new CDlgGrid;
					m_dlgGrid[i + 1] = dlgGrid;

					dlgGrid->InitPage(GridInfo, vGrid);
					if (!dlgGrid->GetSafeHwnd())
					{
						dlgGrid->Create(IDD_WINDCHILL_GRID, &m_TabCtrl);
					}
					dlgGrid->MoveWindow(&rect);
					dlgGrid->ShowWindow(SW_HIDE);
				}

				m_TabCtrl.InsertItem(99, _T("更改标记"));

				m_dlgChangeGrid.InitPage(Firstpage, Pagedata);
				if (!m_dlgChangeGrid.GetSafeHwnd())
				{
					m_dlgChangeGrid.Create(IDD_WINDCHILL_CHANGGRID, &m_TabCtrl);
				}
				
				m_dlgChangeGrid.MoveWindow(&rect);
				m_dlgChangeGrid.ShowWindow(SW_HIDE);
			}
		}
	}
	
	m_TabCtrl.ShowWindow(SW_SHOW);
	m_TabCtrl.SetCurSel(0);
}

void CDlgCollect::GetPdfInfo()
{
	if (m_putpdf)
	{
		delete m_putpdf;
		m_putpdf = NULL;
	}
	
	CString strDllPath = GetModuleDir()+ _T("\\Output3Dpdf.dll");

	CString StrXmlPath = GetModuleDir() + _T("\\Resources\\Chs\\PDFTemplate\\工艺规程模板.xml");
/*
	IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
	if (!doc)
	{
		ASSERT(FALSE);

	}
	
	
	
	CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();

	m_putpdf = new COutPut3DPdf;
	
	//先清空配置
	m_putpdf->ClearXMLConfig();

	CProcess* pProcess = pModel->GetProcess();

	m_putpdf->SetParam(pModel, pProcess, StrXmlPath, "");

	int nSign = 0;
	//解析xml格式
	BOOL bReadXml = m_putpdf->ReadCfg(nSign);
	if (!bReadXml)
	{
		return;
	}
	
	//根据xml及获取的数据，设置页面格式
	BOOL bDataForm = m_putpdf->Convert(bReadXml, true, 0, 0);
	if (!bDataForm)
	{
		return;
	}
	
	CString TemplateAry = _T("PDFTemplate");
	CString sPath = KmAppSettings::GetConfigFileFullPath(TemplateAry);
	std::vector<CString> vctPDFName;
	CFileFind finder;
	BOOL working = finder.FindFile(sPath + "\\*.pdf*");
	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			continue;
		}
		else
		{
			vctPDFName.push_back(finder.GetFileName());
		}
	}


	int nTime = 0;
	std::map<int, CString> mapConfig;
	m_putpdf->GetXMLConfig(mapConfig);
	for (std::map<int, CString>::iterator it = mapConfig.begin(); it != mapConfig.end(); ++it)
	{
		CString sheetName = it->second;
		std::vector<CString>::iterator itFind = std::find(vctPDFName.begin(), vctPDFName.end(), sheetName);
		if (itFind != vctPDFName.end())
		{

		}
		else
		{
			nTime++;
		}
	}

	if (nTime > 0)
	{
		m_putpdf->ClearXMLConfig();
		return;
	}
	mapConfig.clear();
	*/
	

	HMODULE hLib = LoadLibrary(strDllPath);
	if (hLib)
	{
		typedef BOOL(WINAPI *InitA3DPDF)();

		InitA3DPDF pInitA3DPDF = (InitA3DPDF)GetProcAddress(hLib, _T("InitA3DPDF"));
		if (pInitA3DPDF)
		{
			pInitA3DPDF();
		}

		CApsWaitCur waiter1;
		typedef BOOL(WINAPI *GetPutPdfInfo)(CAPSModel*, LPCTSTR, LPCTSTR, LPCTSTR, bool, int, int, int&,COutPut3DPdf *&);

		GetPutPdfInfo pOutPutPdf = (GetPutPdfInfo)GetProcAddress(hLib, _T("GetPutPdfInfo"));

		if (pOutPutPdf == NULL)
		{
			FreeLibrary(hLib);
			hLib = NULL;
			MessageBox("请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
		}

		int nCodeError = 0;

		IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
		if (!doc)
		{
			ASSERT(FALSE);

		}
		CAPSModel *pModel1 = (CAPSModel*)doc->GetAPSModel();
		auto bOutput = pOutPutPdf(pModel1, StrXmlPath, "", "", true, 0, 0 ,nCodeError,m_putpdf);
		
		FreeLibrary(hLib);
		hLib = NULL;
	}
	
	if (m_putpdf)
	{
		auto config = m_putpdf->GetPdfConfig();
		auto data = m_putpdf->GetPdfData();
		CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();

		if (pSetting)
		{
			for (int i = 0; i < pSetting->m_strCollectionMatch.GetCount(); i++)
			{
				CString strName = pSetting->m_strCollectionMatch.GetAt(i).m_strName;
				CString strValue = pSetting->m_strCollectionMatch.GetAt(i).m_strValue;

				for (auto j = 0; j < config->m_arConfigGroup.GetSize(); ++j)
				{
					ConfigGroup* GroupInfo = config->m_arConfigGroup.GetAt(j);
					auto PageInfo = GroupInfo->m_pFirstPage;
					if (strValue.CompareNoCase(PageInfo->m_strName) == 0)
					{
						m_PageConfig[strName] = GroupInfo;
						break;
					}
				}
				
				std::vector<PageData *> vPagedata;
				for (auto j = 0; j < data->GetPage()->GetSize(); ++j)
				{
					auto pageData= data->GetPage()->GetAt(j);

					auto PageName =pageData->m_strDefName;
					if (strValue.CompareNoCase(PageName) == 0)
					{
						
						vPagedata.push_back(pageData);
					}
				}

				m_PageData[strName] = vPagedata;
			}
		}
	}
	
	return;
}

void CDlgCollect::GetPdfInfoFromXml()
{
	XmlToPdfdata pdfdata;
	pdfdata.GetData(path);
	m_PageData = pdfdata.GetPageData();
	
	/*
	auto pPdf = new COutPut3DPdf;

	//先清空配置
	pPdf->ClearXMLConfig();

	IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
	if (!doc)
	{
		ASSERT(FALSE);
	}

	CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();
	CProcess* pProcess = pModel->GetProcess();
	CString sCfgPath = GetModuleDir() + _T("\\Resources\\Chs\\PDFTemplate\\工艺规程模板.xml");
	pPdf->SetParam(pModel, pProcess, sCfgPath, "");

	//设置数据源
	pPdf->SetProcess(pProcess);

	int nSign = 0;
	//解析xml格式
	BOOL bReadXml = pPdf->ReadCfg(nSign);

	if (pPdf)
	{
		auto config = pPdf->GetPdfConfig();
		auto data = pPdf->GetPdfData();
		
		CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();

		if (pSetting)
		{
			for (int i = 0; i < pSetting->m_strCollectionMatch.GetCount(); i++)
			{
				CString strName = pSetting->m_strCollectionMatch.GetAt(i).m_strName;
				CString strValue = pSetting->m_strCollectionMatch.GetAt(i).m_strValue;

				for (auto j = 0; j < config->m_arConfigGroup.GetSize(); ++j)
				{
					ConfigGroup* GroupInfo = config->m_arConfigGroup.GetAt(j);
					auto PageInfo = GroupInfo->m_pFirstPage;
					if (strValue.CompareNoCase(PageInfo->m_strName) == 0)
					{
						m_PageConfig[strName] = GroupInfo;
						break;
					}
				}

			}
		}
	}
	*/
}

void CDlgCollect::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_WINDCHILL_COMBOX, m_combox);
	DDX_Control(pDX, IDC_WINDCHILL_TAB, m_TabCtrl);
}



BOOL CDlgCollect::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitCombox();
	GetPdfInfo();
	GetPdfInfoFromXml();
	InitTab();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


BEGIN_MESSAGE_MAP(CDlgCollect, CDialog)
	ON_CBN_SELCHANGE(IDC_WINDCHILL_COMBOX, &CDlgCollect::OnCbnSelchangeWindchillCombox)
	ON_NOTIFY(TCN_SELCHANGE, IDC_WINDCHILL_TAB, &CDlgCollect::OnTcnSelchangeWindchillTab)
	ON_BN_CLICKED(IDC_WINDCHILL_COLLECT_SAVE, &CDlgCollect::OnBnClickedWindchillCollectSave)
	ON_BN_CLICKED(IDC_WINDCHILL_COLLECT_COLLECT, &CDlgCollect::OnBnClickedWindchillCollectCollect)
END_MESSAGE_MAP()


void CDlgCollect::OnCbnSelchangeWindchillCombox()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dlgInfo.DestroyWindow();
	for (auto it =m_dlgGrid.begin();it!=m_dlgGrid.end();++it)
	{
		if (it->second!=NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}	
	m_dlgGrid.clear();

	m_dlgChangeGrid.DestroyWindow();

	InitTab();
}


void CDlgCollect::OnTcnSelchangeWindchillTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	switch (m_TabCtrl.GetCurSel())
	{
		case 0:
		{
				UpdateData(TRUE);

				m_dlgInfo.ShowWindow(SW_HIDE);
				for (auto it = m_dlgGrid.begin(); it != m_dlgGrid.end(); ++it)
				{
					it->second->ShowWindow(SW_HIDE);
				}
				m_dlgChangeGrid.ShowWindow(SW_HIDE);

				CRect rect;
				GetClientRect(&rect);

				rect.top += 21;
				rect.left += 1;
				rect.right -= 1;
				rect.bottom -= 1;

				m_dlgInfo.MoveWindow(&rect);
				m_dlgInfo.ShowWindow(SW_SHOW);

				break;

			}

		default:
		{
			UpdateData(TRUE);
			m_dlgInfo.ShowWindow(SW_HIDE);
			for (auto it = m_dlgGrid.begin(); it != m_dlgGrid.end(); ++it)
			{
				it->second->ShowWindow(SW_HIDE);
			}
			m_dlgChangeGrid.ShowWindow(SW_HIDE);
			auto i = m_TabCtrl.GetCurSel();

			CRect rect;
			GetClientRect(&rect);

			rect.top += 21;
			rect.left += 1;
			rect.right -= 1;
			rect.bottom -= 1;

			auto result =m_dlgGrid.find(i);
			if (result != m_dlgGrid.end())
			{
				m_dlgGrid[i]->MoveWindow(&rect);
				m_dlgGrid[i]->ShowWindow(SW_SHOW);
			}
			else
			{
				m_dlgChangeGrid.MoveWindow(&rect);
				m_dlgChangeGrid.ShowWindow(SW_SHOW);
			}
			break;
		}
	}
	*pResult = 0;
}



void CDlgCollect::OnBnClickedWindchillCollectSave()
{
	/*
	CString strDllPath = GetModuleDir();
	strDllPath += _T("\\Output3DPdf.dll");
	CString StrXmlPath = GetModuleDir() + _T("\\Resources\\Chs\\PDFTemplate\\工艺规程模板.xml");
	HMODULE hLib = LoadLibrary(strDllPath);
	if (hLib)
	{
		typedef BOOL(WINAPI *InitA3DPDF)();

		InitA3DPDF pInitA3DPDF = (InitA3DPDF)GetProcAddress(hLib, _T("InitA3DPDF"));
		if (pInitA3DPDF)
		{
			pInitA3DPDF();
		}

		CApsWaitCur waiter1;
		typedef BOOL(WINAPI *GetPutPdfInfo)(CAPSModel*, LPCTSTR, LPCTSTR, LPCTSTR, bool, int, int, int&, COutPut3DPdf *&);

		GetPutPdfInfo pOutPutPdf = (GetPutPdfInfo)GetProcAddress(hLib, _T("GetPutPdfInfo"));

		if (pOutPutPdf == NULL)
		{
			FreeLibrary(hLib);
			hLib = NULL;
			MessageBox("请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
		}

		int nCodeError = 0;

		IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
		if (!doc)
		{
			ASSERT(FALSE);

		}
		CAPSModel *pModel1 = (CAPSModel*)doc->GetAPSModel();
		auto bOutput = pOutPutPdf(pModel1, StrXmlPath, "", "", true, 0, 1, nCodeError, m_putpdf);
		FreeLibrary(hLib);
		hLib = NULL;
	}

	if (m_putpdf)
	{
		auto config = m_putpdf->GetPdfConfig();
		auto data = m_putpdf->GetPdfData();
		
		if (data)
		{
			for (auto it =m_PageData.begin();it!=m_PageData.end();++it)
			{
				auto vpages =it->second;

				for (auto pageItem = vpages.begin();pageItem!= vpages.end();++pageItem)
				{
					auto name = (*pageItem)->m_strDefName;
					for (auto i = 0; i < data->GetPage()->GetSize(); ++i)
					{
						auto &page =data->GetPage()->GetAt(i);
						if (name.CompareNoCase(page->m_strDefName)==0 && (*pageItem)->m_No == page->m_No)
						{
							page = *pageItem;
						}
					}
				}
			}
		}
	}
	*/
	PdfDataToXMl pdf(m_PageData);

	// 临时目录
	TCHAR TempDir[1024] = {};
	GetTempPath(1024, TempDir);
	CString sTempDir(TempDir);
	sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");

	CString xmlPath = sTempDir+"save.xml";
	pdf.CreateXml(xmlPath);

	EndDialog(true);
	
}


void CDlgCollect::OnBnClickedWindchillCollectCollect()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dlgInfo.DestroyWindow();

	for (auto it = m_dlgGrid.begin(); it != m_dlgGrid.end(); ++it)
	{
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	m_dlgGrid.clear();
	m_dlgChangeGrid.DestroyWindow();

	GetPdfInfo();
	InitTab();
}
