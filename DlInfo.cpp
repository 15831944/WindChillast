// DlInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WindChill.h"
#include "DlInfo.h"
#include "afxdialogex.h"


// CDlInfo �Ի���

IMPLEMENT_DYNAMIC(CDlInfo, CDialog)

CDlInfo::CDlInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlInfo::IDD, pParent)
{

}

CDlInfo::~CDlInfo()
{
}

void CDlInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlInfo, CDialog)
END_MESSAGE_MAP()


// CDlInfo ��Ϣ�������
