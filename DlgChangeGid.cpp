// DlgChangeGid.cpp : 实现文件
//

#include "stdafx.h"
#include "WindChill.h"
#include "DlgChangeGid.h"
#include "afxdialogex.h"



#define BCGGRIDCTRL 30001

// CDlgChangeGid 对话框

IMPLEMENT_DYNAMIC(CDlgChangeGid, CDialog)

CDlgChangeGid::CDlgChangeGid(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_WINDCHILL_CHANGGRID, pParent)
{

}

CDlgChangeGid::~CDlgChangeGid()
{


}


void CDlgChangeGid::InitListCtrl()
{
	//将bcg控件位置和增加删除 位置 
	CRect rc;
	this->GetParent()->GetClientRect(&rc);
	rc.right =rc.right-60;
	rc.bottom = rc.bottom - 5;

	CRect addRc;
	GetDlgItem(IDC_WINDCHILL_ADDROW)->GetClientRect(&addRc);
	auto width = addRc.Width();
	addRc.left = rc.right + 10;
	addRc.right = addRc.left + width;
	GetDlgItem(IDC_WINDCHILL_ADDROW)->MoveWindow(addRc);

	CRect delRc;
	GetDlgItem(IDC_WINDCHILL_DELROW)->GetClientRect(&delRc);
	delRc.top = addRc.bottom + 10;
	delRc.left = rc.right + 10;
	delRc.right = delRc.left + width;
	delRc.bottom = delRc.top + addRc.Height();
	GetDlgItem(IDC_WINDCHILL_DELROW)->MoveWindow(delRc);


	//bcg控件初始化 
	m_BCGGridCtrl.Create(WS_CHILD | WS_VISIBLE | WS_BORDER|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT, rc, this, BCGGRIDCTRL);

	m_BCGGridCtrl.EnableMarkSortedColumn(FALSE);
	m_BCGGridCtrl.EnableHeader(TRUE, BCGP_GRID_HEADER_SELECT);
	m_BCGGridCtrl.EnableRowHeader();
	m_BCGGridCtrl.EnableLineNumbers();
	m_BCGGridCtrl.SetClearInplaceEditOnEnter(FALSE);
	m_BCGGridCtrl.EnableInvertSelOnCtrl();
	m_BCGGridCtrl.SetScalingRange(0.1, 4.0);
	m_BCGGridCtrl.LoadState(_T("EasyInputGrid"));
	m_BCGGridCtrl.DeleteAllColumns();

	//数据填充  
	m_BCGGridCtrl.InsertColumn(0,_T("更改标记"), 100);
	m_BCGGridCtrl.InsertColumn(1, _T("更改单号"), 100);            
	m_BCGGridCtrl.InsertColumn(2, _T("签字"), 100);

	auto row = 0;
	for (auto it = m_data.begin(); it != m_data.end(); ++it)
	{
		auto GridNum = (*it)->m_arGrid.GetSize();
		
		auto changeGrid = (*it)->m_arGrid[GridNum -1];

		if (!ISChangeGrid(changeGrid))
		{
			return;
		}
		
		for (auto i =0;i < changeGrid->m_arRowData.GetSize();++i)
		{
			auto rowdata = changeGrid->m_arRowData.GetAt(i);

			int nColumns = m_BCGGridCtrl.GetColumnCount();
			CBCGPGridRow* pRow = m_BCGGridCtrl.CreateRow(nColumns);
			m_BCGGridCtrl.AddRow(pRow, FALSE);

			for (auto j =0;j < rowdata->m_arData.GetSize();++j)
			{
				auto data = rowdata->m_arData.GetAt(j);
				
				auto GridItem = pRow->GetItem(j);
				GridItem->SetValue(data.m_strVal.GetBuffer());

				GridItem->SetData((DWORD_PTR)(rowdata));
			}
		}
	}

	m_BCGGridCtrl.AdjustLayout();
}

BOOL CDlgChangeGid::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitListCtrl();


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgChangeGid::OnDestroy()
{
	m_BCGGridCtrl.SaveState(_T("EasyInputGrid"));
	m_BCGGridCtrl.DestroyWindow();
}

bool CDlgChangeGid::ISChangeGrid(GridData *changeGrid)
{
	if (!changeGrid->m_arRowData.IsEmpty())
	{
		if (!changeGrid->m_arRowData.GetAt(0)->m_arData.IsEmpty())
		{
			auto str = changeGrid->m_arRowData.GetAt(0)->m_arData.GetAt(0).m_strRange;
			if (str.CompareNoCase("ggbj") != 0)
			{
			return	false;
			}
		}
		else
			return	false;
	}
	else
		return	false;
	return true;
}

