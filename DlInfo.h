#pragma once


// CDlInfo �Ի���

class CDlInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlInfo)

public:
	CDlInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlInfo();

// �Ի�������
	enum { IDD = IDD_WINDCHILL_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
