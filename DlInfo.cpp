// DlInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "WindChill.h"
#include "DlInfo.h"
#include "afxdialogex.h"


// CDlInfo 对话框

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


// CDlInfo 消息处理程序
