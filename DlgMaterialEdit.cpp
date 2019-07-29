
// windchillDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "afxwin.h"


#include "AtlConv.h"

#include "EasySize.h"
//excle
#include "CApplication.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"

#include "common.h"
#include "DlgMaterialEdit.h"
#include "DlgMaterialQuery.h"
#include "DlgAddMaterial.h"
#include "DlgPBOMEdit.h"
#include "WindChillsetting.h"
#include "DlgSave.h"
#include "KmAppSettings.h"
#include "UtilityFunction.h"
#include "KmZipUtil.h"
#include "WindChillXml.h"
#include "tlhelp32.h"

extern CString GetModuleDir();
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

extern vector<string> split(string str, string pattern);
// CDlgMaterialEdit 对话框

CString GetUid()
{
	CString strTmp = "";
	GUID guid;
	CoInitialize(NULL);
	if (::CoCreateGuid(&guid) == S_OK)
	{
		strTmp.Format("%.8x", guid.Data4);
	}
	CoUninitialize();
	return strTmp;
}

CString GetRelativeDir()
{
	SYSTEMTIME tmptime;
	GetLocalTime(&tmptime);

	CString sUid = GetUid();
	char sTime[32] = { 0 };
	//sprintf_s(sTime, "%.4d%.2d%.2d%.2d%.2d%.2d", tmptime.wYear, tmptime.wMonth, tmptime.wDay,tmptime.wHour,tmptime.wMinute,tmptime.wSecond,tmptime.wMilliseconds, sUid
	sprintf_s(sTime, "%.4d%.2d%.2d%.2d%.2d%.2d", tmptime.wYear, tmptime.wMonth, tmptime.wDay, tmptime.wHour, tmptime.wMinute, tmptime.wSecond);
	return sTime;
}


CDlgMaterialEdit::CDlgMaterialEdit(CAPSModel * pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMaterialEdit::IDD, pParent)
{

	m_pModel = pModel;

}

void CDlgMaterialEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WINDCHILL_PARTLIST, m_Partlist);
	DDX_Control(pDX, IDC_WINDCHILL_MATERUALTYPE, m_materialtypeTree);
	DDX_Control(pDX, IDC_WINDCHILL_EDITLIST, m_editlist);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
}

BOOL CDlgMaterialEdit::PreTranslateMessage(MSG* pMsg)
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

