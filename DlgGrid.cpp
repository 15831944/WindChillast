// DlgGrid.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WindChill.h"
#include "DlgGrid.h"
#include "afxdialogex.h"


// CDlgGrid �Ի���

IMPLEMENT_DYNAMIC(CDlgGrid, CDialog)

CDlgGrid::CDlgGrid(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_WINDCHILL_GRID, pParent)
{
	m_row = -1;
	m_col = -1;

	m_GridConfigNum = 0;
	m_ChildGridConfigNum = 0;
}

CDlgGrid::~CDlgGrid()
{
}

void CDlgGrid::InitPage(ConfigGrid *config, std::vector<GridData*>data)
{

	m_config = config;
	m_data = data;
}

void CDlgGrid::InitListCtrl()
{
	DWORD_PTR dwStyle = m_GridListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_GridListCtrl.SetExtendedStyle(dwStyle);

	//�ж��������Ƿ�Ϊ��   ��Ϊ����ֱ�Ӱ��������ߣ�����Ϊ�� ����������Ϊ������ + ��һ�� 
	if (!m_config->m_arChild.IsEmpty() )
	{
		for (auto i = 0; i < m_config->m_arItem.GetSize(); ++i)
		{
			CString strTmp = m_config->m_arItem.GetAt(i).m_strName;
			if (!strTmp.IsEmpty())
			{
				m_GridListCtrl.InsertColumn(i, strTmp, LVCFMT_LEFT, 80);
				m_GridListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
			}
		}
		m_GridConfigNum = m_config->m_arItem.GetSize();

		auto childConfig = m_config->m_arChild[0];

		for (int i = 0,j= childConfig->m_arItem.GetSize(); i < childConfig->m_arItem.GetSize(); ++i,++j)
		{
			CString strTmp = childConfig->m_arItem.GetAt(i).m_strName;
			if (!strTmp.IsEmpty())
			{
				m_GridListCtrl.InsertColumn(j, strTmp, LVCFMT_LEFT, 80);
				m_GridListCtrl.SetColumnWidth(j, LVSCW_AUTOSIZE_USEHEADER);
			}
		}
		m_ChildGridConfigNum = childConfig->m_arItem.GetSize();
		
		int row = 0;
		for (auto it = m_data.begin(); it != m_data.end(); ++it)
		{
			//���ʱ�ȱ��� �������ڱ���������
			for (auto i = 0; i < (*it)->m_arRowData.GetSize(); ++i)
			{
				
				auto item = (*it)->m_arRowData.GetAt(i);
				if (!item->m_arData.IsEmpty())
				{
					//�������
					for (auto j = 0; j < item->m_arData.GetSize(); ++j)
					{
						int tmpcol = 0;
						if (j % m_GridConfigNum == 0)
						{
							m_GridListCtrl.InsertItem(row, _T(""));
							auto value = item->m_arData.GetAt(j).m_strVal;
							m_GridListCtrl.SetItemText(row, tmpcol, value);

							if (!(*it)->m_arChildGrid.IsEmpty())
							{
								auto childItem = (*it)->m_arChildGrid.GetAt(0);
								for (auto k =0;k<childItem->m_arRowData.GetSize();++k)
								{
									if (row ==k)
									{
										auto Itemdata = (ListData *)m_GridListCtrl.GetItemData(row);
										ListData* Listptr = NULL;
										if (Itemdata==NULL)
										{
											Listptr = new ListData;
											Listptr->ParentConfig = NULL;
											Listptr->ChildConfig = NULL;
										}
										if (Listptr)
										{
											Listptr->ParentConfig = item;
										}
									
										auto childinfo = childItem->m_arRowData.GetAt(k);
										for (auto x = 0; x < childinfo->m_arData.GetSize(); ++x)
										{
											auto childvalue = childinfo->m_arData.GetAt(x).m_strVal;
											m_GridListCtrl.SetItemText(row, m_GridConfigNum + x, childvalue);
										}
										if (Listptr)
										{
											Listptr->ChildConfig = childinfo;
											m_GridListCtrl.SetItemData(row, (DWORD_PTR)(Listptr));
										}
									}
								}
							}
							
							row++;
							tmpcol = 0;
						}
						else
						{
							tmpcol++;
						}
					}
			
				}
			}


			if (m_GridConfigNum ==0)
			{
				for (auto i = 0; i < (*it)->m_arChildGrid.GetSize(); ++i)
				{
					int row = 0;
					auto GridInfo = (*it)->m_arChildGrid.GetAt(i);
					for (auto j = 0; j < GridInfo->m_arRowData.GetSize(); ++j)
					{
						auto item = GridInfo->m_arRowData.GetAt(j);
						m_GridListCtrl.InsertItem(row, _T(""));

						auto Itemdata = (ListData *)m_GridListCtrl.GetItemData(row);
						ListData* Listptr = NULL;
						if (Itemdata == NULL)
						{
							Listptr = new ListData;
							Listptr->ParentConfig = NULL;
							Listptr->ChildConfig = NULL;
						}
						

						for (auto k = 0; k < item->m_arData.GetSize(); ++k)
						{
							auto value = item->m_arData.GetAt(k).m_strVal;
							m_GridListCtrl.SetItemText(row, k, value);
							if (Listptr)
							{
								Listptr->ChildConfig = item;
							}
						}
						if (Listptr)
						{
						
							m_GridListCtrl.SetItemData(row, (DWORD_PTR)(Listptr));
						}
					
						row++;
					}
				}
			}
		}
		
	}
	else
	{
		for (auto i = 0; i < m_config->m_arItem.GetSize(); ++i)
		{
			CString strTmp = m_config->m_arItem.GetAt(i).m_strName;
			if (!strTmp.IsEmpty())
			{
				m_GridListCtrl.InsertColumn(i, strTmp, LVCFMT_LEFT, 0);
				m_GridListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
			}
		}
		m_GridConfigNum = m_config->m_arItem.GetSize();

		auto dataList =new ListData;
		int row = 0;
		for (auto it = m_data.begin(); it != m_data.end(); ++it)
		{
			for (auto i = 0; i < (*it)->m_arRowData.GetSize(); ++i)
			{
				auto item = (*it)->m_arRowData.GetAt(i);
				m_GridListCtrl.InsertItem(row, _T(""));
				for (auto j = 0; j < item->m_arData.GetSize(); ++j)
					{
						auto value = item->m_arData.GetAt(j).m_strVal;
						m_GridListCtrl.SetItemText(row, j, value);
					}

				dataList->ParentConfig = item;
				m_GridListCtrl.SetItemData(row, (DWORD_PTR)dataList);
				row++;
			}
		}
		
	}
	m_GridListCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

BOOL CDlgGrid::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitListCtrl();
	CRect rc;
	this->GetParent()->GetClientRect(&rc);
	rc.right = rc.right - 10;
	rc.bottom = rc.bottom - 10;

	m_GridListCtrl.MoveWindow(rc);
	m_GridListCtrl.ShowWindow(SW_SHOW);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDlgGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WINDCHILL_LISTGRID, m_GridListCtrl);
	DDX_Control(pDX, IDC_GRID_EDIT, m_EditList);
}


