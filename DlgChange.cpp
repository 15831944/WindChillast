// DlgChange.cpp : 实现文件
//

#include "stdafx.h"
#include "WindChill.h"
#include "DlgChange.h"
#include "afxdialogex.h"
#include "tinyxml.h"

#include "APSProp.h"
#include "Solid.h"
#include "Actionbase.h"
#include "AsmStep.h"
#include "CProduct.h"
#include "apsprocess.h"
#include "Equips.h"
#include "APSModel.h"
#include "AsmInst.h"
#include "PDFData.h"
#include "PDFConfig.h"
#include "WindChill_i.h"
#include "WindChillObject.h"
#include "WindChillSetting.h"

#include "UtilityFunction.h"
#include "APSWaitCursor.h"
#include "PdfDataToXMl.h"


#define IDC_PROPINFO 3000001
#define IDC_TREE     3000002
// CDlgChange 对话框


IMPLEMENT_DYNAMIC(CDlgChange, CDialog)

CDlgChange::CDlgChange(IAPSApplication* pApplication, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChange::IDD, pParent)
{
	m_pApplication = pApplication;
	m_putpdf = NULL;
	m_pChangeConfig = NULL;
}

CDlgChange::~CDlgChange()
{
	if (m_putpdf)
	{
		delete m_putpdf;
		m_putpdf = NULL;
	}

	for (auto it =m_XmlData.begin();it!=m_XmlData.end();++it)
	{
		if ( it->second)
		{
			delete it->second;
			it->second = NULL;
		}
		
	}
	m_BCGPropList.DestroyWindow();
	m_wndTree.DestroyWindow();
}

BOOL CDlgChange::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitInfo();
	GetPdfConfig();
	InitTreeCtrl();
	InitGridCtrl();
	
	return true;
}

void CDlgChange::InitGridCtrl()
{
	HTREEITEM hSelected = m_wndTree.GetSelectedItem();

	if (hSelected ==m_wndTree.GetRootItem() ||hSelected ==NULL)
	{
		return ;
	}
	auto data =(ChangeInfo *)m_wndTree.GetItemData(hSelected);

	CRect rc;
	this->GetClientRect(&rc);

	CRect InfoRc;

	InfoRc.left = 120;
	InfoRc.right = rc.right-60;
	InfoRc.bottom = rc.bottom;
	InfoRc.top = rc.top;

	
	m_BCGPropList.Create(WS_CHILD | WS_BORDER | WS_VISIBLE, InfoRc, this, IDC_PROPINFO);

	m_BCGPropList.SetParent(this);
	m_BCGPropList.EnableHeaderCtrl(TRUE, _T("属性"), _T("值"));
	HDITEM item;
	item.cxy = 120;
	item.mask = HDI_WIDTH;

	m_BCGPropList.GetHeaderCtrl().SetItem(0, new HDITEM(item));

	auto Group = new CBCGPProp(_T("基本属性"));

	if (m_pChangeConfig)
	{
	
	for (auto i = 0; i < m_pChangeConfig->m_pFirstPage->m_InfoHeard.GetSize(); ++i)
	{
		auto info = m_pChangeConfig->m_pFirstPage->m_InfoHeard.GetAt(i);
		auto Name = info.m_strName;

		auto temp = info.m_strRange;
		CString value;
		CString * ptr = NULL;
		if (data)
		{

			if (temp.CompareNoCase("plant") == 0)
			{
				value = data->plant;
				ptr = &data->plant;
			}
			
			if (temp.CompareNoCase("genggaidan") == 0)
			{
				value = data->genggaidan;
				ptr = &data->genggaidan;
			}

			if (temp.CompareNoCase("pronum") == 0)
			{
				value = data->pronum;
				ptr = &data->pronum;
			}

			if (temp.CompareNoCase("lzjname") == 0)
			{
				value = data->lzjname;
				ptr = &data->lzjname;
			}

			if (temp.CompareNoCase("lzjnum") == 0)
			{
				value = data->lzjnum;
				ptr = &data->lzjnum;
			}

			if (temp.CompareNoCase("ffdw") == 0)
			{
				value = data->ffdw;
				ptr = &data->ffdw;
			}

			if (temp.CompareNoCase("grouptotal") == 0)
			{
				value = data->grouptotal;
				ptr = &data->grouptotal;
			}
			if (temp.CompareNoCase("groupcurrent") == 0)
			{
				value = data->groupcurrent;
				ptr = &data->groupcurrent;
			}

			if (temp.Find(',')!=-1)
			{
				value = data->dwg;
				ptr = &data->dwg;
				static TCHAR BASED_CODE szFilter[] = _T("更改单附图 (*.dwg)|*.dwg|All Files (*.*)|*.*||");
				auto SubInfo = new CBCGPFileProp(_T("附图"), TRUE, value.GetBuffer(), _T("dwg"), 0, szFilter, _T("File property"));
				SubInfo->SetData((DWORD_PTR)ptr);
				Group->AddSubItem(SubInfo);
			}
		}
		
	
		if (Name.CompareNoCase("dwg") != 0)
		{
			auto SubInfo = new CBCGPProp(Name.GetBuffer(), 1, value.GetBuffer(), _T(""));
			SubInfo->SetData((DWORD_PTR)ptr);

			Group->AddSubItem(SubInfo);
		}
		}
	}

	m_BCGPropList.AddProperty(Group);
}