BEGIN_MESSAGE_MAP(CDlgMaterialEdit, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_WINDCHILL_PARTLIST, &CDlgMaterialEdit::OnNMDblclkWindchillPartlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_WINDCHILL_PARTLIST, &CDlgMaterialEdit::OnLvnItemchangedWindchillPartlist)
	ON_EN_KILLFOCUS(IDC_WINDCHILL_EDITLIST, &CDlgMaterialEdit::OnEnKillfocusWindchillEditlist)
	ON_NOTIFY(NM_CLICK, IDC_WINDCHILL_MATERUALTYPE, &CDlgMaterialEdit::OnNMClickWindchillMaterualtype)
	ON_NOTIFY(TVN_SELCHANGED, IDC_WINDCHILL_MATERUALTYPE, &CDlgMaterialEdit::OnTvnSelchangedWindchillMaterualtype)
	ON_NOTIFY(LVN_ENDSCROLL, IDC_WINDCHILL_PARTLIST, &CDlgMaterialEdit::OnLvnEndScrollWindchillPartlist)
	ON_BN_CLICKED(IDC_WINDCHILL_SELECTPBOM, PBOMQureyDlg)
	ON_BN_CLICKED(IDC_WINDCHILL_SELECTMATERIAL, MaterialQueryDlg)
	ON_BN_CLICKED(IDC_WINDCHILL_ADDMATERIAL, AddMaterialDlg)
	ON_BN_CLICKED(IDC_WINDCHILL_CALCULATE, Calculate)
	ON_BN_CLICKED(IDC_WINDCHILL_SAVE, SaveDlg)
	ON_BN_CLICKED(IDC_WINDCHILL_IMPORT, Import)
	ON_BN_CLICKED(IDC_WINDCHILL_UPLOAD, Upload)
	
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_CBN_KILLFOCUS(IDC_COMBO1, &CDlgMaterialEdit::OnCbnKillfocusCombo1)
	ON_NOTIFY(NM_RCLICK, IDC_WINDCHILL_PARTLIST, &CDlgMaterialEdit::OnNMRClickWindchillPartlist)
	ON_COMMAND(ID_DELETE, &CDlgMaterialEdit::DeleteMatertion)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CDlgMaterialEdit)
	EASYSIZE(IDC_WINDCHILL_MATERUALTYPE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_BORDER, 0)
	EASYSIZE(IDC_WINDCHILL_STATICTYPE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_BORDER, 0)
	EASYSIZE(IDC_WINDCHILL_PARTLIST, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_WINDCHILL_STATICPART, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_WINDCHILL_STATIC, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_WINDCHILL_SELECTPBOM, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_SELECTMATERIAL, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_ADDMATERIAL, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_CALCULATE, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_IMPORT, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_SAVE, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_WINDCHILL_UPLOAD, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
END_EASYSIZE_MAP
// CDlgMaterialEdit 消息处理程序

BOOL CDlgMaterialEdit::DestroyWindow()
{
	// TODO:  在此添加专用代码和/或调用基类
	CRect rt;
	GetWindowRect(rt);

	CString strKey = KmAppSettings::GetAppStrReg();
	if (strKey.GetLength() > 0)
	{
		strKey += _T("\\kmapstool\\RecentSettings\\");

		CString strVal;
		strVal.Format(_T("%d,%d,%d,%d"), rt.left, rt.top, rt.right, rt.bottom);
		if ( rt.bottom -rt.top <30)
		{
			return CDialog::DestroyWindow();
		}
		KmAppSettings::WriteStrKey(strKey, _T("DlgMaterialEdit"), strVal);
	}
	return CDialog::DestroyWindow();
}



BOOL CDlgMaterialEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	InitPartList();
	InitMaterialtypeTree();

	m_row = -1;
	m_col = -1;
	INIT_EASYSIZE;
	pBomNumber ="";

	CRect rt;
	GetWindowRect(rt);
	CString strKey = KmAppSettings::GetAppStrReg();
	if (strKey.GetLength() > 0)
	{
		strKey += _T("\\kmapstool\\RecentSettings\\");
		CString strVal = KmAppSettings::ReadStrKey(strKey, _T("DlgMaterialEdit"), _T(""));
		if (!strVal.IsEmpty())
		{
			int l, t, r, b;
			sscanf_s(strVal, _T("%d,%d,%d,%d"), &l, &t, &r, &b);
			rt.SetRect(l, t, r, b);
			MoveWindow(rt);
		}
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

CDlgMaterialEdit:: ~CDlgMaterialEdit()
{
	//MessageBox("账号密码错误,请重新输入", APS_MSGBOX_TITIE, MB_OK);
}

void CDlgMaterialEdit::OnSysCommand(UINT nID, LPARAM lParam)
{
		CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDlgMaterialEdit::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDlgMaterialEdit::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//从xml配置读取有多少行，显示多少属性
void  CDlgMaterialEdit::InitPartList()
{
	DWORD_PTR dwStyle = m_Partlist.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT;

	m_Partlist.SetExtendedStyle(dwStyle);

	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting)
	{
		for (int i = 0;i < pSetting->m_strMaterialMatch.GetCount();i++)
		{
			CString strTmp = pSetting->m_strMaterialMatch.GetAt(i).m_strName;
			if (!strTmp.IsEmpty())
			{
				m_Partlist.InsertColumn(i, strTmp, LVCFMT_LEFT, 0);
				m_Partlist.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
			}
		}
	}

 	m_Partlist.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
 	//m_Partlist.DeleteAllItems();
}

void  CDlgMaterialEdit::InitMaterialtypeTree()
{
	DWORD dwStyle = GetWindowLong(m_materialtypeTree.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASLINES;
	::SetWindowLong(m_materialtypeTree.m_hWnd, GWL_STYLE, dwStyle);

	HTREEITEM hRoot = m_materialtypeTree.InsertItem(_T("分类"));
	if (hRoot)
	{
		CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
		if (pSetting)
		{
			for (int i = 0;i < pSetting->m_strMaterialType.GetCount();i++)
			{
				CString strTmp = pSetting->m_strMaterialType.GetAt(i).m_strName;
				if (!strTmp.IsEmpty())
				{
					m_materialtypeTree.InsertItem(strTmp, hRoot);
					MaterialType.push_back(strTmp);
				}
			}
		}
		m_materialtypeTree.Expand(hRoot, TVE_EXPAND);
	}
}

void CDlgMaterialEdit::SelectItem(CString item)
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting == NULL)
		return;

	m_Partlist.DeleteAllItems();
	Relation.clear();

	int row = -1, col;
	CWaitCursor wait;
	for (auto it = partInfo.begin(); it != partInfo.end(); ++it)
	{
		if (CString(it->value["Type"].c_str()).CompareNoCase(item)==0)
		{
			++row;
			m_Partlist.InsertItem(row, _T(""));
			Relation.push_back(it - partInfo.begin());
			col = -1;
		
			for (int i =0;i<pSetting->m_strMaterialMatch.GetCount();i++)
			{
				CString strName = pSetting->m_strMaterialMatch.GetAt(i).m_strName;
				CString strValue = pSetting->m_strMaterialMatch.GetAt(i).m_strValue;
				for (auto itemValue = it->value.begin(); itemValue != it->value.end(); ++itemValue)
				{
					auto key = itemValue->first;
					auto word = itemValue->second;
					if (strValue.CompareNoCase(key.c_str())==0)
					{
						m_Partlist.SetItemText(row,i,word.c_str());
						break;
					}
				}
			}
		}
	}
	m_Partlist.SetRedraw(FALSE);
	CHeaderCtrl *pheader =m_Partlist.GetHeaderCtrl();
	auto colcount =pheader->GetItemCount();
	for (int i=0;i<colcount;++i)
	{
		m_Partlist.SetColumnWidth(i,LVSCW_AUTOSIZE);
		auto colwidth =m_Partlist.GetColumnWidth(i);
		m_Partlist.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
		auto headwidth =m_Partlist.GetColumnWidth(i);
		m_Partlist.SetColumnWidth(i,max(colwidth,headwidth));
	}
	m_Partlist.SetRedraw(TRUE);

	wait.Restore();
	//auto t_2 = GetTickCount();
	//AfxMessageBox(to_string(t_2 - t_1).c_str());

	
}

void  CDlgMaterialEdit::SelectPartition()
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting == NULL)
		return;
	m_Partlist.DeleteAllItems();
	Relation.clear();
	/*
	int row = -1, col;
	for (auto it = partInfo.begin(); it != partInfo.end(); ++it)
	{
	auto elem = it->Part;
	auto PartContent = elem->FirstChildElement();
	++row;
	m_Partlist.InsertItem(row, _T(""));
	col = -1;

	while (PartContent != NULL)
	{
	auto partchild = PartContent;
	auto key = PartContent->Value();
	auto word = PartContent->GetText();

	if (string(key) == string("Children"))
	{
	auto ChildrenElem = PartContent->FirstChildElement();

	int child_col = -1;
	while (ChildrenElem != NULL)
	{
	++row;
	m_Partlist.InsertItem(row, _T(""));
	auto PartElem = ChildrenElem->FirstChildElement();
	while (PartElem != NULL)
	{
	auto child_key = PartElem->Value();
	auto child_word = PartElem->GetText();
	auto it = KeyWords.find(child_key);
	if (it == KeyWords.end())
	{
	PartElem = PartElem->NextSiblingElement();
	continue;
	}

	if (child_word != NULL)
	{
	auto it = find(Rank.begin(), Rank.end(), child_key);
	child_col = it - Rank.begin();
	CString val(child_word);
	m_Partlist.SetItemText(row, child_col, val);
	}

	auto partchild = elem->FirstChildElement();
	while (partchild != NULL)
	{
	auto key = partchild->Value();
	auto word = partchild->GetText();

	auto it = KeyWords.find(key);
	if (it == KeyWords.end())
	{
	//该行属性不用显示
	partchild = partchild->NextSiblingElement();
	continue;
	}

	if (word != NULL)
	{
	auto it = find(Rank.begin(), Rank.end(), key);
	col = it - Rank.begin();
	auto text = m_Partlist.GetItemText(row, col);
	if (text == "")
	{
	CString val(word);
	m_Partlist.SetItemText(row, col, val);
	}
	}
	partchild = partchild->NextSiblingElement();
	}
	PartElem = PartElem->NextSiblingElement();
	}
	ChildrenElem = ChildrenElem->NextSiblingElement();
	}
	}

	auto it = KeyWords.find(key);
	if (it == KeyWords.end())
	{
	//该行属性不用显示
	PartContent = PartContent->NextSiblingElement();
	continue;
	}
	if (word != NULL)
	{
	auto it = find(Rank.begin(), Rank.end(), key);
	col = it - Rank.begin();
	CString val(word);
	m_Partlist.SetItemText(row, col, val);
	}
	PartContent = PartContent->NextSiblingElement();
	}
	}
	*/
	int row = -1, col=-1;
	CWaitCursor wait;
	for (auto it = partInfo.begin(); it != partInfo.end(); ++it)
	{
//		if (find(MaterialType.begin(),MaterialType.end(),it->value["Type"].c_str())!=MaterialType.end())
		{
			++row;
			m_Partlist.InsertItem(row, _T(""));

			//m_row = row;
			//m_col = 0;
			//ModifyPartInfo(to_string(static_cast<_LONGLONG>(m_row+1)).c_str());

			Relation.push_back(it - partInfo.begin());

			//m_Partlist中间列顺序为 m_strMaterialMatch

			for (int i =0;i<pSetting->m_strMaterialMatch.GetCount();i++)
			{
				CString strName = pSetting->m_strMaterialMatch.GetAt(i).m_strName;
				CString strValue = pSetting->m_strMaterialMatch.GetAt(i).m_strValue;

				for (auto itemValue = it->value.begin(); itemValue != it->value.end(); ++itemValue)
				{
					auto key = itemValue->first;
					auto word = itemValue->second;
					if (strValue.CompareNoCase(key.c_str())==0)
					{
						m_Partlist.SetItemText(row,i,word.c_str());

						break;
					}
				}
			}
		}
	}
	wait.Restore();
	auto t_22 = GetTickCount();


}

