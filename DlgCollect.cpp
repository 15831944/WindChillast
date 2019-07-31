#include "DlgCollect.h"
#include "WindChillSetting.h"
#include <algorithm>

CDlgCollect::CDlgCollect(CWnd* pParent )
	: CDialog(CDlgCollect::IDD, pParent)
{

}


CDlgCollect::~CDlgCollect()
{
}

void CDlgCollect::InitCombox()
{
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();

	if (pSetting)
	{
		for (int i = 0; i < pSetting->m_strCollectionMatch.GetCount(); i++)
		{
			CString strName  = pSetting->m_strCollectionMatch.GetAt(i).m_strName;
			CString strValue = pSetting->m_strCollectionMatch.GetAt(i).m_strValue;

			if (!strName.IsEmpty())
			{
				m_combox.AddString(strName);
			}
		}
	}

	m_combox.SetCurSel(0);
	
}

void CDlgCollect::InitTab()
{
	CString text;
	m_combox.GetWindowTextA(text);

	auto it =m_tab.find(text);
	if (it!=m_tab.end())
	{
		auto tab=it->second;
		if (tab)
		{

			tab->ShowWindow(true);
		}
	
	}

}

void CDlgCollect::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_WINDCHILL_COMBOX, m_combox);

	DDX_Control(pDX, IDC_WINDCHILL_TAB, m_TabCtrl);
}


//BOOL CDlgCollect::PreTranslateMessage(MSG* pMsg)
//{
//	return TRUE;
//}

BOOL CDlgCollect::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitCombox();
	InitTab();
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CDlgCollect::DestroyWindow()
{
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
