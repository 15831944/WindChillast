#pragma once


// CDlInfo 对话框

class CDlInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlInfo)

public:
	CDlInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlInfo();

// 对话框数据
	enum { IDD = IDD_WINDCHILL_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