//双击list contorl可编辑
void CDlgMaterialEdit::OnNMDblclkWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CRect rc;
	CString strtemp;
	m_row = pNMListView->iItem;
	m_col = pNMListView->iSubItem;
	 //双击空白行，新添加一行
	auto hitem =m_materialtypeTree.GetSelectedItem();
	auto Text =m_materialtypeTree.GetItemText(hitem);
	if (pNMListView->iItem == -1 && Text.CompareNoCase("辅材")==0)
	{
		m_row = m_Partlist.GetItemCount();
		strtemp = "";
	
		m_Partlist.InsertItem(m_row, strtemp);
		m_Partlist.SetItemState(m_row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

		partInfo.push_back(NULL);
		partInfo[partInfo.size()-1].value["Type"]="辅材";
		SelectPartition();
		SelectItem("辅材");
		m_Partlist.EnsureVisible(m_row, FALSE);
	}
	if (m_col != -1)
	{
		CString test;
		test.Format(_T("选中第%d行，第%d列"), m_row, m_col);
		
		auto key =CWindChillSetting::m_strMaterialMatch[m_col];

		if (key.m_strName==CString("材料名称") ||key.m_strName ==CString("零件定额尺寸"))
		{
			if (key.m_strName == CString("材料名称"))
			{
				//钢材系数0.00617、铜材0.00668、铝材0.00219、橡胶0.00109
				m_ComboBox.AddString("钢材");
				m_ComboBox.AddString("铜材");
				m_ComboBox.AddString("铝材");
				m_ComboBox.AddString("橡胶");
			}
			else
			{
				m_ComboBox.AddString("φ ");
				m_ComboBox.AddString("G ");
				m_ComboBox.AddString("δ ");
			}
			m_Partlist.GetSubItemRect(m_row, m_col, LVIR_LABEL, rc);
			m_ComboBox.SetParent(&m_Partlist);
			m_ComboBox.MoveWindow(rc);
			m_ComboBox.SetWindowText(m_Partlist.GetItemText(m_row, m_col));
			m_ComboBox.ShowWindow(SW_SHOW);
			m_ComboBox.SetFocus();
		}
		else
		{
			m_Partlist.GetSubItemRect(m_row, m_col, LVIR_LABEL, rc);
			m_editlist.SetParent(&m_Partlist);
			m_editlist.MoveWindow(rc);
			m_editlist.SetWindowText(m_Partlist.GetItemText(m_row, m_col));
			m_editlist.ShowWindow(SW_SHOW);
			m_editlist.SetFocus();
		}
		m_editlist.ShowCaret();
		m_editlist.SetSel(-1);
	}
	//m_editlist.ShowWindow(SW_SHOW);
	*pResult = 0;
}

void CDlgMaterialEdit::OnLvnItemchangedWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}

bool IsChildPoint(int row, std::vector<int > Relation)
{
	if (row == 0)
	{
		return true;
	}

	if (static_cast<size_t>(row) < Relation.size())
	{
		if (Relation[row] == Relation[row - 1])
			return false;
		else
			return true;
	}
	return false;
}

void CDlgMaterialEdit::ModifyPartInfo(CString str)
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	if (pSetting == NULL)
		return;

	char *STR = str.GetBuffer(str.GetLength()-0);
	
	if (static_cast<size_t>(m_row) < Relation.size())
	{
		if (m_row!=-1)
		{ 
			CString strName = pSetting->m_strMaterialMatch.GetAt(m_col).m_strName;
			CString strValue = pSetting->m_strMaterialMatch.GetAt(m_col).m_strValue;
			if (partInfo.empty())
			{
				return;
			}
			partInfo[Relation[m_row]].value[strValue.GetBuffer()] = STR;
			strValue.ReleaseBuffer();
		}
	}
	
	/*CString strName = pSetting->m_strMaterialMatch.GetAt(m_col).m_strName;
	CString strValue = pSetting->m_strMaterialMatch.GetAt(m_col).m_strValue;

	if (m_row!=-1)
	{ 
		if (partInfo.empty())
		{
			return;
		}
		partInfo[m_row].value[strValue.GetBuffer()] = STR;
		strValue.ReleaseBuffer();
	}*/
	return;
}

void CDlgMaterialEdit::OnEnKillfocusWindchillEditlist()
{
	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_editlist.GetWindowText(str);
	m_Partlist.SetItemText(m_row, m_col, str);
	m_editlist.ShowWindow(SW_HIDE);
	ModifyPartInfo(str);

	/*
	HTREEITEM hItem;
	hItem = m_materialtypeTree.GetSelectedItem();
	auto text = m_materialtypeTree.GetItemText(hItem);

	partInfo[m_row].ModifyNode_Text("Number", "11111111");
	*/

}

void CDlgMaterialEdit::OnCbnKillfocusCombo1()
{
	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_ComboBox.GetWindowText(str);
	m_Partlist.SetItemText(m_row, m_col, str);
	m_ComboBox.ShowWindow(SW_HIDE);
	ModifyPartInfo(str);
	for (int i = m_ComboBox.GetCount() - 1; i >= 0; i--)
	{
		m_ComboBox.DeleteString(i);
	}
}


void CDlgMaterialEdit::OnNMClickWindchillMaterualtype(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	//MessageBox("账号密码错误,请重新输入", APS_MSGBOX_TITIE, MB_OK);
	*pResult = 0;
}

//查询PBOm界面
void CDlgMaterialEdit::PBOMQureyDlg()
{
	// TODO:  在此添加控件通知处理程序代码
	CDlgPBOMEdit dlg(m_pModel,QueryType_Material_PBOM, NULL,this);
	dlg.SetCaption("PBOM查询");

	if (dlg.DoModal() == IDOK)
	{
		m_Partlist.DeleteAllItems();
		pBomNumber =dlg.GetPartNumber();
		m_pPBOMxml = dlg.GetXmlPtr();
		TiXmlElement *root = m_pPBOMxml.FirstChildElement();
		if (root == NULL || root->FirstChildElement() == NULL)
		{
			m_pPBOMxml.Clear();
			return;
		}
		partInfo.clear();
		auto elem = root->FirstChildElement()->FirstChildElement();
		while (elem != NULL)
		{
			PartInfo part(elem);
			if (find(MaterialType.begin(),MaterialType.end(),part.value["Type"].c_str())!=MaterialType.end())
			{
				partInfo.push_back(part);
			}	
			elem = elem->NextSiblingElement();
		}
		SelectPartition();
	}
}

