// DlgExportPDF.cpp : 实现文件
//

#include "stdafx.h"
#include "MyCtrl.h"


IMPLEMENT_DYNAMIC(CMyGridCtrl, CBCGPGridCtrl)

BEGIN_MESSAGE_MAP(CMyGridCtrl, CBCGPGridCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
CMyGridCtrl::CMyGridCtrl()
{

}

CMyGridCtrl::~CMyGridCtrl()
{
}
BOOL CMyGridCtrl::PreTranslateMessage(MSG* pMsg)
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

	//begin add xieliang in 2018.09.28 for bug 70173
	if (pMsg->message == WM_LBUTTONDBLCLK)
		return 0;
	//end add xieliang in 2018.09.28 for bug 70173

	return CBCGPGridCtrl::PreTranslateMessage(pMsg);
}
void CMyGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetFocus();
}
void CMyGridCtrl::CreateCtrl(CWnd* pParentWnd)
{
	CRect parrect;
	pParentWnd->GetClientRect(&parrect);
	//CFont* Font = GetFont();
	CRect rect(10, 40, parrect.Width() / 2 - 15, parrect.bottom - 80);
	DWORD dwStyle = WS_BORDER | WS_VISIBLE;// | LVS_NOCOLUMNHEADER;
	//DWORD dwStyle = WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOCOLUMNHEADER | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES;

	//m_ProcList = new CMyGridCtrl;
	if (!Create(dwStyle, rect, pParentWnd, 0))
	{
		ASSERT("Create is failed!");
		return;
	}
	//begin add xieliang in 2018.09.28 for bug 70166
//	SetSingleSel(TRUE);
	//end add xieliang in 2018.09.28 for bug 70166
	//begin add xieliang in 2018.09.28 for bug 70174
	EnableHeader(TRUE, 0);
	EnableRowHeader(FALSE);
	//end add xieliang in 2018.09.28 for bug 70174
	SetWindowPos(&CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	
}