void CDlgChangeGid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

}


void CDlgChangeGid::OnNMDblclkWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CRect rc;
	CString strtemp;
	m_row = pNMListView->iItem;
	m_col = pNMListView->iSubItem;

	*pResult = 0;
}

void CDlgChangeGid::InitPage(ConfigPage *config, std::vector<PageData*> data)
{
	m_config = config;
	m_data = data;
}

BEGIN_MESSAGE_MAP(CDlgChangeGid, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_CHANGEGRID, &CDlgChangeGid::OnNMDblclkWindchillPartlist)

	ON_BN_CLICKED(IDC_WINDCHILL_ADDROW, &CDlgChangeGid::OnBnClickedWindchillAddrow)
	ON_BN_CLICKED(IDC_WINDCHILL_DELROW, &CDlgChangeGid::OnBnClickedWindchillDelrow)

	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_CHANGED, OnGridtemChanged)
END_MESSAGE_MAP()


// CDlgChangeGid 消息处理程序


void CDlgChangeGid::OnBnClickedWindchillAddrow()
{
	// TODO: 在此添加控件通知处理程序代码
	//将更改单的信息输入到内存里 读取配置 若超过配置的最大行则要换页 输出 
	auto size =m_config->m_arConGrid.GetSize();
	auto ChangeGridConfig = m_config->m_arConGrid[size - 1];
	
	for (auto it =m_data.begin();it!=m_data.end();++it)
	{
		GridData  *ChangeGridData =NULL; 

		auto GridNum = (*it)->m_arGrid.GetSize();

		ChangeGridData = (*it)->m_arGrid[GridNum - 1];
		bool flag= false;
		if (!ISChangeGrid(ChangeGridData))
		{
			ChangeGridData = new GridData;
			flag = true;
		}
	
		//初始化GridData
		auto ranFor = new RanFormat;
		ranFor->strEd = "26";
		ranFor->strRange = "26";
		ranFor->strSt = "26";
		ChangeGridData->m_arRanFt.Add(*ranFor);
		
		delete ranFor;
		ranFor = NULL;

		auto rowdata = new RowData;

		for (auto i =0;i<ChangeGridConfig->m_arItem.GetSize();++i)
		{
			auto item = ChangeGridConfig->m_arItem.GetAt(i);
			auto data = new stCell;
		
			data->m_strRange = item.m_strRange;
			data->m_sFontName = item.m_sFontName;
			data->m_strName = item.m_strName;
			data->m_strVal = "";

			rowdata->m_arData.Add(*data);

			delete data;
			data = NULL;

		}
		ChangeGridData->m_arRowData.Add(rowdata);

		if (flag)
		{
			(*it)->m_arGrid.Add(ChangeGridData);
		}
		
	}

	int nColumns = m_BCGGridCtrl.GetColumnCount();
	CBCGPGridRow* pRow = m_BCGGridCtrl.CreateRow(nColumns);
	m_BCGGridCtrl.AddRow(pRow, FALSE);
	
	for (auto it = m_data.begin(); it != m_data.end(); ++it)
	{
		auto ChangeGridData = (*it)->m_arGrid[size - 1];
		for (auto i=0;i< ChangeGridData->m_arRowData.GetSize();++i)
		{
			auto Griddata = ChangeGridData->m_arRowData.GetAt(i);
			for (auto j =0;j<Griddata->m_arData.GetSize();++j)
			{
				auto GridItem =pRow->GetItem(j);

				GridItem->SetData((DWORD_PTR)(Griddata));
			}
		}
	}
	m_BCGGridCtrl.AdjustLayout();
}


void CDlgChangeGid::OnBnClickedWindchillDelrow()
{
	// TODO: 在此添加控件通知处理程序代码
	auto SelId =m_BCGGridCtrl.GetCurSelItemID();
	auto item =m_BCGGridCtrl.GetCurSelItem();
	
	if (SelId.m_nRow !=-1)
	{
		m_BCGGridCtrl.RemoveRow(SelId.m_nRow);
	}
}

LRESULT CDlgChangeGid::OnGridtemChanged(WPARAM uProListId, LPARAM pProp)
{
	auto selItem =m_BCGGridCtrl.GetCurSelItem();

	auto Rowdata = (RowData *)(selItem->GetData());

	if ( Rowdata || selItem)
	{
		auto text = selItem->GetValue();
		Rowdata->m_arData[selItem->GetColumnId()].m_strVal =text;
	}

	return 0;
}
