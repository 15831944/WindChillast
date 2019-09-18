#pragma once

#include <afxwin.h>
#include "Resource.h"
#include "PDFData.h"

#include "PDFConfig.h"


#include "BCGPPropList.h"

// CDlgInfo �Ի���

class CDlgInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgInfo)

public:
	CDlgInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgInfo();

// �Ի�������

	enum { IDD = IDD_WINDCHILL_INFO };
	void InitPage(ConfigPage *config, std::vector<PageData*> data);

	virtual BOOL OnInitDialog();

	BOOL OnDestroy();
	void InitGridCtrl();

	afx_msg LRESULT OnProChanged(WPARAM uProListId, LPARAM pProp);

	DECLARE_MESSAGE_MAP()

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
private:
	ConfigPage *m_config;
	std::vector<PageData*> m_data;

public:
	CBCGPPropList m_BCGPropList;
};