//查询 材料定额编辑界面
void CDlgMaterialEdit::MaterialQueryDlg()
{
//	// TODO:  在此添加控件通知处理程序代码
	CDlgMaterialQuery dlg(this);
#if 0
	if (dlg.DoModal() == IDOK)
	{

		m_pPBOMxml = dlg.GetXmlPtr();
		TiXmlElement *root = m_pPBOMxml.FirstChildElement();
		if (root == NULL)
		{
			m_pPBOMxml.Clear();
			return;
		}
		partInfo.clear();
		auto elem = root->FirstChildElement()->FirstChildElement();
		while (elem != NULL)
		{
			PartInfo part(elem);
			partInfo.push_back(part);
			elem = elem->NextSiblingElement();
		}
		SelectPartition();
	}
#endif
	if(dlg.DoModal()==IDOK)
	{
		pBomNumber =dlg.GetPartNumber();
		MaterialEditXml = dlg.Materialxml;
		PBOMXml = dlg.PBOMxml;

		TiXmlElement *root = MaterialEditXml.FirstChildElement();

		vector<PartInfo>MateriaPartinfo;
		if (root == NULL)
		{
			//MessageBox("出现异常", APS_MSGBOX_TITIE, MB_OK);
			//return;
			MaterialEditXml.Clear();
		}

		partInfo.clear();
		if (root)
		{
			auto elem = root->FirstChildElement()->FirstChildElement();
			while (elem != NULL)
			{
				PartInfo part(elem);
				MateriaPartinfo.push_back(part);
				//partInfo.push_back(part);
				elem = elem->NextSiblingElement();
			}
		}
		
		root = PBOMXml.FirstChildElement();

		vector<PartInfo> PBOMPartinfo;
		if (root == NULL || root->FirstChildElement() == NULL)
		{
			//MessageBox("出现异常", APS_MSGBOX_TITIE, MB_OK);
			PBOMPartinfo.clear();
			//return;
		}

		PBOMPartinfo.clear();
		if (root)
		{
			auto elem = root->FirstChildElement()->FirstChildElement();
			while (elem != NULL)
			{
				PartInfo part(elem);
				PBOMPartinfo.push_back(part);
				elem = elem->NextSiblingElement();
			}
		}

		
		for (auto PBomItem = PBOMPartinfo.begin();PBomItem != PBOMPartinfo.end(); ++PBomItem)
		{
			auto flag =true;

			for (auto Materiaitem =MateriaPartinfo.begin();Materiaitem!=MateriaPartinfo.end();++Materiaitem)
			{
				if (Materiaitem->value["Number"] == PBomItem->value["Number"])
				{
					PartInfo part(*Materiaitem);
					if (find(MaterialType.begin(),MaterialType.end(),part.value["Type"].c_str())!=MaterialType.end())
					{
						partInfo.push_back(part);
					}	
					flag =  false;
					break;
				}
			}

			if (flag)
			{
				PartInfo part(*PBomItem);
				if (find(MaterialType.begin(),MaterialType.end(),part.value["Type"].c_str())!=MaterialType.end())
				{
					partInfo.push_back(part);
				}	
			}
		}

		for (auto Materiaitem =MateriaPartinfo.begin();Materiaitem!=MateriaPartinfo.end();++Materiaitem)
		{
			if (Materiaitem->value["Type"] == "辅材")
			{
				PartInfo part(*Materiaitem);
				partInfo.push_back(part);
				break;
			}
		}

		SelectPartition();
	}
}

//    材辅料查询界面
void CDlgMaterialEdit::AddMaterialDlg()
{
	//TODO:  在此添加控件通知处理程序代码
	CDlgAddMaterial dlg(ResourceType_ADDMaterial);
	if (dlg.DoModal()== IDOK)
	{
		AddMaterialXml = dlg.GetXMl();

		TiXmlElement *root = AddMaterialXml.FirstChildElement();
		if (root == NULL || root->FirstChildElement() == NULL)
		{
			m_pPBOMxml.Clear();
			return;
		}
		auto elem = root->FirstChildElement()->FirstChildElement();
		while (elem != NULL)
		{
			PartInfo part(elem);
			partInfo.push_back(part);
			elem = elem->NextSiblingElement();
		}
		SelectPartition();
	}
}

//定额计算
void CDlgMaterialEdit::Calculate()
{
	// TODO:  在此添加控件通知处理程序代码
	vector<CString> CLMC;
	CLMC.push_back("钢材");
	CLMC.push_back("铝材");
	CLMC.push_back("铜材");
	CLMC.push_back("橡胶");

	double bangliao[] = { 0.00617, 0.00219 , 0.00668 , 0.00109};
	double guanliao[] = { 0.02466, 0.00873, 0.0267, 0.00 };
	double banliao[] = { 7.85, 2.78, 8.5, 1.57 };
	
	auto pos1 = m_Partlist.GetFirstSelectedItemPosition();
	//if (pos ==NULL)
	{
		for (auto i = 0; i < m_Partlist.GetItemCount(); ++i)
		{
			auto index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "材料名称");
			CString text;
			if (index == -1)
			{
				continue;
			}
			text = m_Partlist.GetItemText(i, index);
			if (text.IsEmpty())
			{

				continue;
			}
			auto it = find(CLMC.begin(), CLMC.end(), text);
			auto pos = 0;
			if (it != CLMC.end())
			{
				pos = it - CLMC.begin();
			}

			index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "零件定额尺寸");
			if (index == -1)
			{
				continue;
			}

			text = m_Partlist.GetItemText(i, index);
			if (text.IsEmpty())
			{
				continue;
			}
			auto content = split(text.GetBuffer()," ");
			if (content.size()==0)
			{
				MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
				//return;
				continue;
			}
			text =content[0].c_str();

			if (text == CString("G"))
			{
				//管料G：G = K（D - b）bL / 1000
				text =content[1].c_str();
				auto number = split(text.GetBuffer(), "*");
				if (number.size() == 3)
				{
					auto K = guanliao[pos];
					auto D = strtod(number[0].c_str(), NULL);
					auto d = strtod(number[1].c_str(), NULL);
					auto L = strtod(number[2].c_str(), NULL);
					auto result = K*(D - d)*d*L / 1000;
					result =floor(result*10000.000f+0.5)/10000;
					index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单件重量");
					if (index == -1)
					{
						continue;
					}
					m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
					m_row = i;
					m_col = index;
					ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());

					index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单套数量");
					if (index == -1)
					{
						continue;
					}
					CString DTSL = m_Partlist.GetItemText(i, index);
					if (DTSL.IsEmpty())
					{
						continue;
					}

					index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "全套重量");
					if (index == -1)
					{
						continue;
					}

					result = result * _ttoi(DTSL);
					result =floor(result*10000.000f+0.5)/10000;
					m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
					m_col = index;
					ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());
				}
			}
			else
			{
				if (text == CString("φ"))
				{
					//圆棒料φ：G = Kd²L / 1000
					text =content[1].c_str();
					auto number = split(text.GetBuffer(), "*");
					if (number.size() == 2)
					{
						auto K = bangliao[pos];
						auto d = strtod(number[0].c_str(), NULL);
						auto L = strtod(number[1].c_str(), NULL);
						auto result = K*d*d*L / 1000;
						result =floor(result*10000.000f+0.5)/10000;
						index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单件重量");
						if (index == -1)
						{
							continue;
						}
						m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
						m_row = i;
						m_col = index;
						ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());

						index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单套数量");
						if (index == -1)
						{
							continue;
						}
						CString DTSL = m_Partlist.GetItemText(i, index);
						if (DTSL.IsEmpty())
						{
							continue;
						}

						index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "全套重量");
						if (index == -1)
						{
							continue;
						}

						result = result * _ttoi(DTSL);
						result =floor(result*10000.000f+0.5)/10000;
						m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
						m_col = index;
						ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());
					}
				}
				else
				{
					if (text == CString("δ"))
					{
						//板料δ：G = 厚度×宽度×长度×δ / 1000
						text =content[1].c_str();
						auto number = split(text.GetBuffer(), "*");
						if (number.size() == 3)
						{
							auto δ = banliao[pos];
							auto P = strtod(number[0].c_str(), NULL);
							auto W = strtod(number[1].c_str(), NULL);
							auto L = strtod(number[2].c_str(), NULL);
							auto result = P*W*L* δ / 1000;
							result =floor(result*10000.000f+0.5)/10000;
							index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单件重量");
							if (index == -1)
							{
								continue;
							}
							m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
							m_row = i;
							m_col = index;
							ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());

							index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单套数量");
							if (index == -1)
							{
								continue;
							}
							CString DTSL = m_Partlist.GetItemText(i, index);
							if (DTSL.IsEmpty())
							{
								continue;
							}

							index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "全套重量");
							if (index == -1)
							{
								continue;
							}

							result = result * _ttoi(DTSL);
							result =floor(result*10000.000f+0.5)/10000;
							m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
							m_col = index;
							ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());
						}
					}
				}
			}
		}
	}
	//begin add in 2018.11.26 by xjt for 70569	
//	else
	//{
	//	int i = m_Partlist.GetNextSelectedItem(pos);
	//	auto index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "材料名称");
	//	CString text;
	//	if (index == -1)
	//	{
	//		MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//		return;
	//	}
	//	text = m_Partlist.GetItemText(i, index);
	//	if (text.IsEmpty())
	//	{
	//		MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//		return;
	//	}
	//	auto it = find(CLMC.begin(), CLMC.end(), text);
	//	auto pos = 0;
	//	if (it != CLMC.end())
	//	{
	//		pos = it - CLMC.begin();
	//	}

	//	index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "零件定额尺寸");
	//	if (index == -1)
	//	{
	//		MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//		return;
	//	}

	//	text = m_Partlist.GetItemText(i, index);
	//	if (text.IsEmpty())
	//	{
	//		MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//		return;
	//	}
	//	auto content = split(text.GetBuffer()," ");
	//	if (content.size()==0)
	//	{
	//		MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//		return;
	//	}
	//	text =content[0].c_str();
	//	if (text == CString("G"))
	//	{
	//		//管料G：G = K（D - b）bL / 1000
	//		text =content[1].c_str();

	//		if (text.IsEmpty())
	//		{
	//			MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//			return;
	//		}
	//		auto number = split(text.GetBuffer(), "*");
	//		if (number.size() == 3)
	//		{
	//			auto K = guanliao[pos];
	//			auto D = strtod(number[0].c_str(), NULL);
	//			auto d = strtod(number[1].c_str(), NULL);
	//			auto L = strtod(number[2].c_str(), NULL);
	//			auto result = K*(D - d)*d*L / 1000;
	//			index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单件重量");
	//			if (index == -1)
	//			{
	//				MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//				return;
	//			}
	//			m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
	//			m_row = i;
	//			m_col = index;
	//			ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());

	//			index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单套数量");
	//			if (index == -1)
	//			{
	//				MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//				return;
	//			}
	//			CString DTSL = m_Partlist.GetItemText(i, index);
	//			if (DTSL.IsEmpty()) 
	//			{
	//				MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//				return;
	//			}

	//			index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "全套重量");
	//			if (index == -1)
	//			{
	//				MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//				return;
	//			}

	//			result = result * _ttoi(DTSL);
	//			m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
	//			m_col = index;
	//			ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());
	//		}
	//	}
	//	else
	//	{
	//		if (text == CString("φ"))
	//		{
	//			//圆棒料φ：G = Kd²L / 1000
	//			text =content[1].c_str();

	//			auto number = split(text.GetBuffer(), "*");
	//			if (number.size() == 2)
	//			{
	//				auto K = bangliao[pos];
	//				auto d = strtod(number[0].c_str(), NULL);
	//				auto L = strtod(number[1].c_str(), NULL);
	//				auto result = K*d*d*L / 1000;
	//				index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单件重量");
	//				if (index == -1)
	//				{
	//					MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//					return;
	//				}
	//				m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
	//				m_row = i;
	//				m_col = index;
	//				ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());

	//				index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单套数量");
	//				if (index == -1)
	//				{
	//					MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//					return;
	//				}
	//				CString DTSL = m_Partlist.GetItemText(i, index);
	//				if (DTSL.IsEmpty())
	//				{
	//					MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//					return;
	//				}

	//				index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "全套重量");
	//				if (index == -1)
	//				{
	//					MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//					return;
	//				}

	//				result = result * _ttoi(DTSL);
	//				m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
	//				m_col = index;
	//				ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());
	//			}
	//		}
	//		else
	//		{
	//			if (text == CString("δ"))
	//			{
	//				//板料δ：G = 厚度×宽度×长度×δ / 1000
	//				text =content[1].c_str();

	//				auto number = split(text.GetBuffer(), "*");
	//				if (number.size() == 3)
	//				{
	//					auto δ = banliao[pos];
	//					auto P = strtod(number[0].c_str(), NULL);
	//					auto W = strtod(number[1].c_str(), NULL);
	//					auto L = strtod(number[2].c_str(), NULL);
	//					auto result = P*W*L* δ / 1000;
	//					index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单件重量");
	//					if (index == -1)
	//					{
	//						MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//						return;
	//					}
	//					m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
	//					m_row = i;
	//					m_col = index;
	//					ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());

	//					index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "单套数量");
	//					if (index == -1)
	//					{
	//						MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//						return;
	//					}
	//					CString DTSL = m_Partlist.GetItemText(i, index);
	//					if (DTSL.IsEmpty())
	//					{
	//						MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//						return;
	//					}

	//					index = KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strMaterialMatch, "全套重量");
	//					if (index == -1)
	//					{
	//						MessageBox("定额计算错误", APS_MSGBOX_TITIE, MB_OK);
	//						return;
	//					}

	//					result = result * _ttoi(DTSL);
	//					m_Partlist.SetItemText(i, index, to_string(static_cast<long double>(result)).c_str());
	//					m_col = index;
	//					ModifyPartInfo(to_string(static_cast<long double>(result)).c_str());
	//				}
	//			}
	//		}
	//	}
	//}
//end add in 2018.11.26 by xjt for 70569	
	return;
}

//导入
void CDlgMaterialEdit::Import()
{
	// TODO:  在此添加控件通知处理程序代码
	
	CFileDialog FileDalog(TRUE, _T("xml"), NULL, OFN_READONLY, _T("(*.xml)|*.xml|所有文件(*.*)"), this);
	if (FileDalog.DoModal() != IDOK)
	{
		return;
	}

	CString File = FileDalog.GetPathName();

	char * Path = (LPSTR)(LPCTSTR)File;


	TiXmlDocument doc;
	if (!doc.LoadFile(Path))
	{
		MessageBox("导入文件有误", APS_MSGBOX_TITIE, MB_OK);
	}
	m_pPBOMxml = doc;
	TiXmlElement *root = m_pPBOMxml.FirstChildElement();
	if (root == NULL || root->FirstChildElement() == NULL)
	{
		MessageBox("导入文件有误", APS_MSGBOX_TITIE, MB_OK);
		m_pPBOMxml.Clear();
		return;
	}
	//导入不删除当前界面
	//partInfo.clear();
	auto value =root->Value();
	CWaitCursor wait;
	if (CString(value).CompareNoCase("wc:COLLECTION")==0)
	{
		pBomNumber =root->FirstChildElement()->Attribute("PBOMNUMBER");
		auto elem = root->FirstChildElement()->FirstChildElement();
		CTime clock;
		auto t_1 =GetTickCount();
		while (elem != NULL)
		{
			PartInfo part(elem);
			partInfo.push_back(part);
			elem = elem->NextSiblingElement();
		}
		auto t_2= GetTickCount();
		//AfxMessageBox(to_string( t_2- t_1).c_str());
		SelectPartition();
	}
	else
		MessageBox("导入文件有误", APS_MSGBOX_TITIE, MB_OK);
	wait.Restore();
}
//保存
void CDlgMaterialEdit::SaveDlg()
{
	// TODO:  在此添加控件通知处理程序代码
	CDlgSave dlg(pBomNumber);
	dlg.SetCaption("保存");
	if (IDOK ==dlg.DoModal())
	{
		m_path = dlg.m_sName;
		if (::PathFileExists(m_path))
		{
			//begin add in 2018.11.22 for 70604
			auto word = MessageBox("已存在文件，是否覆盖？", APS_MSGBOX_TITIE, MB_OKCANCEL | MB_ICONQUESTION);
			if ( word == IDOK)
			{
				DeleteFile(m_path);
			}
			else
			{
				if (word == IDCANCEL)
				{
					return;
				}	
			}
			//end add in 2018.11.22 for 70604
		}
		char *Path  =m_path.GetBuffer(m_path.GetLength());
		
		//auto Path =GetFilePath(m_path);
		//auto changepath =changePartName(pBomNumber);

		//auto SavePath =Path+changepath+".xml";
		m_path.ReleaseBuffer(m_path.GetLength());
		
		bool flag=saveXML(Path);
		if (flag)
		{
			MessageBox("保存成功", APS_MSGBOX_TITIE, MB_OK);
		}
		else
		{
			MessageBox("保存失败", APS_MSGBOX_TITIE, MB_OK);
		}
	} 
}


