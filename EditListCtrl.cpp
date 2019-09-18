///////////////////////////////////////////////////////////////////////
// 文 件 名 : EditListCtrl.cpp
// 描    述 : 
// 公    司 : 武汉开目信息技术有限责任公司
// 项 目 组 : KMAPS3.0
// 创 建 者 : 吴克兵
// 创建日期 : 2009-08-10
///////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "EditListCtrl.h"
//#include "../KmCommonCtrl/DirectoryDialog.h"
//#include "../OpConnect/AssemPartFileDialog.h"
//#include "../OpConnect/FunAcc.h"
//#include "../OpConnect/OpConnectLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_EDITCELL	10001
#define IDC_BTN_BROWSER 10002
#define IDC_COMBOCELL	10003

/////////////////////////////////////////////////////////////////////////////
// CEditCell
CEditCell::CEditCell(CEditListCtrl* pCtrl, int iItem, int iSubItem, CString sInitText)
    : m_bEscape(FALSE)
{
    m_pListCtrl = pCtrl;
    m_Item = iItem;
    m_SubItem = iSubItem;
    m_InitText = sInitText;
}

CEditCell::~CEditCell()
{
	
}

BEGIN_MESSAGE_MAP(CEditCell, CEdit)
    //{{AFX_MSG_MAP(CEditCell)
    ON_WM_KILLFOCUS()
    ON_WM_NCDESTROY()
    ON_WM_CHAR()
    ON_WM_CREATE()
    ON_WM_GETDLGCODE()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCell message handlers

void CEditCell::SetListText()
{
    CString Text;
    GetWindowText(Text);

    // Send Notification to parent of ListView ctrl
    LV_DISPINFO dispinfo;
    dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
    dispinfo.hdr.idFrom = GetDlgCtrlID();
    dispinfo.hdr.code = LVN_ENDLABELEDIT;

    dispinfo.item.mask = LVIF_TEXT;
    dispinfo.item.iItem = m_Item;
    dispinfo.item.iSubItem = m_SubItem;
    dispinfo.item.pszText = m_bEscape ? NULL : LPTSTR((LPCTSTR) Text);
    dispinfo.item.cchTextMax = Text.GetLength();

    GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(),(LPARAM) &dispinfo);
}

BOOL CEditCell::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN)
    {
	    if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DELETE || 
			pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_TAB || 
			pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || GetKeyState(VK_CONTROL))
			{
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;		    	// DO NOT process further
			}
    }

    return CEdit::PreTranslateMessage(pMsg);
}

void CEditCell::OnKillFocus(CWnd* pNewWnd) 
{
    CEdit::OnKillFocus(pNewWnd);

    SetListText();

    // DestroyWindow();
}

void CEditCell::OnNcDestroy() 
{
//	SetListText();

    CEdit::OnNcDestroy();
    
//    delete this;
}

void CEditCell::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    // Up and down are in the OnKeyDown so that the user can hold down the arrow
    // keys to scroll through the entries.
//     BOOL Control = GetKeyState(VK_CONTROL) < 0;
//     switch (nChar)
//     {
// 		case VK_UP :
// 		{
// 			if (Item > 0)
// 				pListCtrl->EditSubItem(Item - 1, SubItem);
// 			return;
// 		}
// 		case VK_DOWN :
// 		{
// 			pListCtrl->EditSubItem(Item + 1, SubItem);
// 			return;
// 		}
// 		case VK_HOME :
// 		{
// 			if (!Control)
// 				break;
// 
// 			pListCtrl->EditSubItem(0, SubItem);
// 			return;
// 		}
// 		case VK_END :
// 		{
// 			if (!Control)
// 				break;
// 
// 			int Count = pListCtrl->GetItemCount() - 1;
// 			pListCtrl->EditSubItem(Count, SubItem);
// 			return;
// 		}
//     }
    
    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEditCell::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//     switch (nChar)
