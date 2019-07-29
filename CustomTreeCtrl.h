#pragma once

#include "CommonCtrlExport.h"

enum SelectionMode
{
	SELECTION_MODE_NORMAL				= 0,
	SELECTION_MODE_SKIP_CHILD
};


class KM_EXT_COMMONCTRL CCustomTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNCREATE(CCustomTreeCtrl)

public:
	CCustomTreeCtrl();
	virtual ~CCustomTreeCtrl();

public:
	virtual BOOL SelectItem(HTREEITEM hItem);

	void SetArrowKeySelectionMode(SelectionMode mode);

protected:
	void Fire_Notify_OnSelectedChanged();

	SelectionMode m_SeletionMode;

	HTREEITEM m_hOldItem;

protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};