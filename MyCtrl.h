#pragma once

#include "Resource.h"

#include "BCGCBProInc.h"

// CDlgExportPDF ¶Ô»°¿ò

class CMyGridCtrl :public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(CMyGridCtrl)
public:
	CMyGridCtrl();
	virtual ~CMyGridCtrl();
	void CreateCtrl(CWnd* pParentWnd);
protected:
	void OnItemChanged(BCGPGRID_ITEM_INFO *iteminfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};
