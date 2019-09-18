///////////////////////////////////////////////////////////////////////
// �� �� �� : EditListCtrl.h
// ��    �� : 
// ��    ˾ : �人��Ŀ��Ϣ�����������ι�˾
// �� Ŀ �� : KMAPS3.0
// �� �� �� : ��˱�
// �������� : 2009-08-10
///////////////////////////////////////////////////////////////////////

#if !defined(_EDITLISTCTRL_H__)
#define _EDITLISTCTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrl.h : header file
//

//�Զ�����ϢIDͳһ�Ƴ�CommonMsg.h��
//#define WM_ENDEDIT	WM_USER + 100

class CEditCell;
class CComboxCell;
/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl window

struct ItemData
{
public:
	ItemData() : arrpsz( NULL ), dwData( NULL ),crText ( NULL ),crBak (NULL) {}

	LPTSTR* arrpsz;
	DWORD dwData;
	//color
	COLORREF * crText;
	COLORREF * crBak;
	

private:
	// ban copying.
	ItemData( const ItemData& );
	ItemData& operator=( const ItemData& );
};

class CEditListCtrl : public CListCtrl
{
public:
	bool IsEdit();
	void KillEdit();
	void AddResString(const CString& str);
    CEditListCtrl(CString Text = "");
    virtual ~CEditListCtrl();
    
    
    
    CEdit*  EditSubItem(int Item, int Column);
	CComboxCell* ComboSubItem(int Item, int Column);
	CComboxCell* CreateCombo(int iItem, int iSubItem, CString sInitText, CRect rect);
    int	    HitTestEx(CPoint& Point, int* Column);
    int	    InsertItemEx(int Item);
    void    Resize(int cx, int cy);

private:
	CEditCell* m_pEdit;
	CComboxCell* m_pCombo;
	CButton*   m_pBtn;
	int		   m_nRow;		// ��������
	int		   m_nCol;		// ��������	
	CString	   m_strDefaultText;
	CString    m_strNewType; // �µ��ļ�����,
	CString    m_strNewUit;  // �µ��ļ���λ
	                         // ��Ҫ���ڣ���Դ�ļ��ı�ʱ���ļ����ͺ͵�λ���Ÿı䡣

    //{{AFX_VIRTUAL(CEditListCtrl)
    //}}AFX_VIRTUAL
    
protected:
    //{{AFX_MSG(CEditListCtrl)
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBtnBrowser();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEditCell window
class CEditCell : public CEdit
{
public:
    CEditCell(CEditListCtrl* pCtrl, int iItem, int iSubItem, CString sInitText);
    virtual ~CEditCell();
    void    SetListText();
    
    //{{AFX_VIRTUAL(CEditCell)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL
    
protected:
    CEditListCtrl*	m_pListCtrl;
    int				m_Item;
    int				m_SubItem;
    CString			m_InitText;
    BOOL			m_bEscape;
    
    //{{AFX_MSG(CEditCell)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnNcDestroy();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()
};

class CComboxCell : public CComboBox
{
// Construction
public:
	CComboxCell(CEditListCtrl* pCtrl, int iItem, int iSubItem, CString sInitText);

// Attributes
public:
	void SetListText();
	void ReSize(CRect rect);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboxCell)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboxCell();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboxCell)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
    CEditListCtrl*	m_pListCtrl;
    int				m_Item;
    int				m_SubItem;
    CString			m_InitText;
    BOOL			m_bEscape;
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_EDITLISTCTRL_H__)
