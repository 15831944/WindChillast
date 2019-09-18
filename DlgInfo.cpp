// DlgInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "WindChill.h"
#include "DlgInfo.h"
#define IDC_PROP_INFO 3000003

// CDlgInfo 对话框

IMPLEMENT_DYNAMIC(CDlgInfo, CDialog)

CDlgInfo::CDlgInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInfo::IDD, pParent)
{

}

CDlgInfo::~CDlgInfo()
{
	//m_BCGPropList.DestroyWindow();
	//BCGCBProCleanUp();
}

void CDlgInfo::InitPage(ConfigPage *config, std::vector<PageData*>data)
{
	m_config = config;
	m_data = data;
}

BOOL CDlgInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitGridCtrl();
	
	/*
	m_PropGrid =new CMFCPropertyGridCtrl;
	CRect rc;
	GetWindowRect(&rc);
	ClientToScreen(&rc);
	rc.InflateRect(-2, -2, -2, -2);

	m_PropGrid->Create(WS_VISIBLE | WS_CHILD, rc,this,-1);
	m_PropGrid->ShowWindow(SW_SHOW);
	*/
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CDlgInfo::OnDestroy()
{
	return TRUE;
}

void CDlgInfo::InitGridCtrl()
{
	CRect rc;

	this->GetParent()->GetClientRect(&rc);
	rc.right = rc.right - 10;
	rc.bottom = rc.bottom - 5;

	m_BCGPropList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER , rc,this, IDC_PROP_INFO);
	
	m_BCGPropList.SetParent(this);
	m_BCGPropList.EnableHeaderCtrl(TRUE, _T("属性"),_T("值"));
	HDITEM item;
	item.cxy = 120;
	item.mask = HDI_WIDTH;

	m_BCGPropList.GetHeaderCtrl().SetItem(0, new HDITEM(item));

	auto Group = new CBCGPProp(_T("基本属性"));
	for (auto i = 0; i < m_config->m_InfoHeard.GetSize(); ++i)
	{
		auto InfoHead = m_config->m_InfoHeard.GetAt(i);

		if (m_data.empty())
		{
			continue;
		}
		auto InfoData = m_data[0]->m_pInfo->m_arRowData.GetAt(i);

		auto Name = InfoHead.m_strName;
		
		auto value = InfoData->m_arData[0].m_strVal;

		auto SubInfo = new CBCGPProp(Name.GetBuffer(),1,value.GetBuffer(),_T(""));

		SubInfo->SetData((DWORD_PTR)InfoData);
		Group->AddSubItem(SubInfo);
	}

	m_BCGPropList.AddProperty(Group);

}


void CDlgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
}


LRESULT CDlgInfo::OnProChanged(WPARAM uProListId,LPARAM pProp)
{
	auto prop1 =(CBCGPProp *)pProp;

	ASSERT(prop1);
	auto value =prop1->GetValue();
	auto data =(RowData *)prop1->GetData();
	ASSERT(data);
	data->m_arData[0].m_strVal =value;
	return 0;
}

BEGIN_MESSAGE_MAP(CDlgInfo, CDialog)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED,OnProChanged)
END_MESSAGE_MAP()



// CDlgInfo 消息处理程序