//检入
void CDlgMaterialEdit::Upload()
{
	auto isSuccess =saveXML("");

	CString m_strPath = GetModuleDir() + _T("\\Resources\\材料定额.xml");
	CString m_sTemplatePath = GetModuleDir() + _T("\\Resources\\材料定额.xls");

	CString File = GetFilePath(m_path) + GetMainFileName(m_path);
	File += ".xls";

	if (::PathFileExists(File))
		DeleteFile(File);

	TiXmlDocument doc;
	if (!doc.LoadFile(m_strPath))
	{
		MessageBox("资源配置文件有误", APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	TiXmlElement *root = doc.FirstChildElement();
	if (root == NULL || root->FirstChildElement() == NULL)
	{
		return;
	}
	//begin add xjt  in 2018.11.29 for 70559 
		auto SheetFirst = root->FirstChildElement();
		auto sheet = SheetFirst->Attribute("SheetFirst");
		if (CString(sheet).CompareNoCase("封面") == 0)
		{
			TiXmlElement * info = SheetFirst->FirstChildElement()->FirstChildElement();
			vector<string> Cover;  //每个excle表格的行列信息   先是列，再是行  中间以冒号分开 
			if (info)
			{
				TiXmlElement * Range=info->FirstChildElement();

				while (Range)
				{
					auto attr = Range->Attribute("Range");
					Cover.push_back(attr);
					Range = Range->NextSiblingElement();
				}
			}

			SheetFirst = SheetFirst->NextSiblingElement();
			sheet = SheetFirst->Attribute("SheetFirst");

			vector<string> FirstPageinfo;  
			TiXmlElement * Info = SheetFirst->FirstChildElement()->FirstChildElement();
			if (Info)
			{
				TiXmlElement * Range = Info->FirstChildElement();
				while (Range)
				{
					auto attr = Range->Attribute("Range");
					FirstPageinfo.push_back(attr);
					Range = Range->NextSiblingElement();
				}
			}
			vector<string> FirstPage;
			TiXmlElement * Grid = SheetFirst->FirstChildElement()->FirstChildElement()->NextSiblingElement();
			if (Grid)
			{
				TiXmlElement * Range = Grid->FirstChildElement();
				while (Range)
				{
					auto attr = Range->Attribute("Range");
					FirstPage.push_back(attr);
					Range = Range->NextSiblingElement();
				}
			}
	
			CApplication app;
			CWorkbook obook;
			CWorkbooks obooks;
			CWorksheet osheet;
			CWorksheets osheets;
			CRange range;

			COleVariant covTure((short)TRUE), covFalse((short)FALSE), covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
			try
			{	
				HANDLE hfile = CreateFile(File, FILE_ALL_ACCESS, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hfile ==INVALID_HANDLE_VALUE)
				{
					auto hr = GetLastError();
					if (hr == ERROR_ACCESS_DENIED || hr==ERROR_SHARING_VIOLATION)
					{
						MessageBox("文件已经打开，无法保存", APS_MSGBOX_TITIE, MB_OK);
						return;
					}
					else
						CloseHandle(hfile);
				}

				if (!app.CreateDispatch(_T("Excel.Application")))
				{
					return;
				}

				VARIANT varOpt;
				varOpt.vt = VT_ERROR;
				varOpt.scode = DISP_E_PARAMNOTFOUND;
				obooks = app.get_Workbooks();
				obook = obooks.Open(m_sTemplatePath, COleVariant(_T("0")), COleVariant(_T("False")), varOpt, varOpt, varOpt, varOpt, varOpt, varOpt, varOpt, varOpt, varOpt, varOpt, varOpt, varOpt);
				osheets = obook.get_Sheets();
				osheet = osheets.get_Item(COleVariant((short)2));

				COleVariant temp;
				temp.pdispVal = osheet.m_lpDispatch;
				temp.vt = VT_DISPATCH;

				SelectPartition();
				//获取表的信息
				CHeaderCtrl   *pmyHeaderCtrl = m_Partlist.GetHeaderCtrl(); //获取表头
				auto   m_cols = FirstPage.size(); //获取列数
				auto   m_rows = m_Partlist.GetItemCount();  //获取行数
				
				std::map<CString,int> MaterialTypeCount;

				for (auto it = MaterialType.begin(); it != MaterialType.end();++it)
				{
					MaterialTypeCount[*it]=0;

					for (auto partitem =partInfo.begin();partitem!=partInfo.end();++partitem)
					{
						if (CString(partitem->value["Type"].c_str()).CompareNoCase(*it)==0)
						{
							MaterialTypeCount[*it]++;
						}
					}
				}

				auto addrow =0;

				for (auto it =MaterialTypeCount.begin();it!=MaterialTypeCount.end();++it)
				{
					if (it->second!=0)
					{
						addrow++;
					}
				}

				auto total_row =(m_rows+addrow);
				auto pages = ( total_row% 25 == 0) ? total_row / 25 : total_row / 25 + 1;
				int page = 1;
				//pages = pages +1;
				while (page != pages && pages != 0)
				{
					osheet.Copy(vtMissing, temp);
					++page;
				}

				for (int j = 0; j <= osheets.get_Count()-2; ++j)
				{
					string name = "第" + to_string(static_cast<_LONGLONG>(j+1)) + "页";
					osheet = osheets.get_Item(COleVariant((short)(j + 2)));
					osheet.put_Name(name.c_str());

					auto XH = FirstPageinfo[0];
					auto Name = FirstPageinfo[1];
					auto Page = FirstPageinfo[2];
					
					auto POSITION = split(XH, ":");
					range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));
					CString   szTemp;
					szTemp = pBomNumber;
					BSTR   bstr = szTemp.AllocSysString();
					VARIANT varValue;
					varValue.vt = VT_BSTR;
					varValue.bstrVal = (_bstr_t)szTemp;
					range.put_Value2(varValue);
					SysFreeString(bstr);

					POSITION = split(Name, ":");
					range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));

					CString version;
					szTemp = "";
					for (auto partitem = partInfo.begin(); partitem != partInfo.end(); ++partitem)
					{
						if (partitem->value["Number"] == pBomNumber.GetBuffer())
						{
							szTemp = partitem->value["Name"].c_str();
							version = partitem->value["Version"].c_str();
							break;
						}
					}

					bstr = szTemp.AllocSysString();

					varValue.vt = VT_BSTR;
					varValue.bstrVal = (_bstr_t)szTemp;
					range.put_Value2(varValue);
					SysFreeString(bstr);

					POSITION = split(Page, ":");
					range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));

					szTemp = name.c_str();
					bstr = szTemp.AllocSysString();

					varValue.vt = VT_BSTR;
					varValue.bstrVal = (_bstr_t)szTemp;
					range.put_Value2(varValue);
					SysFreeString(bstr);
				}

				auto currentpage =2;
				osheet = osheets.get_Item(COleVariant((short)(currentpage)));
				currentpage++;
				int row = 0;
				int count = 0;
				
				for (auto it = MaterialType.begin(); it != MaterialType.end();++it)
				{
					SelectItem(*it);
					m_rows = m_Partlist.GetItemCount();  //获取行数
					if (m_rows>0)
					{
						if (count == 25)
						{
							osheet = osheets.get_Item(COleVariant((short)(currentpage)));	
							count = 0;
							currentpage++;
						}
						count++;
						row++;
						string left, right;
						left += "B"+to_string(static_cast<_LONGLONG>(6 + count));
						right += "B"+to_string(static_cast<_LONGLONG>(6 + count));

						range = osheet.get_Range(COleVariant(CString(left.c_str())), COleVariant(CString(right.c_str())));
						
						CString   szTemp;
						szTemp = to_string(static_cast<_LONGLONG>(row)).c_str();
						BSTR   bstr = szTemp.AllocSysString();             //The AllocSysString method allocates a new BSTR string that is Automation compatible
						VARIANT varValue;
						varValue.vt = VT_BSTR;
						varValue.bstrVal = (_bstr_t)szTemp;
						range.put_Value2(varValue);
						SysFreeString(bstr);
						
						left = "", right = "";

						left += "C" + to_string(static_cast<_LONGLONG>(6 + count));
						right += "C" + to_string(static_cast<_LONGLONG>(6 + count));
						range = osheet.get_Range(COleVariant(CString(left.c_str())), COleVariant(CString(right.c_str())));
						
						szTemp = *it;
						varValue.vt = VT_BSTR;
						varValue.bstrVal = (_bstr_t)szTemp;
						range.put_Value2(varValue);
						SysFreeString(bstr);
						
					}
					for (auto iRow = 0; iRow < m_rows; ++iRow) //将列表内容写入EXCEL 
					{
						if (count == 25)
						{
							osheet = osheets.get_Item(COleVariant((short)(currentpage)));
							count = 0;
							currentpage++;
						}
						count++;
						row++;
						for (size_t iCol = 0; iCol < m_cols; ++iCol)
						{
							string left, right;
							auto pos = FirstPage[iCol].find_first_of(':');

							for (size_t i1 = 0; i1 < pos - 1; ++i1)
							{
								left.push_back(FirstPage[iCol][i1]);
							}
							left += to_string(static_cast<_LONGLONG>(6+ count));

							for (size_t i1 = pos + 1; i1 < FirstPage[iCol].size() - 1; ++i1)
							{
								right.push_back(FirstPage[iCol][i1]);
							}

							right += to_string(static_cast<_LONGLONG>(6 + count));

							range = osheet.get_Range(COleVariant(CString(left.c_str())), COleVariant(CString(right.c_str())));
							if (left[0]=='B')
							{
								CString   szTemp;
							
								szTemp = to_string(static_cast<_LONGLONG>(row)).c_str();
								BSTR   bstr = szTemp.AllocSysString(); //The AllocSysString method allocates a new BSTR string that is Automation compatible
								VARIANT varValue;
								varValue.vt = VT_BSTR;
								varValue.bstrVal = (_bstr_t)szTemp;
								range.put_Value2(varValue);
								SysFreeString(bstr);
							}
							else
							{
								CString   szTemp;
								szTemp = m_Partlist.GetItemText(iRow, iCol);  //取得m_list控件中的内容
								BSTR   bstr = szTemp.AllocSysString();        //The AllocSysString method allocates a new BSTR string that is Automation compatible
								VARIANT varValue;
								varValue.vt = VT_BSTR;
								varValue.bstrVal = (_bstr_t)szTemp;
								range.put_Value2(varValue);
								SysFreeString(bstr);
							}
						}
					}
				}
					
				//填写封面信息
				osheet = osheets.get_Item(COleVariant((short)(1)));
				auto XH =Cover[0];
				auto Name = Cover[1];
				auto Version = Cover[2];
				auto Page = Cover[3];
				auto Time = Cover[4];

				auto POSITION=split(XH,":");
				range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));
				CString   szTemp;
				szTemp = pBomNumber;
				BSTR   bstr = szTemp.AllocSysString();
				VARIANT varValue;
				varValue.vt = VT_BSTR;
				varValue.bstrVal = (_bstr_t)szTemp;
				range.put_Value2(varValue);
				SysFreeString(bstr);

				POSITION =split(Name,":");
				range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));

				CString version;
				szTemp = "";

				for (auto partitem =partInfo.begin();partitem!=partInfo.end();++partitem)
				{
					if (partitem->value["Number"]==pBomNumber.GetBuffer())
					{
						szTemp = partitem->value["Name"].c_str();
						version = partitem->value["Version"].c_str();
					}
				}

				bstr = szTemp.AllocSysString();

				varValue.vt = VT_BSTR;
				varValue.bstrVal = (_bstr_t)szTemp;
				range.put_Value2(varValue);
				SysFreeString(bstr);

				POSITION =split(Version,":");
				range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));

				szTemp = version+"版";
				bstr = szTemp.AllocSysString();

				varValue.vt = VT_BSTR;
				varValue.bstrVal = (_bstr_t)szTemp;
				range.put_Value2(varValue);
				SysFreeString(bstr);

				POSITION =split(Page,":");
				range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));

				szTemp = CString("共") + to_string(static_cast<long long>(page)).c_str() + "页";
				bstr = szTemp.AllocSysString();
				varValue.vt = VT_BSTR;
				varValue.bstrVal = (_bstr_t)szTemp;
				range.put_Value2(varValue);
				SysFreeString(bstr);


				POSITION =split(Time,":");
				range = osheet.get_Range(COleVariant(CString(POSITION[0].c_str())), COleVariant(CString(POSITION[1].c_str())));
				SYSTEMTIME tmptime;
				GetLocalTime(&tmptime);
				char sTime[32] = { 0 };
				sprintf_s(sTime, "%.4d年%.2d月%.2d日", tmptime.wYear, tmptime.wMonth, tmptime.wDay);
				szTemp = sTime;
				bstr = szTemp.AllocSysString();
				varValue.vt = VT_BSTR;
				varValue.bstrVal = (_bstr_t)szTemp;
				range.put_Value2(varValue);
				SysFreeString(bstr);
				
				obook.SaveCopyAs(COleVariant(File)); //保存到cStrFile文件
				obook.put_Saved(true);
				obook.Close(covOptional, COleVariant(File), covOptional);
				obooks.Close();
				range.ReleaseDispatch();
				osheet.ReleaseDispatch();
				osheets.ReleaseDispatch();
				obook.ReleaseDispatch();
				obooks.ReleaseDispatch();
				
				app.Quit();
				app.ReleaseDispatch();

		}
	
			catch (...)
			{
				//begin add by xjt in 2018.11.27 for 70564 
				//			assert(FALSE);
				HANDLE process = OpenProcess(PROCESS_ALL_ACCESS,FALSE,GetProcessIDbyName("EXCEL.EXE"));
				if (process!=NULL)
				{
					BOOL result = TerminateProcess(process, 1);
				}

				//MessageBox("请检查Excel", APS_MSGBOX_TITIE, MB_OK);
				//end add by xjt in 2018.11.27 for 70564 
			}
	}

	//end add xjt  in 2018.11.29 for 70559 
	
	//检入
	auto time =GetRelativeDir();
	CString zipPath = GetFilePath(m_path) + time +".zip";

	KmZipUtil zip;
	if (zip.CreateZipFile(zipPath))
	{
		zip.ZipFile(m_path,NULL);
		zip.ZipFile(File,NULL);
		zip.CloseZipFile();
	}

	TCHAR TempDir[1024];
	GetTempPath(1024, TempDir);
	CString sTempDir(TempDir);
	CString tempdir = sTempDir + _T("KM3DCAPP-AWORK");


	if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
	{
		MessageBox("ftp连接失败！", APS_MSGBOX_TITIE, MB_OK);
		return;
	}
	CString strDir = zipPath.Left(zipPath.ReverseFind(_T('\\')) + 1);

	CString strNewSubPath;
	if (!GenAlonePath(strDir, strNewSubPath))
	{
		return;
	}

	//创建路径
	if (!CreateDirectory(strNewSubPath, NULL))
	{
		return;
	}

	CString strRelDir = "\\checkin\\doc\\";
	//BOOL bSucc = m_FTPInterface.UpLoad(strlocal, strFTPURL, strFTPPort, "3DAST", strFileName);
	BOOL bSucc = m_FTPInterface.UpLoad(zipPath, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strRelDir, GetFileName(zipPath));
	if (!bSucc)
	{
		MessageBox("上传失败！", APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	CString strInput =  
		CString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
		"<checkInDoc>"+
		"<partNumber>"+pBomNumber+"</partNumber>"+
		"<fileName>"+GetFileName(zipPath)+"</fileName>"+
		"</checkInDoc>";

	auto Xmlcontent = m_WebServiceInterface.CheckInDoc("1",strInput);
	//AfxMessageBox(Xmlcontent);
	bool bSucess = CWindChillXml::ParseResult(Xmlcontent);
	
	if (bSucess)
	{
		MessageBox("检入成功", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST );
	}
	else
	{
		MessageBox("检入失败", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST );
	}

	if (ExistDir(strNewSubPath))
	{
		DeleteDir(strNewSubPath);
	}
}

void CDlgMaterialEdit::OnTvnSelchangedWindchillMaterualtype(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	HTREEITEM hItem;
	hItem = m_materialtypeTree.GetSelectedItem();
	auto text = m_materialtypeTree.GetItemText(hItem);
	m_materialtypeTree.Expand(hItem,TVE_EXPAND);
	if (text == "分类")
	{
		if (partInfo.size() > 0)
		{
			SelectPartition();
		}
		m_materialtypeTree.SelectItem(hItem);
	}
	else
	{
		SelectItem(text);
	}
	*pResult = 0;
}

void CDlgMaterialEdit::OnLvnEndScrollWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	LPNMLVSCROLL pnmLVScroll = (LPNMLVSCROLL)(pNMHDR);
	m_editlist.ShowWindow(HIDE_WINDOW);
	m_ComboBox.ShowWindow(HIDE_WINDOW);
	*pResult = 0;
}

