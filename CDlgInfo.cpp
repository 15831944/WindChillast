#include "CDlgInfo.h"

IMPLEMENT_DYNAMIC(CDlgInfo, CDialog)

const int PROP_GRID = 10;

BEGIN_MESSAGE_MAP(CDlgInfo, CDialog)
	
END_MESSAGE_MAP()

void CDlgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CDlgInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitPropList();
	//£¨CMFCPropGridCtrl*)GetDlgItem(IDC_MFCPROPERTYGRID1);

	return TRUE;
}

void CDlgInfo::InitPropList()
{
	CRect rc;
	this->GetParent()->GetClientRect(&rc);
	m_list.Create(WS_CHILD|WS_VISIBLE|WS_BORDER,rc,this, PROP_GRID);
}

CDlgInfo::CDlgInfo( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInfo::IDD, pParent)
{

}


CDlgInfo::~CDlgInfo()
{
}