//     {
// 		case VK_NEXT :
// 		{
// 			int Count = pListCtrl->GetItemCount();
// 			int NewItem = Item + pListCtrl->GetCountPerPage();
// 			if (Count > NewItem)
// 				pListCtrl->EditSubItem(NewItem, SubItem);
// 			else
// 				pListCtrl->EditSubItem(Count - 1, SubItem);
// 			return;
// 		}
// 		case VK_PRIOR :
// 		{
// 			int NewItem = Item - pListCtrl->GetCountPerPage();
// 			if (NewItem > 0)
// 				pListCtrl->EditSubItem(NewItem, SubItem);
// 			else
// 				pListCtrl->EditSubItem(0, SubItem);
// 			return;
// 		}
//     }
    
    CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CEditCell::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    BOOL Shift = GetKeyState(VK_SHIFT) < 0;
    switch (nChar)
    {
		case VK_ESCAPE :
		{
			if (nChar == VK_ESCAPE)
				m_bEscape = TRUE;
			GetParent()->SetFocus();
			return;
		}
		case VK_RETURN :
		{
			SetListText();
			m_pListCtrl->EditSubItem(m_Item + 1, 0);
			return;
		}
		case VK_TAB :
		{
			if (Shift)
			{
				if (m_SubItem > 0)
					m_pListCtrl->EditSubItem(m_Item, m_SubItem - 1);
				else
				{
					if (m_Item > 0)
						m_pListCtrl->EditSubItem(m_Item - 1, 2);
				}
			}
			else
			{
				if (m_SubItem < 2)
					m_pListCtrl->EditSubItem(m_Item, m_SubItem + 1);
				else
					m_pListCtrl->EditSubItem(m_Item + 1, 0);
			}
			return;
		}
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);

    // Resize edit control if needed

    // Get text extent
    CString Text;

    GetWindowText(Text);
    CWindowDC DC(this);
    CFont *pFont = GetParent()->GetFont();
    CFont *pFontDC = DC.SelectObject(pFont);
    CSize Size = DC.GetTextExtent(Text);
    DC.SelectObject(pFontDC);
    Size.cx += 5;			   	// add some extra buffer

    // Get client rect
    CRect Rect, ParentRect;
    GetClientRect(&Rect);
    GetParent()->GetClientRect(&ParentRect);

    // Transform rect to parent coordinates
    ClientToScreen(&Rect);
    GetParent()->ScreenToClient(&Rect);

    // Check whether control needs to be resized and whether there is space to grow
    if (Size.cx > Rect.Width())
    {
		if (Size.cx + Rect.left < ParentRect.right )
			Rect.right = Rect.left + Size.cx;
		else
			Rect.right = ParentRect.right;
		MoveWindow(&Rect);
    }
}

int CEditCell::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

    // Set the proper font
    CFont* Font = GetParent()->GetFont();
    SetFont(Font);

    SetWindowText(m_InitText);
    SetFocus();
    SetSel(0, -1);
    return 0;
}

UINT CEditCell::OnGetDlgCode() 
{
    return CEdit::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTTAB;
}

//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl

CEditListCtrl::CEditListCtrl(CString Text /* = "Some Text" */)
{
    m_strDefaultText = Text;
	m_strNewType     = "";
	m_strNewUit      = "";

	m_pEdit	 = NULL;
	m_pCombo = NULL;
	m_pBtn	 = NULL;
}

CEditListCtrl::~CEditListCtrl()
{
}

BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
    //{{AFX_MSG_MAP(CEditListCtrl)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    //ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_WM_LBUTTONDOWN()
	//ON_NOTIFY_REFLECT(NM_KILLFOCUS, OnKillfocus)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BTN_BROWSER, OnBtnBrowser)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl message handlers

void CEditListCtrl::Resize(int cx, int cy)
{
    CRect Rect(0, 0, cx, cy);
    MoveWindow(&Rect);
    InvalidateRect(Rect);
    SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
}