void CDlgChange::InitTreeCtrl()
{
	CRect rc;
	this->GetClientRect(&rc);

	CRect treeRc;

	treeRc.left = 0;
	treeRc.right = 100;
	treeRc.bottom = rc.bottom;
	treeRc.top = rc.top;
	UINT nTreeStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP |TVS_EDITLABELS | TVS_HASLINES;
	m_wndTree.Create(nTreeStyles, treeRc, this, IDC_TREE);
	m_wndTree.DeleteAllColumns();
	m_wndTree.InsertColumn(0, _T(""), 120);
	m_wndTree.EnableHeader( BCGP_GRID_HEADER_SORT);
	m_wndTree.EnableColumnAutoSize(true);
	m_wndTree.EnableGridLines(true);
	m_wndTree.EnableAlternateRows(true);
	HTREEITEM hRootA = m_wndTree.InsertItem(_T("更改单"), 0 );


	for (auto it =m_XmlData.begin();it != m_XmlData.end();++it)
	{
		auto hitem =m_wndTree.InsertItem(it->first,0 ,0 , hRootA);
		m_wndTree.SetItemData(hitem,(DWORD_PTR)it->second);
	}

	m_wndTree.AdjustLayout();
}

void CDlgChange::InsertChange()
{
	HTREEITEM hSelected = m_wndTree.GetSelectedItem();

	if (hSelected == NULL)
	{
		return;
	}
	HTREEITEM hNew = NULL;

	LPCTSTR lpszItem = _T("新建更改单");

	// Insert after the currently selected item
	if (hSelected != NULL)
	{
		HTREEITEM hParent = m_wndTree.GetParentItem(hSelected);
		if (hParent)
		{
			hNew = m_wndTree.InsertItem(lpszItem, hParent, hSelected);
		}
		else
		{
			auto hChild =m_wndTree.GetChildItem(hSelected);
			if (!hChild)
				hNew = m_wndTree.InsertItem(lpszItem, hSelected, hSelected);
		
		}
	}

	m_wndTree.AdjustLayout();
	m_wndTree.RedrawWindow();
	m_wndTree.SelectItem(hNew);

	// update the checkbox of the header control
	m_wndTree.UpdateHeaderCheckbox();

	CEdit* pEdit = m_wndTree.EditLabel(hNew);

	if (pEdit != NULL)
	{
		ASSERT_VALID(pEdit);
		pEdit->SetSel(0, -1); // Select All
	}

	if (hNew)
	{
		auto infodata = new ChangeInfo;
		m_wndTree.SetItemData(hNew,(DWORD_PTR)infodata);
	
		m_XmlData[lpszItem]=infodata;
	}

}

void CDlgChange::DeleteChange()
{
	HTREEITEM hSelected = m_wndTree.GetSelectedItem();
	if (hSelected == NULL)
	{
		return;
	}

	if (hSelected != NULL)
	{
		HTREEITEM hParent = m_wndTree.GetParentItem(hSelected);
		if (hParent)
		{
			m_wndTree.SelectItem(hSelected); // select only one item if multiple selection
			m_wndTree.SelectSetFirstVisible(hSelected);
			m_wndTree.RedrawWindow();

			HTREEITEM hNext = m_wndTree.GetNextItem(hSelected, TVGN_PREVIOUSVISIBLE);

			m_wndTree.DeleteItem(hSelected);

			m_wndTree.AdjustLayout();
			m_wndTree.SelectItem(hNext);
		}
	}

}