void CDlgMaterialEdit::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO:  在此处添加消息处理程序代码
	UPDATE_EASYSIZE;
}

void CDlgMaterialEdit::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	// TODO:  在此处添加消息处理程序代码
	EASYSIZE_MINSIZE(600, 370, fwSide, pRect);
}



bool CDlgMaterialEdit::saveXML(CString path)
{

	changePartName(path);
	TiXmlDocument doc;
	TiXmlElement*root = new TiXmlElement("wc:COLLECTION");
	root->SetAttribute("xmlns:wc", "\"http://www.ptc.com/infoengine/1.0\"");
	doc.LinkEndChild(root);
	TiXmlElement*BOM = new TiXmlElement("Bom");
	BOM->SetAttribute("NAME", "output");
	BOM->SetAttribute("TYPE", "OBZect");
	BOM->SetAttribute("STATUS", "0");
	BOM->SetAttribute("pBomNumber", pBomNumber.GetBuffer());
	root->LinkEndChild(BOM);
	for (auto item = partInfo.begin(); item != partInfo.end(); ++item)
	{
		TiXmlElement * WTPart = new TiXmlElement("wc:INSTANCE");
		for (auto it = item->value.begin(); it != item->value.end(); ++it)
		{
			auto key = it->first;
			auto word = it->second;
			TiXmlElement * item1 = new TiXmlElement(key.c_str());
			TiXmlText * text = new TiXmlText(word.c_str());
			item1->LinkEndChild(text);
			WTPart->LinkEndChild(item1);
		}

		TiXmlElement * Children = new TiXmlElement("CHINDREN");
		WTPart->LinkEndChild(Children);
		for (auto it = item->childvalue.begin(); it != item->childvalue.end(); ++it)
		{
			string child = "| ;;; ;;; ;;; ";

			if (it == item->childvalue.begin())
			{
				child = " ;;; ;;; ;;; ";
			}
			for (auto childitem = it->begin(); childitem != it->end(); ++childitem)
			{
				auto key = childitem->first;
				auto word = childitem->second;

				vector<string::size_type> Pos;
				auto pos = child.find(" ", 0);
				while (pos != string::npos)
				{
					Pos.push_back(pos);
					pos = child.find(" ", pos + 1);
				}

				if (string(key) == string("Number"))
				{
					child.insert(Pos[0], word);
				}

				if (string(key) == string("Name"))
				{
					child.insert(Pos[1], word);
				}

				if (string(key) == string("Count"))
				{
					child.insert(Pos[2], word);
				}

				if (string(key) == string("Unit"))
				{
					child.insert(Pos[3], word);
				}
			}

			int index = 0;
			if (!child.empty())
			{
				while ((index = child.find(' ', index)) != string::npos)
				{
					child.erase(index, 1);
				}
			}

			TiXmlText * text = new TiXmlText(child.c_str());
			Children->LinkEndChild(text);
		}
		BOM->LinkEndChild(WTPart);
	}
	if (path.IsEmpty())
	{
		TCHAR TempDir[1024];
		GetTempPath(1024, TempDir);
		CString sTempDir(TempDir);
		CString tempdir = sTempDir + _T("KM3DCAPP-AWORK");
		m_path = tempdir+"\\"+"CLXHDEB.xml";
	}
	else
		m_path = path;

	if (::PathFileExists(m_path))
	{
		DeleteFile(m_path);
	}

	char *Path = m_path.GetBuffer(m_path.GetLength());
	m_path.ReleaseBuffer(m_path.GetLength());
	bool flag = doc.SaveFile(Path);
	doc.Clear(); 

	return flag;
}