BEGIN_MESSAGE_MAP(CDlgGrid, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_WINDCHILL_LISTGRID, &CDlgGrid::OnNMDblclkWindchillPartlist)
	ON_EN_KILLFOCUS(IDC_GRID_EDIT, &CDlgGrid::OnEnKillfocusWindchillEditlist)
END_MESSAGE_MAP()


void CDlgGrid::OnNMDblclkWindchillPartlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CRect rc;
	CString strtemp;
	m_row = pNMListView->iItem;
	m_col = pNMListView->iSubItem;

	m_GridListCtrl.GetSubItemRect(m_row, m_col, LVIR_LABEL, rc);
	m_EditList.SetParent(&m_GridListCtrl);
	m_EditList.MoveWindow(rc);
	m_EditList.SetWindowText(m_GridListCtrl.GetItemText(m_row, m_col));
	m_EditList.ShowWindow(SW_SHOW);
	m_EditList.SetFocus();
	m_EditList.ShowCaret();
	m_EditList.SetSel(-1);
	m_EditList.ShowWindow(SW_SHOW);
	*pResult = 0;
}

// CDlgGrid ��Ϣ�������

void CDlgGrid::OnEnKillfocusWindchillEditlist()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_EditList.GetWindowText(str);
	if (m_row!=-1)
	{
		m_GridListCtrl.SetItemText(m_row, m_col, str);
		m_EditList.ShowWindow(SW_HIDE);

		//��������
		if (m_ChildGridConfigNum > 0)
		{
			if (m_GridConfigNum == 0) //ֻ��������
			{
				auto Itemdata = (ListData *)m_GridListCtrl.GetItemData(m_row);
				if (Itemdata && Itemdata->ChildConfig )
				{
					Itemdata->ChildConfig->m_arData[m_col].m_strVal = str;
				}
			}
			else
			{
				auto Itemdata = (ListData *)m_GridListCtrl.GetItemData(m_row);
				
				if (m_col <m_GridConfigNum) //�޸�����Ϊ������
				{
					if (Itemdata && Itemdata->ParentConfig)
					{
						Itemdata->ParentConfig->m_arData[m_col].m_strVal = str;
					}
				}
				else          // �޸�����Ϊ����������
				{
					if (Itemdata && Itemdata->ChildConfig)
					{
						Itemdata->ChildConfig->m_arData[m_col -m_GridConfigNum].m_strVal = str;
					}
				}
			}
		}
		else
		{
			auto Itemdata = (ListData *)m_GridListCtrl.GetItemData(m_row);
			if (Itemdata && Itemdata->ParentConfig)
			{
				Itemdata->ParentConfig->m_arData[m_col].m_strVal = str;
			}
		}
	}
}