void CDlgChange::InitInfo()
{
	TiXmlDocument doc;

	// 临时目录
	TCHAR TempDir[1024] = {};
	GetTempPath(1024, TempDir);
	CString sTempDir(TempDir);
	sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");

	CString xmlpath = sTempDir + "changeinfo.xml";
	if (!doc.LoadFile(xmlpath))
	{
		return;
	}

	auto rootEle = doc.FirstChildElement();

	for (auto InfoEle = rootEle; InfoEle; InfoEle = InfoEle->NextSiblingElement())
	{
		CString changeName = InfoEle->Attribute("PageFirst");
		auto chageinfo = new ChangeInfo;
		auto Info = InfoEle->FirstChildElement();
		for (auto Item = Info->FirstChildElement();Item;Item =Item->NextSiblingElement())
		{
			CString temp = Item->Attribute("Range");
			if (temp.CompareNoCase("plant") == 0)
			{
				chageinfo->plant = Item->GetText();
			}
			if (temp.CompareNoCase("genggaidan") == 0)
			{
				chageinfo->genggaidan = Item->GetText();
			}
			if (temp.CompareNoCase("pronum") == 0)
			{
				chageinfo->pronum = Item->GetText();
			}
			if (temp.CompareNoCase("lzjname") == 0)
			{
				chageinfo->lzjname = Item->GetText();
			}

			if (temp.CompareNoCase("lzjnum") == 0)
			{
				chageinfo->lzjnum = Item->GetText();
			}

			if (temp.CompareNoCase("ffdw") == 0)
			{
				chageinfo->ffdw = Item->GetText();
			}

			if (temp.CompareNoCase("grouptotal") == 0)
			{
				chageinfo->grouptotal = Item->GetText();
			}

			if (temp.CompareNoCase("groupcurrent") == 0)
			{
				chageinfo->groupcurrent = Item->GetText();
			}

			if (temp.Find(',') == 0)
			{
				chageinfo->dwg = Item->GetText();
			}
		}
	
		m_XmlData[changeName] = chageinfo;
	}

}

void CDlgChange::SaveData()
{
	TiXmlDocument doc;

	auto rootItem =m_wndTree.GetRootItem();
	rootItem=m_wndTree.GetChildItem(rootItem);
	for (auto item= rootItem; item; item = m_wndTree.GetNextSiblingItem(item))
	{
		auto itemData =(ChangeInfo *)m_wndTree.GetItemData(item);
		
		auto itemvalue = m_wndTree.GetItemText(item);
		auto PageGroup = new TiXmlElement("PageGroup");
		PageGroup->SetAttribute("PageFirst", itemvalue);

		auto info= new TiXmlElement("Info");

		auto Item1 = new TiXmlElement("Item");

		Item1->SetAttribute("Range", "plant");
		auto text1 =new TiXmlText(itemData->plant);
		Item1->LinkEndChild(text1);
		info->LinkEndChild(Item1);

		auto Item2 = new TiXmlElement("Item");
		Item2->SetAttribute("Range", "genggaidan");
		auto text2 = new TiXmlText(itemData->genggaidan);
		Item2->LinkEndChild(text2);
		info->LinkEndChild(Item2);

		auto Item3 = new TiXmlElement("Item");
		Item3->SetAttribute("Range", "pronum");
		auto text3 = new TiXmlText(itemData->pronum);
		Item3->LinkEndChild(text3);
		info->LinkEndChild(Item3);

		auto Item4 = new TiXmlElement("Item");
		Item4->SetAttribute("Range", "lzjname");
		auto text4 = new TiXmlText(itemData->lzjname);
		Item4->LinkEndChild(text4);
		info->LinkEndChild(Item4);

		auto Item5 = new TiXmlElement("Item");
		Item5->SetAttribute("Range", "lzjnum");
		auto text5 = new TiXmlText(itemData->lzjnum);
		Item5->LinkEndChild(text5);
		info->LinkEndChild(Item5);

		auto Item6 = new TiXmlElement("Item");
		Item6->SetAttribute("Range", "ffdw");
		auto text6 = new TiXmlText(itemData->ffdw);
		Item6->LinkEndChild(text6);
		info->LinkEndChild(Item6);

		auto Item7 = new TiXmlElement("Item");
		Item7->SetAttribute("Range", "grouptotal");
		auto text7 = new TiXmlText(itemData->grouptotal);
		Item7->LinkEndChild(text7);
		info->LinkEndChild(Item7);

		auto Item8 = new TiXmlElement("Item");
		Item8->SetAttribute("Range", "groupcurrent");
		auto text8 = new TiXmlText(itemData->groupcurrent);
		Item8->LinkEndChild(text8);
		info->LinkEndChild(Item8);

		auto Item9 = new TiXmlElement("Item");
		Item9->SetAttribute("Range", ",,,");
		auto text9 = new TiXmlText(itemData->dwg);
		Item9->LinkEndChild(text9);
		info->LinkEndChild(Item9);

		PageGroup->LinkEndChild(info);

		doc.LinkEndChild(PageGroup);

	}

	// 临时目录
	TCHAR TempDir[1024] = {};
	GetTempPath(1024, TempDir);
	CString sTempDir(TempDir);
	sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");

	CString xmlpath = sTempDir + "changeinfo.xml";
	doc.SaveFile(xmlpath);
}

