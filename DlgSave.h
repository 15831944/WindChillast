#pragma once
#include "Resource.h"
#include "DirectoryDialog.h"

// CDlgSave 对话框

class CDlgSave : public CDialog
{
	DECLARE_DYNAMIC(CDlgSave)

public:
	CDlgSave(CString Number,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSave();

// 对话框数据
	enum { IDD = IDD_WINDCHILL_SAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAwindchillButtonPdfpath();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	void SetCaption(CString Caption);
	void Setpath(CString path);
	CString m_sName;
private:
	CString m_Caption;
	CString m_sPath;

	CString m_strPath;
	CString m_loadName;
	CString partNumber;
};