void CDlgMaterialEdit::OnNMRClickWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	auto selectHitem =m_materialtypeTree.GetSelectedItem();

	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CRect rc;
	CString strtemp;
	m_row = pNMListView->iItem;
	m_col = pNMListView->iSubItem;
	
	//双击空白行，新添加一行
	auto hitem =m_materialtypeTree.GetSelectedItem();
	if ( selectHitem != NULL &&m_row!=-1)
	{
		auto Text =m_materialtypeTree.GetItemText(selectHitem);
		if(Text.CompareNoCase("辅材")==0)
		{
			CMenu Menu;
			Menu.LoadMenu(IDR_DELETE);
			auto pm =Menu.GetSubMenu(0);
			CPoint pot;
			GetCursorPos(&pot);
			pm->TrackPopupMenu(TPM_LEFTALIGN,pot.x,pot.y,this);
		}
	}
	
	*pResult = 0;
}


void CDlgMaterialEdit::DeleteMatertion()
{
	// TODO: 在此添加命令处理程序代码
	
	auto text= m_Partlist.GetItemText(m_row,1);

	for (auto it = partInfo.begin();it!=partInfo.end();)
	{
		auto tempstr =it->value["Number"];
		if(text.CompareNoCase(tempstr.c_str())==0)
		{
			it =partInfo.erase(it);
		}
		if (it!=partInfo.end())
		{
			++it;
		}
	}


	SelectItem("辅材");
}