void CDlgChange::GetPdfConfig()
{
	/*
	m_putpdf = new COutPut3DPdf;

	//先清空配置
	m_putpdf->ClearXMLConfig();

	IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
	if (!doc)
	{
		ASSERT(FALSE);
	}

	CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();
	CProcess* pProcess = pModel->GetProcess();
	CString sCfgPath = GetModuleDir() + _T("\\Resources\\Chs\\PDFTemplate\\工艺规程模板.xml");
	m_putpdf->SetParam(pModel, pProcess, sCfgPath, "");

	//设置数据源
	m_putpdf->SetProcess(pProcess);

	int nSign = 0;
	//解析xml格式
	BOOL bReadXml = m_putpdf->ReadCfg(nSign);

	*/

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
	
	m_pChangeConfig = NULL;
	auto pConfig = m_putpdf->GetPdfConfig();
	for (auto i=0;i< pConfig->m_arConfigGroup.GetSize();++i)
	{
		auto pTemp =pConfig->m_arConfigGroup.GetAt(i);
		if (pTemp && pTemp->m_pFirstPage->m_strName.CompareNoCase("厂艺表065工艺规程更改单.pdf")==0)
		{
			m_pChangeConfig = pTemp;
			break;
		}
	}
	}

}

void CDlgChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


void CDlgChange::OnContextMenu(CWnd*, CPoint)
{
	auto  SelectedItem=m_wndTree.GetSelectedItem();
	if (SelectedItem)
	{
		CMenu Menu;

		Menu.LoadMenu(IDR_CAHNGEGRID);
		auto pm = Menu.GetSubMenu(0);
		CPoint pot;
		GetCursorPos(&pot);
		pm->TrackPopupMenu(TPM_LEFTALIGN, pot.x, pot.y, this);
	}
}

LRESULT CDlgChange::OnItemChanged(WPARAM uProListId, LPARAM pProp)
{
	HTREEITEM hSelected = m_wndTree.GetSelectedItem();
	if (hSelected == NULL ||hSelected == m_wndTree.GetRootItem())
	{
			m_BCGPropList.DestroyWindow();
		return 0;
	}

	m_BCGPropList.DestroyWindow();

	InitGridCtrl();
	
	return 0;
}

LRESULT CDlgChange::OnGridItemChanged(WPARAM uProListId, LPARAM pProp)
{
	auto selItem = m_wndTree.GetSelectedItem();

	auto curItem =m_wndTree.GetCurSelItem();
	auto value =curItem->GetValue();

	auto ptr = (ChangeInfo *)m_wndTree.GetItemData(selItem);
 
	for (auto it =m_XmlData.begin();it!=m_XmlData.end();)
	{
		if (it->second == ptr)
		{
			it = m_XmlData.erase(it);
			m_XmlData[value] = ptr;
		}
		else
			++it;
	}
	
	return 0;
}

LRESULT CDlgChange::OnProChanged(WPARAM uProListId, LPARAM pProp)
{
	auto prop1 = (CBCGPProp *)pProp;
	auto value = prop1->GetValue();
	auto data = (CString*)prop1->GetData();
	if (!data)
	{
		return 0;
	}
	*data = value;
	return 0;
}


BEGIN_MESSAGE_MAP(CDlgChange, CDialog)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ADDCHANGE, &CDlgChange::InsertChange)
	ON_COMMAND(ID_DELETECHAGNE, &CDlgChange::DeleteChange)
	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, OnItemChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, OnGridItemChanged)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnProChanged)
	ON_BN_CLICKED(IDC_SAVECHange, &CDlgChange::OnBnClickedSavechange)
END_MESSAGE_MAP()


// CDlgChange 消息处理程序


void CDlgChange::OnBnClickedSavechange()
{
	// TODO: 在此添加控件通知处理程序代码
	SaveData();

}