CEdit* CEditListCtrl::EditSubItem(int Item, int Column)
{
    // The returned pointer should not be saved
	if(Column == 0)
	{
		return NULL;
	}
	
    // Make sure that the item is visible
    if (!EnsureVisible (Item, TRUE)) 
    {
		InsertItemEx(Item);
		if (!EnsureVisible(Item, TRUE)) 
			return NULL;
    }

    // Make sure that nCol is valid
    CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();
    if (Column >= nColumnCount || GetColumnWidth (Column) < 5)
		return NULL;

    // Get the column offset
    int Offset = 0;
    for (int iColumn = 0; iColumn < Column; iColumn++)
		Offset += GetColumnWidth(iColumn);

    CRect Rect;
    GetItemRect(Item, &Rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect ClientRect;
    GetClientRect(&ClientRect);
    if (Offset + Rect.left < 0 || Offset + Rect.left > ClientRect.right)
    {
		CSize Size;
		if (Offset + Rect.left > 0)
			Size.cx = -(Offset - Rect.left);
		else
			Size.cx = Offset - Rect.left;
		Size.cy = 0;
		Scroll(Size);
		Rect.left -= Size.cx;
    }

    // Get Column alignment
    LV_COLUMN lvCol;
    lvCol.mask = LVCF_FMT;
    GetColumn(Column, &lvCol);
    DWORD dwStyle;
    if ((lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
    else if ((lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
    else dwStyle = ES_CENTER;

    Rect.left += Offset + 1;
    Rect.right = Rect.left + GetColumnWidth(Column) - 25;
	Rect.top -= 3;
    if (Rect.right > ClientRect.right)
	{
		Rect.right = ClientRect.right;
	}

    dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

	if(m_pEdit)
	{
		if(m_pEdit->GetSafeHwnd())
		{
			m_pEdit->DestroyWindow();
		}

		delete m_pEdit;
		m_pEdit = NULL;
	}

    m_pEdit = new CEditCell(this, Item, Column, GetItemText(Item, Column));
    m_pEdit->Create(dwStyle, Rect, this, IDC_EDITCELL);

	if (m_pBtn)
	{
		if (m_pBtn->GetSafeHwnd())
		{
			m_pBtn->DestroyWindow();
		}
		delete m_pBtn;
		m_pBtn = NULL;
	}

	CRect rectBtn(Rect);
	rectBtn.left   = Rect.right - 1;
	rectBtn.right  = rectBtn.left + 25;	

	m_pBtn = new CButton();
	m_pBtn->Create(_T("..."), BS_DEFPUSHBUTTON, rectBtn, this, IDC_BTN_BROWSER);
	m_pBtn->SetFont(GetFont());
	m_pBtn->ShowWindow(SW_NORMAL);

    return m_pEdit;
}

CComboxCell* CEditListCtrl::ComboSubItem(int Item, int Column)
{
    // The returned pointer should not be saved
	if(Column == 0)
	{
		return NULL;
	}
	
    // Make sure that the item is visible
    if (!EnsureVisible (Item, TRUE)) 
    {
		InsertItemEx(Item);
		if (!EnsureVisible(Item, TRUE)) 
			return NULL;
    }

    // Make sure that nCol is valid
    CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();
    if (Column >= nColumnCount || GetColumnWidth (Column) < 5)
		return NULL;

    // Get the column offset
    int Offset = 0;
    for (int iColumn = 0; iColumn < Column; iColumn++)
		Offset += GetColumnWidth(iColumn);

    CRect Rect;
    GetItemRect(Item, &Rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect ClientRect;
    GetClientRect(&ClientRect);
    if (Offset + Rect.left < 0 || Offset + Rect.left > ClientRect.right)
    {
		CSize Size;
		if (Offset + Rect.left > 0)
			Size.cx = -(Offset - Rect.left);
		else
			Size.cx = Offset - Rect.left;
		Size.cy = 0;
		Scroll(Size);
		Rect.left -= Size.cx;
    }

    // Get Column alignment
    LV_COLUMN lvCol;
    lvCol.mask = LVCF_FMT;
    GetColumn(Column, &lvCol);
    
    Rect.left += Offset/* + 4*/;
    Rect.right = Rect.left + GetColumnWidth(Column);
    if (Rect.right > ClientRect.right)
	{
		Rect.right = ClientRect.right;
	}

	if(m_pCombo)
	{
		if(m_pCombo->GetSafeHwnd())
		{
			m_pCombo->DestroyWindow();
		}

		delete m_pCombo;
		m_pCombo = NULL;
	}

    return CreateCombo(Item, Column, GetItemText(Item, Column), Rect);
}

CComboxCell* CEditListCtrl::CreateCombo(int iItem, int iSubItem, CString sInitText, CRect rect)
{
	m_pCombo = new CComboxCell(this, iItem, iSubItem, GetItemText(iItem, iSubItem));

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE 
					| WS_VSCROLL | CBS_AUTOHSCROLL |CBS_DROPDOWN ;
	m_pCombo->Create(dwStyle, rect, this, IDC_COMBOCELL);

	rect.top -= 4;
	m_pCombo->ReSize(rect);

	return m_pCombo;
}

int CEditListCtrl::HitTestEx(CPoint& Point, int* pColumn)
{
    int ColumnNum = 0;
    int Row = HitTest(Point, NULL);
    
    if (pColumn)
		*pColumn = 0;

    // Make sure that the ListView is in LVS_REPORT
    if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return Row;

    // Get the top and bottom row visible
    Row = GetTopIndex();
    int Bottom = Row + GetCountPerPage();
    if (Bottom > GetItemCount())
	    Bottom = GetItemCount();
    
    // Get the number of columns
    CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();

    // Loop through the visible rows
    for(; Row <= Bottom; Row++)
    {
		// Get bounding rect of item and check whether point falls in it.
		CRect Rect;
		GetItemRect(Row, &Rect, LVIR_BOUNDS);
		if (Rect.PtInRect (Point))
		{
			// Now find the column
			for (ColumnNum = 0; ColumnNum < nColumnCount; ColumnNum++)
			{
				int ColWidth = GetColumnWidth(ColumnNum);
				if (Point.x >= Rect.left && Point.x <= (Rect.left + ColWidth))
				{
					if (pColumn)
						*pColumn = ColumnNum;
					return Row;
				}
				Rect.left += ColWidth;
			}
		}
    }
    return -1;
}

BOOL hexNumberToInt(CString HexNumber, int& Number)
{
    char* pStopString;
    Number = strtoul(HexNumber, &pStopString, 16);
    return Number != ULONG_MAX;
} // hexNumberToInt

int CEditListCtrl::InsertItemEx(int Item)
{
    int Result = InsertItem(Item + 1, m_strDefaultText);
    CString ItemVal, Temp;
    if (Item == 0)
    {
        ItemVal = "1000";
    }
    else
	{
		int HexVal;
		Temp = GetItemText(Item - 1, 1);
		hexNumberToInt(Temp, HexVal);
		ItemVal.Format("%x", HexVal + 1);
	}

    SetItemText(Item, 1, ItemVal);

    SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

    return Result;
}

void CEditListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (GetFocus() != this) 
    {
        SetFocus();
    }
    
    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEditListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (GetFocus() != this) 
		SetFocus();
    
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEditListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
    LV_ITEM	*plvItem = &plvDispInfo->item;

    if (plvItem->pszText != NULL && m_strDefaultText != CString(plvItem->pszText))
    {
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);		
		GetParent()->SendMessage(WM_ENDEDIT, (WPARAM)plvItem->iItem, (LPARAM)plvItem->iSubItem);
		m_strDefaultText = plvItem->pszText;

		if (m_strNewType != "")
		{
			SetItemText(plvItem->iItem, 3, m_strNewType);
			GetParent()->SendMessage(WM_ENDEDIT, (WPARAM)plvItem->iItem, 3);
		}

		//if (m_strNewUit != "")
		//{
		//	SetItemText(plvItem->iItem, 4, m_strNewUit);
		//	GetParent()->SendMessage(WM_ENDEDIT, (WPARAM)plvItem->iItem, 4);
		//}
    }
	m_strNewType = "";
	//m_strNewUit  = "";

    *pResult = FALSE;
}

void CEditListCtrl::OnLButtonDown(UINT nFlags, CPoint Point) 
{
	int nIndex   = 0;
    int nColNum  = 0;
	nIndex = HitTestEx(Point, &nColNum);
    
    if (m_nRow != nIndex || m_nCol != nColNum)
	{
		if(m_pEdit)
		{
			if(m_pEdit->GetSafeHwnd())
			{
				m_pEdit->SetListText();
				m_pEdit->DestroyWindow();
			}

			delete m_pEdit;
			m_pEdit = NULL;
		}

		if(m_pBtn)
		{
			if(m_pBtn->GetSafeHwnd())
			{
				m_pBtn->DestroyWindow();
			}

			delete m_pBtn;
			m_pBtn = NULL;
		}

		if (m_pCombo)
		{
			if(m_pCombo->GetSafeHwnd())
			{
				m_pCombo->DestroyWindow();
			}

			delete m_pCombo;
			m_pCombo = NULL;
		}
	}
	m_nRow = nIndex;
	m_nCol = nColNum;

	m_strDefaultText = GetItemText(m_nRow, m_nCol);

    CListCtrl::OnLButtonDown(nFlags, Point);
}

void CEditListCtrl::AddResString(const CString &str)
{
	if(m_pEdit)
	{
		m_pEdit->ReplaceSel(str);
	}
}

void CEditListCtrl::KillEdit()
{
	if(m_pEdit)
	{
		if(m_pEdit->GetSafeHwnd())
		{
			m_pEdit->DestroyWindow();
		}

		delete m_pEdit;

		m_pEdit = NULL;
	}
}

bool CEditListCtrl::IsEdit()
{
	if(m_pEdit && m_pEdit->GetSafeHwnd())
	{
		return true;
	}

	return false;
}

void CEditListCtrl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CEditListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int Index   = 0;
    int ColNum  = 0;
    
    if ((Index = HitTestEx(point, &ColNum)) != -1)
    {
		// 第2、3列可编辑
        if (ColNum == 1 || ColNum == 2)
        {
            EditSubItem(Index, ColNum);
        }
		// KmAps 3.0 内部优化。 Modify by 吴克兵 2009-10-13
		// 第4、5列combox
		else if (ColNum == 3 || ColNum == 4)
		{
			ComboSubItem(Index, ColNum);
		}
		// end Modify by 吴克兵 2009-10-13
		else
		{
			if(m_pEdit)
			{
				if(m_pEdit->GetSafeHwnd())
				{
					m_pEdit->DestroyWindow();
				}

				delete m_pEdit;

				m_pEdit = NULL;
			}
		}
    }
	else
	{
		if(m_pEdit)
		{
			if(m_pEdit->GetSafeHwnd())
			{
				m_pEdit->DestroyWindow();
			}

			delete m_pEdit;

			m_pEdit = NULL;
		}
	}
	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}


void CEditListCtrl::OnBtnBrowser()
{
	if (m_pEdit)
	{
		CString strPath;
		m_pEdit->GetWindowText(strPath);
		switch(m_nCol)
		{
		case 1:
			{
				CDirectoryDialog dirDlg;
				strPath = dirDlg.ShowDirectoryTree(strPath);
			}
			break;
		case 2:
			{
				CAssemPartFileDialogIO dlgIO(AfxGetMainWnd(), FALSE, FALSE);
				if(IDOK != dlgIO.DoModal())
				{
					return ;
				}
				
				const CStringArray& arFileNames = dlgIO.GetFileNames();
				if (arFileNames.GetSize() > 0)
				{
					strPath = arFileNames[0];
				}
				
				
				//CString strFileType = dlgIO.GetFileType();	
				CString strFileType = _T("");
				Get3DFileType(strPath, strFileType.GetBuffer(128), 128);
				strFileType.ReleaseBuffer();

				int nUnitID = dlgIO.GetFileUnit();	
				CString strUint;
				//if (nUnitID < 14)
				//{
				//	strUint = szPicUnit[nUnitID];
				//}
				if (strFileType != GetItemText(m_nRow, 3))
				{
					m_strNewType = strFileType;
				}
				//if (strUint != GetItemText(m_nRow, 4))
				//{
				//	m_strNewUit = strUint;
				//}
			}
		    break;
		default:
		    break;
		}
		
		strPath.TrimLeft();
		if (strPath != "")
		{
			m_pEdit->SetWindowText(strPath);
			m_pEdit->SetListText();
		}		
	}
	UpdateData(FALSE);
}

void CEditListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int nIndex   = 0;
    int nColNum  = 0;
	nIndex = HitTestEx(point, &nColNum);
    
    if (m_nRow != nIndex || m_nCol != nColNum)
	{
		if(m_pEdit)
		{
			if(m_pEdit->GetSafeHwnd())
			{
				m_pEdit->SetListText();
				m_pEdit->DestroyWindow();
			}

			delete m_pEdit;
			m_pEdit = NULL;
		}

		if(m_pBtn)
		{
			if(m_pBtn->GetSafeHwnd())
			{
				m_pBtn->DestroyWindow();
			}

			delete m_pBtn;
			m_pBtn = NULL;
		}
	}
	m_nRow = nIndex;
	m_nCol = nColNum;
	
	CListCtrl::OnRButtonDown(nFlags, point);
}


//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CComboxCell

CComboxCell::CComboxCell(CEditListCtrl* pCtrl, int iItem, int iSubItem, CString sInitText)
	: m_bEscape(FALSE)
{
	m_pListCtrl = pCtrl;
    m_Item = iItem;
    m_SubItem = iSubItem;
    m_InitText = sInitText;
}

CComboxCell::~CComboxCell()
{
}


BEGIN_MESSAGE_MAP(CComboxCell, CComboBox)
	//{{AFX_MSG_MAP(CComboxCell)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////
// 标    识  : KmAps 3.0 内部优化。
// 函数名称  : CComboxCell::ReSize
// 参    数  : 
//      CRect rect -- 
// 返 回 值  : 
//      void -- 
// 目的/功能 : 重设combo的区域
// 调用关系  : 
// 创 建 者  : 吴克兵
// 创建日期  : 2009-10-13
// 最新修改者:                        修改日期：
///////////////////////////////////////////////////////////////////////
void CComboxCell::ReSize(CRect rect)
{
	char* SupportType[] = {"UG", "ProE", "SolidEdge", "SolidWorks", "CATIAV5", "IGES"};
	CStringArray saItems;
	switch(m_SubItem)
	{
	case 3:
		{			
			for (int i = 0; i< 6; i++)
			{
				AddString(SupportType[i]);
				saItems.Add(SupportType[i]);
			}
		}
		break;
	case 4:
		{			
			//for (int i = 0; i< 14; i++)
			//{
			//	AddString(szPicUnit[i]);
			//	saItems.Add(szPicUnit[i]);
			//}
		}
	    break;
	default:
	    break;
	}

	// Get the maximum width of the text strings
	int nMaxLength = 0;
	CClientDC dc(GetParent());
	CFont* pOldFont = dc.SelectObject(m_pListCtrl->GetFont());

	for (int i = 0; i < saItems.GetSize(); i++) 
	{
		nMaxLength = max(nMaxLength, dc.GetTextExtent(saItems[i]).cx);
	}

	nMaxLength += (::GetSystemMetrics(SM_CXVSCROLL) + dc.GetTextExtent(_T(" ")).cx*2);
	dc.SelectObject(pOldFont);

    if (nMaxLength > rect.Width())
	{
	    rect.right = rect.left + nMaxLength;
	}

	// Resize the edit window and the drop down window
	int nHeight  = rect.Height();
	rect.bottom += (int)saItems.GetSize() * nHeight + ::GetSystemMetrics(SM_CYHSCROLL);
	MoveWindow(rect);

	SetFont(m_pListCtrl->GetFont());
	SetItemHeight(-1, nHeight);

	SetDroppedWidth(nMaxLength);
	SetHorizontalExtent(0); // no horz scrolling

	// Set the initial text to m_sInitText
	if (SelectString(-1, m_InitText) == CB_ERR) 
	{
		SetWindowText(m_InitText);		// No text selected, so restore what was there before
	}

    // Subclass the combobox edit control if style includes CBS_DROPDOWN
    SetFocus();
}

void CComboxCell::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	SetListText();	
}

void CComboxCell::SetListText()
{
	CString Text;
    GetWindowText(Text);

    // Send Notification to parent of ListView ctrl
    LV_DISPINFO dispinfo;
    dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
    dispinfo.hdr.idFrom = GetDlgCtrlID();
    dispinfo.hdr.code = LVN_ENDLABELEDIT;

    dispinfo.item.mask = LVIF_TEXT;
    dispinfo.item.iItem = m_Item;
    dispinfo.item.iSubItem = m_SubItem;
    dispinfo.item.pszText = m_bEscape ? NULL : LPTSTR((LPCTSTR) Text);
    dispinfo.item.cchTextMax = Text.GetLength();

    GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(),(LPARAM) &dispinfo);
}
