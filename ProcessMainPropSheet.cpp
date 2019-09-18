#include "stdafx.h"

#include "ProcessMainPropSheet.h"
#include "apmanager.h"
#include "CommonMsg.h"
//#include "SymbolDlg.h"

#include "APManager.h"
#include "AsmStep.h"
#include "apsprocess.h"
//#include "KmResSelectDlg.h"
#include "LeaderAction.h"
#include "APSModel.h"
#include "KmResViewProxy.h"
#include "AddUseProp.h"
#include "ResourceString.h"
#include "..\ResourceDef\APM_Res.h"
#include "../HistoryOp/HistoryOpMgr.h"
#include "HistoryAsmAction.h"
#include "APSModel.h"
#include "KmMessageBox.h"
#include "PropListCtrl.h"
#include "EasySize.h"


IMPLEMENT_DYNAMIC(CProcessMainPropSheet, CKmPropertySheet)

CProcessMainPropSheet::CProcessMainPropSheet(CWnd* pParentWnd, UINT iSelectPage)
	:CKmPropertySheet(IDS_APM_ACTION_CAPTION, pParentWnd, iSelectPage)
{
	m_pAPManager = NULL;
	m_pBasicPage = new CProcessBasicPage;
	m_pCustomPage = new CProcessCustomPage;
// 	m_pAcessPage = new CProcessAccessoryPage;
// 	m_pResourcePage = new CProcessResourcePage;

	this->AddPage(m_pBasicPage);
	this->AddPage(m_pCustomPage);
// 	this->AddPage(m_pResourcePage);
// 	this->AddPage(m_pAcessPage);
	
}

CProcessMainPropSheet::~CProcessMainPropSheet()
{
	if (m_pBasicPage)
	{
		if (IsWindow(m_pBasicPage->GetSafeHwnd()))
		{
			m_pBasicPage->DestroyWindow();
		}

		delete m_pBasicPage;
		m_pBasicPage = NULL;
	}

	if (m_pCustomPage)
	{
		if (IsWindow(m_pCustomPage->GetSafeHwnd()))
		{
			m_pCustomPage->DestroyWindow();
		}

		delete m_pCustomPage;
		m_pCustomPage = NULL;
	}

	for (int i = 0;i<m_ProcessGridPageAry.GetSize();i++)
	{
		if(m_ProcessGridPageAry[i])
		{
			delete m_ProcessGridPageAry[i];
			m_ProcessGridPageAry[i] = NULL;
		}
	}
	m_ProcessGridPageAry.RemoveAll();
}


BEGIN_MESSAGE_MAP(CProcessMainPropSheet, CKmPropertySheet)
	ON_MESSAGE(MSG_APS_UPDATE_PROP , OnUpdate)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CProcessBasicPage)
	EASYSIZE(IDC_APM_STATIC_ASMSTEP_NAME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_APM_EDIT_ASMSTEP_NAME, ES_BORDER, IDC_APM_EDIT_ASMSTEP_NAME, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_APM_STATIC_ASMSTEP_CONTENT, ES_BORDER, IDC_APM_EDIT_ASMSTEP_NAME, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_APM_EDIT_ASMSTEP_CONTENT, ES_BORDER, IDC_APM_EDIT_ASMSTEP_NAME, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_APM_RES, ES_BORDER, IDC_APM_EDIT_ASMSTEP_CONTENT, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_APM_BUTTON_GRAP_TEXT, ES_BORDER, IDC_APM_EDIT_ASMSTEP_CONTENT, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_APM_SAVE, ES_BORDER, IDC_APM_EDIT_ASMSTEP_CONTENT, ES_KEEPSIZE, ES_KEEPSIZE, 0)
END_EASYSIZE_MAP

/////////////////////////////////////////////////////////////////////////////
// CProcessMainPropSheet message handlers
BOOL CProcessMainPropSheet::OnInitDialog()
{
	CKmPropertySheet::OnInitDialog();

	// ����������ҳ������
	//this->SetActivePage(m_pResourcePage);
	//this->SetActivePage(m_pAcessPage);
	this->SetActivePage(m_pCustomPage);
	this->SetActivePage(m_pBasicPage);
	
	
	return TRUE;
}

void CProcessMainPropSheet::OnShowWindow(BOOL bShow, UINT nStatus)
{
	try
	{
		if(!bShow && IsWindow(m_hWnd))
		{
			if(m_pCustomPage) m_pCustomPage->Update(NULL);
			if(m_pBasicPage) m_pBasicPage->Update(NULL);

			//�����Ƕ�ȡAPM��ÿ������洢������
			for(int i=0;i<m_ProcessGridPageAry.GetCount();i++)
			{
				CProcessGridPage* pGridPage = m_ProcessGridPageAry.GetAt(i);
				pGridPage->OnShowWindow(bShow, nStatus);
				pGridPage->Update(NULL);
			}
		}
	}
	catch (...)
	{

	}
	
	CKmPropertySheet::OnShowWindow(bShow, nStatus);
}
CGridPage* CProcessMainPropSheet::GetPageByName(CString strName)
{
	for(int i=0;i<m_ProcessGridPageAry.GetCount();i++)
	{
		CProcessGridPage* pGridPage = m_ProcessGridPageAry.GetAt(i);
		if(pGridPage->GetGridName() == strName)
		{
			return pGridPage;
		}
	}
	return NULL;
}


BOOL CProcessMainPropSheet::OnEraseBkgnd(CDC * pDC)
{
	if(pDC)
	{
		CRect rc;
		GetClientRect(&rc);
		pDC->FillSolidRect(&rc, RGB(255, 255, 255)/*GetSysColor(COLOR_WINDOWFRAME)*/);
	}
	return TRUE;
}


void CProcessMainPropSheet::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CKmPropertySheet::OnWindowPosChanged(lpwndpos);	

	CRect rc;
	CTabCtrl* pTabCtrl = this->GetTabControl();

	
	if (pTabCtrl && pTabCtrl->GetSafeHwnd())
	{
		// ����TabCtrl��λ��
		rc.left = 2;
		rc.right = rc.left + lpwndpos->cx - 4;
		rc.top = 2;
		rc.bottom = rc.top + lpwndpos->cy - 4;
		pTabCtrl->MoveWindow(&rc);
		pTabCtrl->RedrawWindow();

		// ѡ��ĸ߶�
		CRect itemRc;
		pTabCtrl->GetItemRect(0, itemRc);
		int tabRow = pTabCtrl->GetRowCount();
		int tabHeight = itemRc.Height() * tabRow;

		// ����PropertyPage��λ��
		//rc.InflateRect(-2, -18, -2, -2);
		rc.InflateRect(-2, -tabHeight - 2, -2, -2);
		int count = this->GetPageCount();
		for (int i = 0; i < count; i ++)
		{
			CPropertyPage* pPropertyPage = this->GetPage(i);
			if (pPropertyPage->GetSafeHwnd())
			{
				pPropertyPage->MoveWindow(&rc);
			}
		}

		//pTabCtrl->Invalidate();
	}
	
	//::SetWindowPos(GetSafeHwnd(), NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
}

void CProcessMainPropSheet::SetAPManager(CAPManager* pManager)
{
	m_pAPManager = pManager;
	m_pBasicPage->SetAPManager(pManager);
	m_pCustomPage->SetAPManager(m_pAPManager);
	for(int i=0;i<m_ProcessGridPageAry.GetCount();i++)
	{
		CProcessGridPage* pGridPage = m_ProcessGridPageAry.GetAt(i);
		pGridPage->SetAPManager(pManager);
	}
}

CKmPropertyPage * CProcessMainPropSheet::GetPageByProp(CString strPageName)
{
	
	if(strPageName == Prop_Base)
	{
		return m_pBasicPage;
	}
	else if(strPageName == Prop_User)
	{
		return m_pCustomPage;
	}
	//�������������Ҫ���	2014.07.11 modify by cx
	else
	{
		for(int i=0;i<m_ProcessGridPageAry.GetCount();i++)
		{
			CProcessGridPage* pGridPage = m_ProcessGridPageAry.GetAt(i);
			if(pGridPage->GetGridName() == strPageName)
			{
				return pGridPage;
			}
		}
	}
	return NULL;
}

void CProcessMainPropSheet::Update(CProcess * pProcess)
{
	
	if(NULL == pProcess && m_hWnd!=NULL)
	{
		ShowWindow(SW_HIDE);
	}

	if(pProcess==NULL)
	{
		return;
	}

	CString strPrePageName = CProcess::GetActivePropPageName();

	//ShowWindow(SW_SHOW);
	//UpdatePropWndVisibility(ACT_PROCESS);

	if (IsPageVisible(m_pBasicPage)) 
	{
		m_pBasicPage->SetAPManager(m_pAPManager);
		m_pBasicPage->Update(pProcess);
	}

	if (IsPageVisible(m_pCustomPage))
	{
		m_pCustomPage->SetAPManager(m_pAPManager);
		m_pCustomPage->Update(pProcess);
	}

	//show page 2014.08.07 add by cx 
	int count = GetPageCount();
	int nAllPage = GetPageCount();
	for(int i=nAllPage-1;i>=0;i--)
	{
		CPropertyPage* pTmp = NULL;
		pTmp = GetPage(i);

		for(int j=0;j<m_ProcessGridPageAry.GetSize();j++)
		{
			if(pTmp == m_ProcessGridPageAry.GetAt(j))
			{
				RemovePage(i);
				break;
			}
		}
	}

	for (int i = 0;i<m_ProcessGridPageAry.GetSize();i++)
	{
		if(m_ProcessGridPageAry[i])
		{
			delete m_ProcessGridPageAry[i];
			m_ProcessGridPageAry[i] = NULL;
		}
	}
	m_ProcessGridPageAry.RemoveAll();

	//int nActive = GetActiveIndex();

	CArray<CAPSGridPropData*>* plistdataary = pProcess->GetProcessGridDatas();
    if(m_pAPManager->GetModel()->GetOpenFileType() == 0 || plistdataary != NULL)
	{	
		for(int i=0;i<plistdataary->GetCount();i++)
		{
			CAPSGridPropData* pPropData = plistdataary->GetAt(i);
			CString strName = pPropData->GetGridName();

			CProcessGridPage* pGridPage = new CProcessGridPage();
			pGridPage->SetTitle(strName);
			pGridPage->SetVis(pPropData->GetBVis());
			AddPage(pGridPage);
			SetActivePage(pGridPage);
			pGridPage->SetWindowText(strName);

			if(IsPageVisible(pGridPage))
			{
				pGridPage->SetAPManager(m_pAPManager);
				pGridPage->SetGridName(strName); 
				pGridPage->Update(pProcess);
			}
			m_ProcessGridPageAry.Add(pGridPage);
		}
	}
	else
	{
		//�������ݶ������ļ� modify by cx 2015.02.05
		for(int i=0;i<CProcess::s_processGridPropAry.GetCount();i++)
		{
			BOOL bHave = FALSE;
			CString strs_Nmae = CProcess::s_processGridPropAry.GetAt(i).GetGridName();
			for(int j=0;j<m_ProcessGridPageAry.GetCount();j++)
			{
				CString str_dataName = m_ProcessGridPageAry.GetAt(j)->GetGridName();
				if(strs_Nmae == str_dataName)
				{
					bHave = TRUE;
					break;
				}
			}
			if(!bHave)
			{
				CAPSPropList tableHead;
				CProcess::GetProcessGridHead(strs_Nmae, tableHead, NULL);

				CArray<CAPSPropList,CAPSPropList> data;
				pProcess->SetCurGridData(strs_Nmae, &data);

				CAPSGridPropData* pGrid = pProcess->GetCurGrid(strs_Nmae);
				if (pGrid)
				{
					pGrid->SetBVis(FALSE);
					pGrid->SetTableHeadList(tableHead);
				}

				CProcessGridPage* pGridPage = new CProcessGridPage();
				pGridPage->SetTitle(strs_Nmae);
				pGridPage->SetGridName(strs_Nmae);
				pGridPage->SetVis(FALSE);
				AddPage(pGridPage);
				SetActivePage(pGridPage);

				pGridPage->SetAPManager(m_pAPManager);
				pGridPage->SetGridName(strs_Nmae); 	
				pGridPage->Update(pProcess);

				m_ProcessGridPageAry.Add(pGridPage);
			}
		}
	}


	SetCurProcess(pProcess);
	UpdatePropWndVisibility(ACT_PROCESS);

	if (!m_pBasicPage->IsVisPage())
	{
		ShowPage(m_pBasicPage, false);
	}

	//SetActivePage(nActive);
	int iActivePage = CalPageByPreName(strPrePageName);
	//CPropertyPage *pPage = GetPage(iActivePage);
	//pPage->ShowWindow(SW_SHOW);
	SetActivePage(iActivePage);
}

LRESULT CProcessMainPropSheet::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	m_pAPManager = (CAPManager*)lParam;
	if (!m_pAPManager)
	{
		return 0;
	}

	//AFX_MODULE_STATE* pState = NULL;
	//if(m_pAPManager->m_iLaunchMode == EMBED_MODE)
	//{				
	//	pState = AfxGetStaticModuleState();
	//}
	//else
	//{
	//	pState = AfxGetAppModuleState();
	//}

	//AFX_MANAGE_STATE(pState);

	Update((CProcess *)wParam);

	return 0;
}





// CProcessBasicPage �Ի���
IMPLEMENT_DYNAMIC(CProcessBasicPage, CKmPropertyPage)

CProcessBasicPage::CProcessBasicPage()
: CKmPropertyPage(CProcessBasicPage::IDD, IDS_APM_GROUP_BASIC_CAPTION)
{
	m_pManager = NULL;
	m_pProcess = NULL;
	m_bVis = true;
}

CProcessBasicPage::~CProcessBasicPage()
{
}

void CProcessBasicPage::DoDataExchange(CDataExchange* pDX)
{
	CKmPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_APM_EDIT_ASMSTEP_NAME, m_editName);
	DDX_Control(pDX, IDC_APM_EDIT_ASMSTEP_CONTENT, m_editContent);
}


BEGIN_MESSAGE_MAP(CProcessBasicPage, CKmPropertyPage)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_COPYDATA()
	ON_WM_SIZE()
	//ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_APM_SPECIAL_SYMBOL, &CProcessBasicPage::OnClickSpecialSymbolBtn)
	ON_BN_CLICKED(IDC_APM_RES, &CProcessBasicPage::OnClickKmResBase)
	ON_BN_CLICKED(IDC_APM_BUTTON_GRAP_TEXT, &CProcessBasicPage::OnClickKmGrapText)
	ON_BN_CLICKED(IDC_APM_BUTTON_MODIFLY, &CProcessBasicPage::OnBnClickedButtonModifly)
	ON_EN_KILLFOCUS(IDC_APM_EDIT_ASMSTEP_NAME, &CProcessBasicPage::OnKillFocusAsmStepName)
	ON_EN_KILLFOCUS(IDC_APM_EDIT_ASMSTEP_CONTENT, &CProcessBasicPage::OnKillFocusAsmStepContent)

	ON_EN_CHANGE(IDC_APM_EDIT_ASMSTEP_NAME, &CProcessBasicPage::OnEnChangeEditAsmstepName)
	ON_EN_CHANGE(IDC_APM_EDIT_ASMSTEP_CONTENT, &CProcessBasicPage::OnEnChangeEditAsmstepContent)
	ON_BN_CLICKED(IDC_APM_SAVE, &CProcessBasicPage::OnBnClickedSave)
	ON_BN_CLICKED(IDC_APM_CHECK_EXPORT, &CProcessBasicPage::OnBnClickedCheckExport)
END_MESSAGE_MAP()


// CGroupBasicPage ��Ϣ�������
/*
BOOL CProcessBasicPage::OnInitDialog() 
{
	CKmPropertyPage::OnInitDialog();

	ReadProp();

	UpdateData(FALSE);

	return TRUE;
}
*/

void CProcessBasicPage::OnKillFocusProcessName()
{
	if (m_pManager == NULL || m_pProcess == NULL)
	{
		return;
	}

	if (m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	CString strName;
	m_editName.GetWindowText(strName);
	strName.Trim();

	WriteProp();
}

void CProcessBasicPage::Update(CProcess * pProcess)
{
	m_pProcess = pProcess;

	if (!IsWindow(GetSafeHwnd()))
	{
		return;
	}

	//m_editName.SetWindowText(_T(""));

	if (m_pManager && m_pProcess)
	{
		ReadProp();
	}
}

BOOL CProcessBasicPage::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CString strTemp((LPCSTR)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
	if(!strTemp.IsEmpty())
	{
		CString ss;
		m_editContent.GetWindowText(ss);

		m_editContent.SetFocus();	// Ϊ���뿪�˿ؼ�ʱ���Զ�����
		m_editContent.ReplaceSel(strTemp);

		m_pManager->GetModel()->GetView()->SetFocus();
	}
	return TRUE;
}


BOOL CProcessBasicPage::OnInitDialog() 
{
	CKmPropertyPage::OnInitDialog();

	UpdateData(FALSE);

	if (m_pManager && m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		GetDlgItem(IDC_APM_SAVE)->EnableWindow(FALSE);
		m_editName.SetReadOnly(TRUE);//���Ʋ����޸�
	}
	else
	{
		m_editName.SetReadOnly(false);
	}


// 	if(m_pProcess)
// 	{
// 	}

	//begin_add in cq in 2014.03 for Ӣ�İ�_��֧�ֹ��չ�����
	if(KmAppSettings::GetAppLanguage() != APS_LANG_CHS&& GetDlgItem(IDC_APM_RES))
	{
		GetDlgItem(IDC_APM_RES)->ShowWindow(SW_HIDE);
	}
	//end_add

	INIT_EASYSIZE;
	return TRUE;
}

void CProcessBasicPage::OnClickKmGrapText()
{
	PropFillToolData data;
	data._hPropWnd = m_hWnd;
	m_pManager->Fire_OnSetActiveTool(L"CAPSToolGrapText",(VOID_PTR)&data);
}

void CProcessBasicPage::OnClickSpecialSymbolBtn() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CProcessBasicPage::OnClickKmResBase()
{
	KmResViewProxy kmResView;
	//if(m_pManager->m_pProcess->IsUseStdProcedureName())
		//if(CProcess::s_bUseStdProcedureName)
	{
		kmResView.SetParentWnd(m_hWnd);
		kmResView.SetRootNodeName(CProcess::GetGXContentNodePath(ACT_PROCESS));
		kmResView.SetGXNameAndValue(CProcess::GetGXFieldName(ACT_PROCESS), m_pProcess->GetName());
	}

	if (kmResView.DoModal())
	{
		CString val = kmResView.GetValue();
		if (!m_pManager || !m_pManager->ParseParaRes(m_pProcess, NULL, val, val))
		{
			return;
		}
		if (!val.IsEmpty())
		{
			m_editContent.SetFocus();	// Ϊ���뿪�˿ؼ�ʱ���Զ�����
			m_editContent.ReplaceSel(val);
		}
	}
}

void CProcessBasicPage::OnKillFocusAsmStepName()
{
	if(!m_bDirtyName) return ;

	if (m_pManager && m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	WriteProp();

	m_bDirtyName = false;
}

void CProcessBasicPage::OnKillFocusAsmStepContent()
{
	if(!m_bDirtyContent)
		return;

	if (m_pManager && m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	WriteProp();

	m_bDirtyContent = false;
}

void CProcessBasicPage::ReadProp(void)
{
	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	CAPSProp propName;
	CAPSProp propContent;

	if(1)
	{
		if (m_pManager->m_iLaunchMode == EMBED_MODE)
		{
			m_editName.SetReadOnly();
		}
		else
		{		
			m_editName.SetReadOnly(false);
		}
		CString sVal = m_pProcess->GetName();
		m_editName.SetWindowText(m_pProcess->GetName());
		m_editName.ShowWindow(SW_SHOW);
		CWnd *pWnd = GetDlgItem(IDC_APM_STATIC_ASMSTEP_NAME);
		if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		m_editName.EnableWindow(TRUE);
		if (m_pProcess->m_baseList.SeekProp(APSPROP_ASM_NAME, &propName))
		{
			if (!propName.GetVisInProplistWnd())
			{
				m_editName.ShowWindow(SW_HIDE);
				if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
				{
					pWnd->ShowWindow(SW_HIDE);
				}
			}
			if (!propName.GetEdit())
			{
				m_editName.EnableWindow(FALSE);
			}
		}
	}

	//����
	if(1)
	{
		m_editContent.SetReadOnly(m_pManager->m_iLaunchMode == EMBED_MODE ? TRUE : FALSE);

		/*
		CAPSProp prop;
		POSITION findPos = m_pProcess->m_baseList.SeekProp(APSPROP_ASM_CONTENT, &prop);

		CString strContent = _T("");
		if(findPos)
		{
			strContent = prop.ConvertTo();
			if(strContent != "" && strContent.Find("\r\n") == -1 && strContent.Find("\n") > -1)
			{
				strContent.Replace("\n", "\r\n");
			}
		}
		*/
		CString strContent = _T("");
		
		m_pProcess->GetContent(strContent);
		
		if(!strContent.IsEmpty() && strContent.Find(_T("\r\n")) == -1 && strContent.Find(_T("\n")) > -1)
		{
			strContent.Replace(_T("\n"), _T("\r\n"));
		}
		m_editContent.SetWindowText(strContent);
		m_editContent.ShowWindow(SW_SHOW);
		m_editContent.EnableWindow(TRUE);
		CWnd *pWnd = GetDlgItem(IDC_APM_STATIC_ASMSTEP_CONTENT);
		if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
		{
			pWnd->ShowWindow(SW_SHOW);
		}
		CWnd *pRes = GetDlgItem(IDC_APM_RES);
		if (pRes != NULL && IsWindow(pRes->GetSafeHwnd()))
		{
			pRes->EnableWindow(TRUE);
		}
		CWnd *pGrap = GetDlgItem(IDC_APM_BUTTON_GRAP_TEXT);
		if (pGrap != NULL && IsWindow(pGrap->GetSafeHwnd()))
		{
			pGrap->EnableWindow(TRUE);
		}

		if (m_pProcess->m_baseList.SeekProp(APSPROP_ASM_CONTENT, &propContent))
		{
			if (!propContent.GetVisInProplistWnd())
			{
				m_editContent.ShowWindow(SW_HIDE);
				if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
				{
					pWnd->ShowWindow(SW_HIDE);
				}
			}
			if (!propContent.GetEdit())
			{
				m_editContent.EnableWindow(FALSE);
				if (pRes != NULL && IsWindow(pRes->GetSafeHwnd()))
				{
					pRes->EnableWindow(FALSE);
				}
				if (pGrap != NULL && IsWindow(pGrap->GetSafeHwnd()))
				{
					pGrap->EnableWindow(FALSE);
				}

			}
		}
	} 

	if (!propName.GetVisInProplistWnd() && !propContent.GetVisInProplistWnd())
	{
		m_bVis = false;
	}
	else
	{
		m_bVis = true;
	}
}

void CProcessBasicPage::WriteProp(void)
{
	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	// Ƕ��ؼ�ģʽ�£�������д��
	if (m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	//HistoryOpMgr * pHistoryMgr = m_pManager->GetModel()->GetHistoryMgr();

	try
	{
		CString     strName(_T("")); 
		CString     strContent(_T(""));
		CAPSProp    propName;
		CAPSProp    propContent;

		CAPSModelHistorySmartObject CmdObject(m_pManager->GetModel());

		m_editName.GetWindowText(strName);
		// KmAps 210 Bug10471 �޸Ļ�ڵ����ƺ󣬵�ǰ��ڵ�����Ƹ������������еĻ���Ʋ�����������һ�¡� Modify by ��˱� 2009-11-16
		CString strPreName = m_pProcess->GetName();
		
		//��ͬ
		if(strPreName != strName)
		{
			//��ӻ�������
			HistoryModifyData<CString> * pData = new HistoryModifyData<CString>(HISTORY_ACTION_MODIFYNAME);
			pData->m_data = strPreName;

			m_pProcess->SetName(strName);

			HistoryProcessModifyBase * pCmd = new HistoryProcessModifyBase(m_pProcess, pData);
			m_pManager->GetModel()->AddCmd(pCmd);

			m_pManager->RefreshProcessItem(m_pProcess);
		}
		else
		{//fix 4943
			m_pProcess->SetName(strPreName);
		}
		
		m_editContent.GetWindowText(strContent);
		
// 		CAPSProp prop;
// 		prop.ModifyByType(strContent);
// 		prop.SetName(_T("����"));
// 		m_pProcess->m_baseList.ModifyProp(_T("����"), prop);

		CString strC;
		m_pProcess->GetContent(strC);
		if(m_bDirtyContent && strC.Compare(strContent) != 0)
		{
			//��ӻ�������
			HistoryModifyData<CString> * pData = new HistoryModifyData<CString>(HISTORY_ACTION_MODIFYCONTENT);
			m_pProcess->GetContent(pData->m_data);

			m_pProcess->SetContent(strContent);

			HistoryProcessModifyBase * pCmd = new HistoryProcessModifyBase(m_pProcess, pData);
			m_pManager->GetModel()->AddCmd(pCmd);
		}
		/*
		propName.SetName(APSPROP_ASM_CONTENT);
		set_strvalue( &propName, (LPCTSTR)strContent );
		m_pProcess->m_baseList.ModifyProp(propName.GetName(), propName);
		*/
		CmdObject.Detach();
	}
	catch (...) 
	{
		ASSERT(FALSE);
	}
}

void CProcessBasicPage::SetAPManager(CAPManager* pManager)
{
	m_pManager = pManager;

	if (pManager && pManager->m_iLaunchMode == EMBED_MODE && m_hWnd != NULL)
	{
		m_editContent.SetReadOnly(TRUE);
		m_editName.SetReadOnly(TRUE);
	}
}

BOOL CProcessBasicPage::OnEraseBkgnd(CDC * pDC)
{
	if(pDC)
	{
		CRect rc;
		GetClientRect(&rc);
		pDC->FillSolidRect(&rc, RGB(255, 255, 255)/*GetSysColor(COLOR_WINDOWFRAME)*/);
	}
	return TRUE;
}

void CProcessBasicPage::OnSize(UINT nType, int cx, int cy) 
{
	CKmPropertyPage::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

	auto pWhd = GetDlgItem(IDC_APM_EDIT_ASMSTEP_CONTENT);
	if (pWhd)
	{
		CRect rectContent;
		pWhd->GetWindowRect(&rectContent);
		ScreenToClient(&rectContent);
		if (rectContent.Height() <= 50)
		{
			rectContent.bottom = rectContent.top + 60;
			pWhd->MoveWindow(rectContent);

			pWhd = GetDlgItem(IDC_APM_CHECKTYPE_STATIC);
			if (pWhd)
			{
				CRect rect;
				pWhd->GetWindowRect(&rect);
				ScreenToClient(&rect);
				auto height = rect.Height();
				rect.top = rectContent.bottom + 10;
				rect.bottom = rect.top + height;
				pWhd->MoveWindow(rect);
			}

			pWhd = GetDlgItem(IDC_APM_TESTTYPEEDIT);
			if (pWhd)
			{
				CRect rect;
				pWhd->GetWindowRect(&rect);
				ScreenToClient(&rect);
				auto height = rect.Height();
				rect.top = rectContent.bottom + 10;
				rect.bottom = rect.top + height;
				pWhd->MoveWindow(rect);
			}

			pWhd = GetDlgItem(IDC_APM_RES);
			if (pWhd)
			{
				CRect rect;
				pWhd->MoveWindow(rect);
			}
			pWhd = GetDlgItem(IDC_APM_BUTTON_GRAP_TEXT);
			if (pWhd)
			{
				CRect rect;
				pWhd->MoveWindow(rect);
			}

			pWhd = GetDlgItem(IDC_APM_SAVE);
			if (pWhd)
			{
				CRect rect;
				pWhd->MoveWindow(rect);
			}
		}
	}

}

void CProcessBasicPage::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CKmPropertyPage::OnWindowPosChanged(lpwndpos);
	//del begin by xjt for ����ҳ������
	/*
	if (IsWindow(GetSafeHwnd()))
	{
		CButton* pModifyNameBtn = (CButton*)GetDlgItem(IDC_APM_BUTTON_MODIFLY);
		//CButton* pSpecialSymbolBtn = (CButton*)GetDlgItem(IDC_APM_SPECIAL_SYMBOL);
		CButton* pResBtn = (CButton*)GetDlgItem(IDC_APM_RES);
		CButton* pGrapTextBtn = (CButton*)GetDlgItem(IDC_APM_BUTTON_GRAP_TEXT);
		CButton* pSaveBtn = (CButton*)GetDlgItem(IDC_APM_SAVE);

		CRect rc;
		GetClientRect(&rc);

		int iWidth = rc.Width();
		int iHeight = rc.Height();

		CRect contentRc;
		m_editContent.GetWindowRect(&contentRc);
		ScreenToClient(&contentRc);

		m_editContent.MoveWindow(contentRc.left, contentRc.top, rc.Width() - 75, rc.Height() - 100);

		//if(pSpecialSymbolBtn) pSpecialSymbolBtn->MoveWindow(40, rc.bottom-30, 60, 20);

		int dx = (int)iWidth*0.05;
		if (dx>10)
		{
			dx = 10;
		}
		int i1 = 0;
		int i2 = 0;
		CRect rcResBtn;
		
		if(pResBtn) 
		{
			pResBtn->GetWindowRect(rcResBtn);
			pResBtn->MoveWindow(dx, rc.bottom-30, rcResBtn.Width(), rcResBtn.Height());

			if (!pResBtn->IsWindowVisible())
			{
				i1 = iWidth*0.3;
			}
			else
			{
				i1 = dx + rcResBtn.Width()+5;
			}
		}

		CRect rcGrapTextBtn;
		if(pGrapTextBtn)
		{
			pGrapTextBtn->GetWindowRect(rcGrapTextBtn);
			pGrapTextBtn->MoveWindow(i1, rc.bottom-30, rcGrapTextBtn.Width(), rcGrapTextBtn.Height());
			i2 = i1 + rcGrapTextBtn.Width()+5;
		}

		CRect rcSaveBtn;
		if(pSaveBtn)
		{
			pSaveBtn->GetWindowRect(rcSaveBtn);
			pSaveBtn->MoveWindow(i2, rc.bottom-30, rcSaveBtn.Width(), rcSaveBtn.Height());
		}
	}
*/
//del end by xjt for ����ҳ������
}

void CProcessBasicPage::OnBnClickedButtonModifly()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	KmResViewProxy kmResView;
	kmResView.SetParentWnd(m_hWnd);
	kmResView.SetRootNodeName(CProcess::GetResNodePath());
	if (kmResView.DoModal())
	{
		CString strVal = kmResView.GetValue();
		if(!strVal.IsEmpty())
		{
			SetDlgItemText(IDC_APM_EDIT_ASMSTEP_NAME, strVal);
			m_bDirtyContent = true;
			OnKillFocusAsmStepContent();
		}
	}
}

void CProcessBasicPage::OnEnChangeEditAsmstepName()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CKmPropertyPage::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_bDirtyName = true;
}

void CProcessBasicPage::OnEnChangeEditAsmstepContent()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CKmPropertyPage::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_bDirtyContent = true;
}

void CProcessBasicPage::OnBnClickedSave()
{
	WriteProp();

	//fix bug 4641
	CString strFileName = m_pManager->GetModel()->GetOpenedFileName();
	m_pManager->GetModel()->Save(strFileName, 0, SERIAL_NORMAL);

	m_bDirtyContent = false;
	m_bDirtyName = false;
}

void CProcessBasicPage::OnBnClickedCheckExport()
{
}

bool CProcessBasicPage::IsVisPage()
{
	return m_bVis;
}

/////////////////////////////////////////////////////////////////////////////
// CProcessCustomPage property page
using namespace KmAPSTreeListCtrl;
IMPLEMENT_DYNCREATE(CProcessCustomPage, CKmPropertyPage)


CProcessCustomPage::CProcessCustomPage() : CKmPropertyPage(CProcessCustomPage::IDD, IDS_APM_STEP_CUSTOM_CAPTION)
{
	m_pManager = NULL;
	m_pProcess = NULL;

	//m_strUsePropName = _T("");
	m_nRowSel = -1;
	m_bDoModel = false;
	m_bModified = false;


	m_nItemIndex = -1;
	m_nSubItemIndex = -1;

	//m_listDefine.SetStepCustomPag(this);
	m_pPropListCtrl = NULL;
}

CProcessCustomPage::~CProcessCustomPage()
{
	if (m_pPropListCtrl != NULL)
	{
		delete m_pPropListCtrl;
		m_pPropListCtrl = NULL;
	}
}

void CProcessCustomPage::DoDataExchange(CDataExchange* pDX)
{
	CKmPropertyPage::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_APM_LIST_ASMSTEP_DEFINE, m_listDefine);	
}

BEGIN_MESSAGE_MAP(CProcessCustomPage, CKmPropertyPage)
	//{{AFX_MSG_MAP(CProcessCustomPage)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ACTIVATE()
	ON_WM_KILLFOCUS()
	ON_COMMAND(IDM_APM_ADD_USEPROP, OnAddUseprop)
	ON_COMMAND(IDM_APM_DELETE_USEPROP, OnDeleteUseprop)
	ON_COMMAND(IDM_APM_UPDATE_USEPROP, OnUpdateUseprop)
	
	ON_EN_KILLFOCUS(IDC_APM_EDIT_MUTILEVALUE, OnKillFocusEditMultiValue)
	ON_LBN_KILLFOCUS(IDC_APM_LIST_MUTILEVALUE, OnKillFocusListMultiValue)

	//ON_NOTIFY(NM_KILLFOCUS, IDC_APM_LIST_ASMSTEP_DEFINE, OnKillFocusCustomList)
	ON_NOTIFY(NM_KILLFOCUS, IDC_APM_PROCESSCUSTORM_LIST, OnKillFocusCustomList)

	//ON_NOTIFY(NM_RCLICK, IDC_APM_LIST_ASMSTEP_DEFINE, OnRclickListAsmstepDefine)

	ON_NOTIFY(LVN_KEYDOWN, IDC_APM_LIST_MUTILEVALUE,OnKeyDownInMutileValue)
	ON_NOTIFY(NM_RCLICK, IDC_APM_LIST_MUTILEVALUE, OnRclickListMutileValue)

	ON_BN_CLICKED(IDC_APM_CUSTOM_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_APM_BUTTON_MUTILEVALUE, OnBnClickedButtonMutileValue)
	ON_BN_CLICKED(IDC_APM_BUTTON_DEL_MUTILE_VALUE, OnBnClickedButtonMutileValueDel)

	//ON_MESSAGE(WM_ENDEDITLABEL, OnEndLabelEdit)
	//ON_MESSAGE(WM_BEGINEDITLABEL, OnBeginLabelEdit)
	ON_BN_CLICKED(IDC_APM_PROJECT_SYMBOL_C, OnBnClickedButtonC)
	ON_BN_CLICKED(IDC_APM_PROJECT_SYMBOL_G, OnBnClickedButtonG)

	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, OnBCGGridSelChanged)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ITEM_ENDINPLACEEDIT, OnEndLabelEdit)
	ON_MESSAGE(BCGM_GRID_RBUTTONUP, OnBCGGridRButtonUp)
	//}}AFX_MSG_MAP
//	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessCustomPage message handlers
void CProcessCustomPage::OnBnClickedButtonMutileValue()
{
	CString str;
	GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->GetWindowText(str);
	if(!str.IsEmpty() && m_nRowSel>=0)
	{
		((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->AddString(str);
		GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->SetWindowText(_T(""));

		WriteProp(m_nRowSel);

		CAPSProp prop;
		//m_pAsmStep->SeekObjProp(m_strUsePropName,prop);
		m_useList.SeekProp(m_strUsePropName, &prop);

		CString strAll = prop.ConvertTo();
		//m_pTreeList->SetItemText(m_nRowSel,1,strAll);
		
		CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(m_nRowSel);
		CBCGPGridItem *pItem = pRow->GetItem(1); //����ֵ
		str = prop.ConvertDefaultTo();
		int is = str.Find('|');
		int ie = str.Find(';');
		if (is != -1 && ie > is)
		{
			is = is + 1;
			while (ie != -1)
			{
				CString sub = str.Mid(is, ie - is);
				pItem->AddOption(sub);
				is = ie + 1;
				ie = str.Find(';', is);
			}
		}
		pItem->SetValue(_variant_t(strAll));
		pItem->SetReadOnly(TRUE);

		CWnd * pWnd = GetDlgItem(IDC_APM_EDIT_MUTILEVALUE);
		if(pWnd)
		{
			pWnd->SetFocus();
		}
	}
}

void CProcessCustomPage::OnBnClickedButtonMutileValueDel()
{
	if(((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->GetCurSel() == -1)
		return;

	((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->DeleteString(((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->GetCurSel());

	WriteProp(m_nRowSel);

	//modify by hkf
	CAPSProp prop;
	//m_pAsmStep->SeekObjProp(m_strUsePropName,prop);
	m_useList.SeekProp(m_strUsePropName, &prop);

	CString strAll = prop.ConvertTo();
	if(m_nRowSel>=0)
	{
		//m_pTreeList->SetItemText(m_nRowSel, 1, strAll);

		CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(m_nRowSel);
		CBCGPGridItem *pItem = pRow->GetItem(1); //����ֵ
		pItem->RemoveAllOptions();
		CString str = prop.ConvertDefaultTo();
		int is = str.Find('|');
		int ie = str.Find(';');
		if (is != -1 && ie > is)
		{
			is = is + 1;
			while (ie != -1)
			{
				CString sub = str.Mid(is, ie - is);
				pItem->AddOption(sub);
				is = ie + 1;
				ie = str.Find(';', is);
			}
		}
		pItem->SetValue(_variant_t(strAll));
		pItem->SetReadOnly(TRUE);
	}

	CWnd * pWnd = GetDlgItem(IDC_APM_LIST_MUTILEVALUE);
	if(pWnd)
	{
		pWnd->SetFocus();
	}
}

void CProcessCustomPage::OnKeyDownInMutileValue(NMHDR *pNMHDR, LRESULT *pResult)
{
	ASSERT(false);
	return ;

	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	int nCount = ((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->GetSelCount();
	if(nCount > 0)
	{
		((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->DeleteString(((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->GetCurSel());
	}

	*pResult = 0;
}
//����TypeΪ�б�//
bool CProcessCustomPage::IsMutileType(int nIndex)
{
	if (m_pPropListCtrl==NULL || nIndex < 0 || nIndex >= m_pPropListCtrl->GetRowCount())
		return false;

	CString strType;
	//strType = m_pTreeList->GetItemText(nIndex,2);
	CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(nIndex);
	strType = pRow->GetItem(2)->GetValue();

	if(strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_LIST, "�б�")) == 0)
	{
		return true;
	}

	return false;
}

void CProcessCustomPage::UpdateMutileList()
{
	int      iType = 0;
	CAPSProp prop;
	CString  strName;

	//m_pAsmStep->m_useList.SeekProp(m_strUsePropName, &prop);
	m_useList.SeekProp(m_strUsePropName, &prop);

	//begin mod by dm in 20190419
	//old
	//CArray<CString,CString> vals;
	//prop.ConvertTo(vals);
	//new
	CArray<CString, CString> vals;
	CString strDefault = prop.ConvertDefaultTo();
	int is = strDefault.Find('|');
	int ie = strDefault.Find(';');
	if (is != -1 && ie > is)
	{
		is = is + 1;
		while (ie != -1)
		{
			CString sub = strDefault.Mid(is, ie - is);
			vals.Add(sub);
			is = ie + 1;
			ie = strDefault.Find(';', is);
		}
	}
	//end mod

	//
	CListBox * pListBox = static_cast<CListBox *>(GetDlgItem(IDC_APM_LIST_MUTILEVALUE));
	if(pListBox)
	{
		pListBox->ResetContent();
		for(int i = 0; i < vals.GetSize(); i++)
		{
			pListBox->AddString(vals.GetAt(i));
		}
	}
}
/*BOOL CProcessCustomPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    int nCol = LOWORD(wParam);
    int a = nCol;
    return TRUE;
}*/

void CProcessCustomPage::OnKillFocus(CWnd* pNewWnd)
{
	/*
	CKmPropertyPage::OnKillFocus(pNewWnd);

	if(m_pAsmStep == NULL || m_pManager == NULL) return ;

	if(m_pManager && m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}
	*/

	//WriteProp();
}

BOOL CProcessCustomPage::OnInitDialog() 
{
	CKmPropertyPage::OnInitDialog();
	//m_baseList.RemoveAll();
	m_useList.RemoveAll();
	SetMutileEditShow(false);
	if (GetDlgItem(IDC_APM_CUSTOM_SAVE))
	{
		GetDlgItem(IDC_APM_CUSTOM_SAVE)->ShowWindow(SW_HIDE);
	}
	m_ImageList.DeleteImageList();
	m_ImageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 1, 2);
	CBitmap bm;
	bm.LoadBitmap(IDB_APM_HEADERIMAGE);
	m_ImageList.Add(&bm, RGB(192, 192, 192));
	DeleteObject(bm);

	CRect rect;
	GetClientRect(&rect);
	rect.left += 2;
	rect.top += 10;

	//begin mod by dm ����bcg�ؼ�
	//old
	//m_pTreeList = &m_listDefine;
	//m_pTreeList->SetItemCanDrag(FALSE);

	////m_pTreeList = new KmAPSTreeListCtrl::CTreeListCtrl();
	////m_pTreeList->Create(WS_VSCROLL | WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL /*| TVS_DISABLEDRAGDROP*/, rect, this, IDC_APM_RES_LIST);
	////m_pTreeList->SetExtendedStyle(WS_EX_RIGHTSCROLLBAR | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | WS_EX_TOPMOST);
	////m_pTreeList->EnableWindow();

	//SetListStyle();
	//CHeaderCtrl * pHeaderCtrl = m_pTreeList->GetHeaderCtrl();
	//if(pHeaderCtrl)
	//{
	//	pHeaderCtrl->SetImageList(&m_ImageList);
	//}
	//
	////InitColumn();
	////InitListCtrl();
	//new
	//m_listDefine.ShowWindow(SW_HIDE);

	CStringArray strColNames;
	strColNames.Add(ResourceString(IDS_APM_PORPNAME, "��������"));
	strColNames.Add(ResourceString(IDS_APM_PROPVALUE, "����ֵ"));
	strColNames.Add(ResourceString(IDS_APM_PROPTYPE, "��������"));
	UINT nStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | TVS_LINESATROOT | TVS_HASLINES | TVS_EDITLABELS;
	m_pPropListCtrl = new CPropListCtrl();
	m_pPropListCtrl->Create(nStyles, rect, this, IDC_APM_PROCESSCUSTORM_LIST);
	m_pPropListCtrl->SetColNames(&strColNames, 2, 1);
	m_pPropListCtrl->ShowWindow(SW_SHOW);
	m_pPropListCtrl->SetSingleSel(FALSE);
	//end mod

	AdjustPlace();
	UpdateData(FALSE);

	return TRUE;
}

void CProcessCustomPage::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CKmPropertyPage::OnWindowPosChanged(lpwndpos);

	//2011.8.5,���µ�������
	if (IsWindow(GetSafeHwnd()))
	{
		AdjustPlace();

		// 		bool bShow = GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->IsWindowVisible();
		// 		if (bShow)
		// 		{
		// 			m_listDefine.MoveWindow(5, 5, lpwndpos->cx-10, lpwndpos->cy/2);
		// 			
		// 			GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->MoveWindow(5, lpwndpos->cy/2 + 8, lpwndpos->cx*0.6,24);
		// 			GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE)->MoveWindow(lpwndpos->cx*0.6+5+5, lpwndpos->cy/2 + 8,32,24);
		// 			GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE)->MoveWindow(lpwndpos->cx*0.6+5+5+32+5, lpwndpos->cy/2 + 8,32,24);
		// 			GetDlgItem(IDC_APM_LIST_MUTILEVALUE)->MoveWindow(5,lpwndpos->cy/2 + 8 + 24 + 4, lpwndpos->cx-10-5, lpwndpos->cy/2-60);
		// 		}
		// 		else
		// 		{
		// 			m_listDefine.MoveWindow(5, 5, lpwndpos->cx-10, lpwndpos->cy - 10);
		// 		}

	}
}

void CProcessCustomPage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	/*
	if(m_pManager && m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}
	
	if(m_bDoModel) //����������Դ������
	{
		return ;
	}

	WriteProp();
	*/
}

void CProcessCustomPage::OnKillFocusListMultiValue()
{
	OnKillFocusSave();
}

void CProcessCustomPage::OnKillFocusEditMultiValue()
{
	OnKillFocusSave();
}

void CProcessCustomPage::OnKillFocusCustomList(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnKillFocusSave();
}

void CProcessCustomPage::OnKillFocusSave()
{
	if(m_pProcess == NULL || m_pManager == NULL || m_pManager->m_iLaunchMode == EMBED_MODE) return ;

	if(m_bDoModel) //���������Ի���
	{
		return ;
	}

	HWND hFocus = ::GetFocus();

	CWnd * pWnd1 = GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE);
	CWnd * pWnd2 = GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE);
	CWnd * pWnd3 = GetDlgItem(IDC_APM_EDIT_MUTILEVALUE);
	CWnd * pWnd4 = GetDlgItem(IDC_APM_LIST_MUTILEVALUE);
	CWnd * pWnd5 = GetDlgItem(IDC_APM_CUSTOM_SAVE);

	if(pWnd1 && hFocus == pWnd1->GetSafeHwnd()) return ;
	if(pWnd2 && hFocus == pWnd2->GetSafeHwnd()) return ;

	if(pWnd3 && hFocus == pWnd3->GetSafeHwnd()) return ;
	if(pWnd4 && hFocus == pWnd4->GetSafeHwnd()) return ;
	if(pWnd5 && hFocus == pWnd5->GetSafeHwnd()) return ;

	//if(hFocus == m_pTreeList->GetSafeHwnd()) return ;
	if(hFocus == m_pPropListCtrl->GetSafeHwnd()) return ;

	//if(::GetParent(hFocus) == m_pTreeList->GetSafeHwnd()) return ;

//	if(!m_bModified) return ;

	WriteProp();

	SetModify(false);
}

void CProcessCustomPage::SetMutileEditShow(bool bShow)
{
	CWnd* pWnd = NULL;
	if(bShow)
	{
		pWnd = GetDlgItem(IDC_APM_EDIT_MUTILEVALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
		}

		pWnd = GetDlgItem(IDC_APM_LIST_MUTILEVALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
		}

		pWnd = GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
		}

		pWnd = GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		pWnd = GetDlgItem(IDC_APM_EDIT_MUTILEVALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		pWnd = GetDlgItem(IDC_APM_LIST_MUTILEVALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		pWnd = GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		pWnd = GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
	
}

void CProcessCustomPage::SetModify(bool bModify)
{
	//ʹ��ť�仯
	CWnd * pWnd = GetDlgItem(IDC_APM_CUSTOM_SAVE);
	if(pWnd)
	{
		pWnd->EnableWindow(bModify);
	}

	m_bModified = bModify;
}

void CProcessCustomPage::OnBnClickedButtonSave()
{
	if(!m_bModified) return ;
	//fix bug 4641 
	CString strFileName = m_pManager->GetModel()->GetOpenedFileName();
	m_pManager->GetModel()->Save(strFileName, 0, SERIAL_NORMAL);

	WriteProp();

	SetModify(false);
}

LRESULT CProcessCustomPage::OnEndLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	if (m_pPropListCtrl && wParam == IDC_APM_PROCESSCUSTORM_LIST)
	{
		BCGPGRID_ITEM_INFO *info = (BCGPGRID_ITEM_INFO *)lParam;
		int nItemIndex = info->nRow;
		int nSubIndex = info->nCol;
		if (info->pItem == NULL)
		{
			return S_FALSE;
		}

		CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(nItemIndex);
		if (pRow == NULL)
		{
			return S_FALSE;
		}
		CBCGPGridItem *pItem = pRow->GetItem(nSubIndex);
		if (pItem)
		{
			int nPropIndex = nItemIndex;
			CAPSPropList tmpPropList;
			tmpPropList.CopyVis(m_useList, true);

			if (nPropIndex >= 0 && nPropIndex < tmpPropList.GetCount())
			{
				CAPSProp &prop = tmpPropList.GetAt(tmpPropList.FindIndex(nPropIndex));
				CString strOldName = prop.GetName();
				CString strPropName(pRow->GetItem(0)->GetValue());

				//����Ϊ�գ���ʾҪɾ��������
				if (strPropName.GetLength() == 0)
				{
					if (1/*nPropIndex != 0*/)
					{
						SetModify(true);
						//tmpPropList.RemoveAt(tmpPropList.FindIndex(nPropIndex));
						POSITION dPos = m_useList.FindProp(strOldName);
						if (dPos)
						{
							m_useList.RemoveAt(dPos);
						}

						//m_pTreeList->DeleteItemEx(m_pTreeList->GetTreeItem(nItemIndex), nItemIndex);
						m_pPropListCtrl->RemoveRow(nItemIndex);
					}
					else
					{
						ASSERT(false);
					}
				}
				//�޸�ĳ������
				else
				{
					if (IsFreePropName(tmpPropList, strPropName, nPropIndex))
					{
						CAPSProp oldProp = prop;
						//int nType = ConvertType( pItemInfo->GetSubItem(1));
						CString strType = pRow->GetItem(2)->GetValue();
						int nType = ConvertType(strType);
						int nPreType = prop.GetType();
						if (nType == APSVT_NULL)
						{
							return S_FALSE;
						}
						prop.SetType(nType);

						CString strText = pItem->GetValue();
						if (nSubIndex == 0) //��һ�У��޸���������
						{
							if (strText.CompareNoCase(strPropName) != 0)
							{
								prop.SetName(strPropName);
								SetModify(true);
							}
						}
						//��ʾ�޸ĵ������һ�У����������ͣ���ʱ����Ҫ�ı�����ֵ����ʾ������������ʾ
						else if (nSubIndex == 2)
						{
							//ȷʵ�޸�������
							if (nType != nPreType)
							{
								//CString strValue = pItemInfo->GetSubItem(0);
								CString strValue = pRow->GetItem(1)->GetValue();
								FormatStrByPropType(nType, strValue);

								if (nType == APSVT_BOOL)
								{
									CString strBoolDefault = ResourceString(IDS_APM_NO, "��");
									pRow->GetItem(1)->SetValue(_variant_t(strBoolDefault));
									pRow->GetItem(1)->AddOption(ResourceString(IDS_APM_YES, "��"));
									pRow->GetItem(1)->AddOption(ResourceString(IDS_APM_NO, "��"));
									pRow->GetItem(1)->SetReadOnly(TRUE);
								}
								else if (nType == APSVT_CHECK)
								{
									CBCGPGridCheckItem *pCheck = (CBCGPGridCheckItem *)pRow->GetItem(1);
									if (pCheck != NULL)
									{
										bool bval = atoi(strValue);
										pCheck->SetState(bval);
									}
								}
								else if (nType == APSVT_MUTILE) //�б�
								{
									pRow->GetItem(1)->SetValue(_variant_t(CString("")));
									pRow->GetItem(1)->SetReadOnly(TRUE);
								}
								else
								{
									pRow->GetItem(1)->RemoveAllOptions();
									pRow->GetItem(1)->SetValue(_variant_t(CString("")));
									pRow->GetItem(1)->SetReadOnly(FALSE);
								}
								prop.ModifyByType(strValue);
							}
							SetModify(true);
						}
						else //�޸�����ֵ
						{
							CString strValue = strText;
							FormatStrByPropType(nType, strValue);
							prop.m_Val.m_str = strValue;
							prop.ModifyByType(strValue);
							SetModify(true);
						}

						if (oldProp.GetType() == APSVT_MUTILE && prop.GetType() != APSVT_MUTILE)
						{
							SetMutileEditShow(false);
							AdjustPlace();
						}
						if (oldProp.GetName().CompareNoCase(prop.GetName()) != 0 ||
							oldProp.GetType() != prop.GetType() ||
							oldProp.ConvertTo().CompareNoCase(prop.ConvertTo()) != 0)
						{
							m_useList.ModifyProp(oldProp.GetName(), prop);

							WriteProp();
						}
					}
					else
					{
						CString strShowPropName = prop.GetDisplayName();
						//m_pTreeList->SetItemText(nItemIndex, 0, strShowPropName);
						pRow->GetItem(0)->SetValue(_variant_t(strShowPropName));

						KmMessageBox(ResourceString(IDS_APM_PROMT44, "����������֮ǰ������������������"), APS_MSGBOX_TITIE, MB_ICONWARNING);

						m_pPropListCtrl->SetFocus();
					}
				}
			}
		}
	}

	return 1;
}

//void CProcessCustomPage::SetListStyle(void)
//{
//	CRect rect;    
//	GetWindowRect(&rect);
//
//	m_pTreeList->InsertColumn(0, ResourceString(IDS_APM_PORPNAME, "��������"), LVCFMT_LEFT, rect.Width() * 1/3, 0);
//	m_pTreeList->InsertColumn(1, ResourceString(IDS_APM_PROPVALUE, "����ֵ"), LVCFMT_LEFT, rect.Width() * 1/3, 0);
//	// Bug10526 ѡ������������������� Modify by ��˱� 2009-12-02
//	m_pTreeList->InsertColumn(2, ResourceString(IDS_APM_PROPTYPE, "��������"), LVCFMT_LEFT, rect.Width() * 1/3, 0);
//	// end Modify by ��˱� 2009-12-02
//
//	m_pTreeList->SetExtendedStyle(m_pTreeList->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);    
//	m_pTreeList->EnableWindow(TRUE);
//}

void CProcessCustomPage::UpdateItem(int nItem, const CAPSProp & Prop)
{
	if (nItem < 0 || m_pPropListCtrl==NULL || nItem >= m_pPropListCtrl->GetRowCount()) return;

	CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(nItem);
	if (pRow == NULL)
	{
		return;
	}
	
	CString strType;
	switch(Prop.GetType())
	{
	case 0:
		strType = ResourceString(IDS_APM_PROPTYPE_NULL, "������");
		break;
	case 1:
		strType = ResourceString(IDS_APM_PROPTYPE_STRING, "�ַ���");
		break;
	case 2:
		strType = ResourceString(IDS_APM_PROPTYPE_NUMBER, "��ֵ");
		break;
	case 3:
		strType = ResourceString(IDS_APM_PROPTYPE_INT, "����");
		break;
	case 5:
		strType = ResourceString(IDS_APM_PROPTYPE_LIST, "�б�");
		break;
	case 6:
		strType = ResourceString(IDS_APM_PROPTYPE_LOGIC,"�߼�");
		break;
	//begin add dm in 2019.04.17 for bug70905
	case 7:
		strType = ResourceString(IDS_APM_PROPTYPE_CHECKBOX, _T("��ѡ��"));
		break;
	//end add
	default:
		break;
	}

	pRow->GetItem(0)->SetValue(_variant_t(Prop.GetName()));
	CBCGPGridItem *pItem = pRow->GetItem(1);
	if (Prop.GetType() == 7 && pItem->GetRuntimeClass() == RUNTIME_CLASS(CBCGPGridCheckItem))
	{
		CBCGPGridCheckItem *pCheck = (CBCGPGridCheckItem *)pItem;
		bool ival = atoi(Prop.ConvertTo());
		pCheck->SetState(ival);
	}
	else
	{
		pItem->SetValue(_variant_t(Prop.ConvertTo()));
	}
	pRow->GetItem(2)->SetValue(_variant_t(strType));

}

BOOL CProcessCustomPage::IsFreePropName(const CAPSPropList & PropList, const CString & name, int nIgnoreIndex/* = -1*/)
{
	BOOL re = TRUE;

	int nIndex = 0;
	POSITION pos = PropList.GetHeadPosition();
	while(pos != 0)
	{
		const CAPSProp & prop = PropList.GetNext(pos);

		if (!prop.GetVisInProplistWnd())
		{
			continue;
		}

		if(nIgnoreIndex != nIndex)
		{
			if(name.CompareNoCase(prop.GetName()) == 0)
			{
				re = FALSE;
				break;
			}
		}

		++nIndex;
	}

	return re;
}

int CProcessCustomPage::ConvertType(LPCTSTR szType)
{
	CString strType(szType);
	int ret = 0;
	if (strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_STRING,"�ַ���")) == 0)
	{
		ret = APSVT_STR;
	}
	else if (strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_NUMBER,"��ֵ")) == 0)
	{
		ret = APSVT_DOUBLE;
	}
	else if (strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_INT,"����")) == 0)
	{
		ret = APSVT_INT;
	}
	else if (strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_LIST,"�б�")) == 0)
	{
		ret = APSVT_MUTILE;
	}
	else if (strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_NULL,"������")) == 0)
	{
		ret = APSVT_NULL;
	}
	else if (strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_LOGIC,"�߼�")) == 0)
	{
		ret = APSVT_BOOL;
	}
	//begin add dm in 2019.04.17 for bug70905
	else if (strType.CompareNoCase(ResourceString(IDS_APM_PROPTYPE_CHECKBOX, _T("��ѡ��"))) == 0)
	{
		ret = APSVT_CHECK;
	}
	//end add

	return ret;
}

void CProcessCustomPage::ReadProp(void)
{
	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	//    ASSERT(m_baseList.GetCount() == 2); 
	m_ColumnItems.RemoveAll();

	
	
	CProcess::GetPropByID(PROCESSUSEPROPCONFIG, &m_ColumnItems);



	//���б���ȫ�����
#if 0
	m_pTreeList->DeleteAllItems();

	CHeaderCtrl * pHeadCtrl = m_pTreeList->GetHeaderCtrl();
	if(pHeadCtrl)
	{
		int nCount = pHeadCtrl->GetItemCount();
		// Delete all of the items.
		for (int i = 0; i < nCount; ++i)
		{
			//pHeadCtrl->DeleteItem(0);
			m_pTreeList->DeleteItem(0);
		}
	}

	CHeaderCtrl * pHeaderCtrl = m_pTreeList->GetHeaderCtrl();
	int index = 0;
	POSITION pos = m_useList.GetHeadPosition();
	while (pos != NULL)
	{
		const CAPSProp & prop = m_useList.GetNext(pos);
		if (prop.GetShow() && prop.GetVisInProplistWnd())
		{
			InsertUsePropItem(prop);
		}
	}
	int iok = m_pTreeList->GetItemCount();
#else
	m_pPropListCtrl->SetUserData(m_useList);
#endif
}

void CProcessCustomPage::WriteProp(void)
{	
	if (!m_bModified)
	{
		return;
	}
	SetModify(false);

	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	if (m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	try
	{
		m_pPropListCtrl->GetUserData(m_useList);

		//���ڻ��˹�����
		if(m_pManager->GetModel()->GetHistoryMgr()->GetNotify()) return ;

		CAPSPropList * pProcessUserProp = m_pProcess->GetUserPropList();
		if(m_useList == *pProcessUserProp) return ;

		//begin add cq in 2018.11.23 for bug70090
		m_pProcess->SetModify();
		//end add
		
		HistoryModifyData<CAPSPropList> * pData = new HistoryModifyData<CAPSPropList>(HISTORY_ACTION_MODIFYUSEPROP);
		pData->m_data.AddTail(pProcessUserProp);
		HistoryProcessModifyBase * pCmd = new HistoryProcessModifyBase(m_pProcess, pData);
		m_pManager->GetModel()->AddCmd(pCmd);

		/*
		char szBuf[APS_MAX_STRLEN]; 
		int iCount = m_listDefine.GetItemCount();
		m_useList.RemoveAll();
		for (int i = 0; i < iCount; i++)
		{
			int         iType = 0;
			CAPSProp    prop;
			CString     strName;

			iType = m_listDefine.GetItemData(i);
			m_listDefine.GetItemText(i, 0, szBuf, APS_MAX_STRLEN);
			strName = szBuf;

			prop.SetName(strName);
			prop.SetType(iType);

			m_listDefine.GetItemText(i, 1, szBuf, APS_MAX_STRLEN);

			prop.ModifyByType(szBuf);
            m_useList.ModifyProp(prop.GetName(), prop);
			m_useList.ModifyProp(prop.GetName(), prop);
		}
		*/

		//���»�ڲ�����
		m_pProcess->SetUserPropList(m_useList);

	}
	catch (...) 
	{
		//plh KMAPS3.0 ѡ��װ����̽ڵ�ʱ���㱣�棬������
		ASSERT(FALSE);
	}
}

void CProcessCustomPage::SetBaseList(CAPSPropList& list)
{
	//m_baseList.RemoveAll();
	//m_baseList.AddTail(&list);
}

void CProcessCustomPage::SetUseList(CAPSPropList& list)
{
	m_useList.RemoveAll();
	m_useList.AddTail(&list);
}

void CProcessCustomPage::SetAPManager(CAPManager* pManager)
{
	m_pManager = pManager;

	if (pManager)
	{
		//m_pTreeList->m_iLaunchMode = pManager->m_iLaunchMode;
	}
}

void CProcessCustomPage::Update(CProcess * pProcess)
{
	
	m_pProcess = pProcess;

	// KMAPS3.2 Add by �¹ں� 2010-9-1	
	if (!IsWindow(this->GetSafeHwnd()))
	{
		return;
	}
	// End Add by �¹ں� 2010-9-1

	// Bug 10141 KMAPS3.0 Add by �¹ں� 2009-08-05
	// ʹOnKillfocusEditBox�����ã������༭״̬��
	if (m_pManager->m_iLaunchMode != EMBED_MODE)
	{
		//this->SetFocus();
	}
	// end Add by �¹ں� 2009-08-05

	//m_baseList.RemoveAll();
	m_useList.RemoveAll();
	//m_pTreeList->DeleteAllItems();
	m_pPropListCtrl->RemoveAll();

	if (m_pManager && m_pProcess)
	{
		//m_baseList.AddTail(m_pAsmStep->GetBasePropList());
		m_useList.AddTail(m_pProcess->GetUserPropList());
		if(m_useList.GetCount()== 0 && m_pManager->GetModel()->GetOpenFileType() == 1 )//�����ļ�
		{
			m_useList.Copy(CProcess::s_processUseList);
		}
		/*
		m_baseList.AddTail( &(m_pAsmStep->m_baseList) );
		m_useList.AddTail( &(m_pAsmStep->m_useList) );
		*/
		ReadProp();
	}

	//�Ա�ʧȥ����ʱ����
	//m_listDefine.SetFocus();

	SetModify(false);
}

bool CProcessCustomPage::CanEdit(int nRow,int nCol)
{
	if(IsMutileType(nRow))
	{
		return false;
	}

	CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(nRow);
	//CString propName = m_pTreeList->GetItemText(nRow,0);
	CString propName = pRow->GetItem(0)->GetValue();
	CAPSProp Prop;
	m_ColumnItems.SeekProp(propName, &Prop);
	if(!Prop.GetRegMgrPath().IsEmpty() || Prop.GetType() == APSVT_BOOL)
	{
		return false;
	}

    //CString strName = m_pTreeList->GetItemText(nRow,0);
	CAPSProp prop;
	
	if(nCol == 0 || nCol == 2)
		return false;

	return true;
}

LRESULT CProcessCustomPage::OnBCGGridSelChanged(WPARAM wp, LPARAM lp)
{
	int ret = 0;
	//���������ֵһ�����ᵽѡ���������ȥ�鿴��Դ������·��//
	if (m_pPropListCtrl == (CWnd *)lp)
	{
		CBCGPGridRow *pRow = m_pPropListCtrl->GetCurSel();
		CBCGPGridItem *pItem = m_pPropListCtrl->GetCurSelItem();
		if ((pItem == NULL) || (pRow == NULL))
		{
			return S_FALSE;
		}

		int nItemIndex = pRow->GetRowId();
		int nSubIndex = pItem->GetColumnId();
		m_nItemIndex = nItemIndex;
		m_nSubItemIndex = nSubIndex;

		if (m_pProcess && nItemIndex != -1)
		{
			//�Ƿ���ʾ����Ϊ�б�ʱ��
			ShowTheList(nItemIndex);
			if (nSubIndex == 1)
			{
				int iItemIndex = nItemIndex;
				int iSubIndex = nSubIndex;

				//CItemInfo::CONTROLTYPE type = CItemInfo::none;
				//CItemInfo *pInfo = m_pTreeList->GetListItem(iItemIndex);
				//if (pInfo != NULL)
				//{
				//	pInfo->GetControlType(iSubIndex, type);
				//}

				CString propName = pRow->GetItem(0)->GetValue();
				CAPSProp Prop;

				if (m_ColumnItems.SeekProp(propName, &Prop))
				{
					CString strItemText = pItem->GetValue();
					//begin add by dm in 2019.04.17 for bug70905
					if (CBCGPGridCheckItem* pCheck = dynamic_cast<CBCGPGridCheckItem*>(pItem))
					{
						int nState = pCheck->GetState();
						strItemText.Format("%d",nState);
					}	
					//end add

					bool bRestore = false;
					CString strAPMType = pRow->GetItem(2)->GetValue();
					int nType = ConvertType(strAPMType);
					CString s_Val;
					FormatStrByPropType(nType, s_Val);
					strItemText = s_Val;
					//�ж���Դ������·���Ƿ����
					//OriPropΪѡ��ҳ�������			
					if (!Prop.GetRegMgrPath().IsEmpty() && ConvertType(strAPMType) != APSVT_BOOL && ConvertType(strAPMType) != APSVT_MUTILE/*  && type == CItemInfo::edit*/ && ConvertType(strAPMType) != APSVT_CHECK)
					{
						CString strResText;
						m_bDoModel = true;

						bool bSuccess = false;
						KmResViewProxy kmResView;
						kmResView.SetParentWnd(m_hWnd);
						kmResView.SetRootNodeName(Prop.GetRegMgrPath());
						if (kmResView.DoModal() == IDOK)
						{
							strResText = kmResView.GetValue();
							bSuccess = true;
						}

						m_bDoModel = false;

						if (!bSuccess/*strResText.IsEmpty()*/)
						{
							//bRestore = true;
							//strItemText = Prop.ConvertDefaultTo();
						}
						else
						{
							SetModify(true);
							strItemText = strResText;

							POSITION pos = m_useList.SeekProp(propName);
							if (pos)
							{
								CAPSProp & refProp = m_useList.GetAt(pos);
								FormatStrByPropType(refProp.GetType(), strItemText);

								refProp.ModifyByType(strItemText);
								//refProp.m_Val.m_str = strItemText;
								//�޸�����
								//m_useList.ModifyProp(propName, Prop);
								bRestore = true;
							}
						}

						ret = 1;
					}
					if (bRestore)
					{
						//m_pTreeList->SetItemText(iItemIndex, iSubIndex, strItemText);
						pItem->SetValue(_variant_t(strItemText));

						//LV_ITEM lvItem;
						//lvItem.mask = LVIF_TEXT;
						//lvItem.cchTextMax = strItemText.GetLength();
						//lvItem.pszText = strItemText.GetBuffer();
						//lvItem.iSubItem = iSubIndex;
						//lvItem.iItem = iItemIndex;
						//m_pTreeList->SetItem(&lvItem);
						//propName.ReleaseBuffer();

					}
				}

				//���ý��㣬�Ա�ʧȥ����ʱ����
				m_pPropListCtrl->SetFocus();
			}
		}

		if (ret != 0) return ret;

		if (IsMutileType(nItemIndex))
		{
			return nSubIndex == 1;
		}
		ShowTheList(nItemIndex);
		//m_pTreeList->EditLabel(nItemIndex);
		//m_strUsePropName = m_pTreeList->GetItemText(nItemIndex, nSubIndex);
		m_strUsePropName = pRow->GetItem(nSubIndex)->GetValue();
	}
	return TRUE;
}

LRESULT CProcessCustomPage::OnBeginLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	return 0;
#if 0
	int nItemIndex = LOWORD(wParam);
	int nSubIndex = HIWORD(wParam);

	m_nItemIndex = nItemIndex;
	m_nSubItemIndex = nSubIndex;

	int ret = 0;
	if(m_pTreeList && (HWND)lParam == m_pTreeList->GetSafeHwnd())
	{
		//���һ��������Զ��������������£��Զ������µ����ԣ�����Ĭ��Ϊ'����i'������Ĭ��Ϊ�ַ���//
		if(m_pProcess && nItemIndex == -1)
		{
			ShowTheList(nItemIndex);
			if(nSubIndex == 0)
			{
				CString strName = GetFreePropName();
				CAPSProp propAdd;
				if(m_useList.SeekProp(strName, &propAdd))
				{
					m_bDoModel = true;
					KmMessageBox(ResourceString(IDS_APM_EXISTUSERPROP, "���Զ��������Ѵ���!"), APS_MSGBOX_TITIE, MB_ICONWARNING);
					m_bDoModel = false;
					m_pTreeList->SetFocus();
				}
				else
				{
					propAdd.SetName(strName);
					propAdd.SetType(propAdd.GetType());
					propAdd.ModifyByType(propAdd.ConvertDefaultTo());

					m_useList.AddTail(propAdd);

					CList<KmResViewProxy::ResViewFieldRecord, KmResViewProxy::ResViewFieldRecord> RecordsetList;
					int nNewIndex = m_BackUpResDatas.Add(m_ColumnItems);

					CAPSPropList & RefPropList = m_BackUpResDatas[nNewIndex];
					POSITION pos = RefPropList.GetHeadPosition();
					CAPSProp & prop = RefPropList.GetNext(pos);

					if(RecordsetList.GetCount() <= 0)
					{
						prop.ModifyByType(strName);
					}                               

					InsertPropListItem(RefPropList);

					SetModify(true);
					m_pTreeList->SetFocus();

				}
			}
		}
		//����������������ֵһ�����ᵽѡ���������ȥ�鿴��Դ������·��//
		else if(m_pProcess && nItemIndex != -1 )
		{
			//�Ƿ���ʾ����Ϊ�б�ʱ��
			ShowTheList(nItemIndex);
			if(nSubIndex == 1)  
			{
				int iItemIndex = nItemIndex;
				int iSubIndex = nSubIndex;

				CItemInfo::CONTROLTYPE type = CItemInfo::none;
				CItemInfo *pInfo = m_pTreeList->GetListItem(iItemIndex);
				if (pInfo != NULL)
				{
					pInfo->GetControlType(iSubIndex, type);
				}

				CString propName = m_pTreeList->GetItemText(iItemIndex,0);
				CAPSProp Prop;

				if(m_ColumnItems.SeekProp(propName, &Prop))
				{
					CString strItemText = m_pTreeList->GetItemText(iItemIndex,iSubIndex);
					bool bRestore = false;
					CString strAPMType = m_pTreeList->GetItemText(iItemIndex,2);
					int nType = ConvertType(strAPMType);
					CString s_Val;
					FormatStrByPropType(nType,s_Val);
					strItemText = s_Val;
					//�ж���Դ������·���Ƿ����
					//OriPropΪѡ��ҳ�������			
					if (!Prop.GetRegMgrPath().IsEmpty() && ConvertType(strAPMType) != APSVT_BOOL && ConvertType(strAPMType) != APSVT_MUTILE  && type == CItemInfo::edit)
					{
						CString strResText;
						m_bDoModel = true;

						bool bSuccess = false;
						KmResViewProxy kmResView;
						kmResView.SetParentWnd(m_hWnd);
						kmResView.SetRootNodeName(Prop.GetRegMgrPath());
						if (kmResView.DoModal() == IDOK)
						{
							strResText = kmResView.GetValue();
							bSuccess = true;
						}

						m_bDoModel = false;

						if(!bSuccess/*strResText.IsEmpty()*/)
						{
							//bRestore = true;
							//strItemText = Prop.ConvertDefaultTo();
						}
						else
						{
							SetModify(true);
							strItemText = strResText;

							POSITION pos = m_useList.SeekProp(propName);
							if(pos)
							{
								CAPSProp & refProp = m_useList.GetAt(pos);
								FormatStrByPropType(refProp.GetType(), strItemText);

								refProp.ModifyByType(strItemText);
								//refProp.m_Val.m_str = strItemText;
								//�޸�����
								//m_useList.ModifyProp(propName, Prop);
								bRestore = true;
							}
						}

						ret = 1;
					}
					if(bRestore)
					{
						m_pTreeList->SetItemText(iItemIndex, iSubIndex, strItemText);

						LV_ITEM lvItem;
						lvItem.mask = LVIF_TEXT;
						lvItem.cchTextMax = strItemText.GetLength();
						lvItem.pszText = strItemText.GetBuffer();
						lvItem.iSubItem = iSubIndex;
						lvItem.iItem = iItemIndex;
						m_pTreeList->SetItem(&lvItem);
						propName.ReleaseBuffer();

					}
				}

				//���ý��㣬�Ա�ʧȥ����ʱ����
				m_pTreeList->SetFocus();
			}
		}
	}

	if(ret != 0) return ret;

	if(IsMutileType(nItemIndex))
	{
		return nSubIndex == 1;
	}
	ShowTheList(nItemIndex);
	//m_pTreeList->EditLabel(nItemIndex);
	m_strUsePropName = m_pTreeList->GetItemText(nItemIndex,nSubIndex);


	return 0;
#endif
}


///////////////////////////////////////////////////////////////////////
// ��    ʶ  : KmAps 3.0 bug10380 �������Ա�ǩҳ�е��Զ������Բ������á���ѡ�������ж��岽�������е��Զ�������Ҳ�������ã�������ӵ��Զ������Բ������ܱ��档
// ��������  : CProcessCustomPage::WriteProp
// ��    ��  : 
//      int nRow -- 
// �� �� ֵ  : 
//      void -- 
// Ŀ��/���� : ֻдĳһ�е�ֵ
// ���ù�ϵ  : 
// �� �� ��  : ��˱�
// ��������  : 2009-10-23
// �����޸���:                        �޸����ڣ�
///////////////////////////////////////////////////////////////////////
void CProcessCustomPage::WriteProp(int nRow)
{
	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	if (m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	try
	{
		//ASSERT(m_baseList.GetCount() == 2);

		char szBuf[APS_MAX_STRLEN] = {0};
		int iType = 0;
		CAPSProp prop;
		CString strName;
		CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(nRow);

		//m_pAsmStep->m_useList.SeekProp(m_strUsePropName, &prop);
		m_useList.SeekProp(m_strUsePropName, &prop);
		if(IsMutileType(nRow))
		{
			CListBox * pListBox = (CListBox *)GetDlgItem(IDC_APM_LIST_MUTILEVALUE);
			if(pListBox)
			{
				CString str, strDefault; 
				int nCount = pListBox->GetCount();
				if (nCount > 0)
				{
					bool bFind = false;
					CStringArray arVals;
					str = prop.ConvertTo();
					for (int i = 0; i < nCount; i++)
					{
						CString ss;
						pListBox->GetText(i, ss);
						arVals.Add(ss);
						if (str.CollateNoCase(ss) == 0)
						{
							bFind = true;
						}
					}
										
					if (bFind)
					{
						strDefault = str + '|';
					}
					else
					{			
						str = arVals.GetAt(0);
						strDefault = arVals.GetAt(0) + '|';
					}
					for (int i = 0; i < (int)arVals.GetCount();++i)
					{
						strDefault += arVals.GetAt(i);
						strDefault += ';';
					}
				}

				prop.ModifyByType(str);
				prop.ModifyDefaultByType(strDefault);
			}
			else
				ASSERT(false);
		}
		else
		{
			//prop.ModifyByType(m_pTreeList->GetItemText(nRow, 1));
			CString strval = pRow->GetItem(1)->GetValue();
			prop.ModifyByType(strval);
		}

		//strName = m_pTreeList->GetItemText(nRow, 0);
		strName = pRow->GetItem(0)->GetValue();
		prop.SetName(strName);

		//m_pAsmStep->m_useList.ModifyProp(m_strUsePropName, prop);
		m_useList.ModifyProp(m_strUsePropName, prop);

		//���¸��������
		UpdateItem(nRow, prop);
		//m_listDefine.SetItemText(nRow, 0, prop.ConvertTo());
		//m_listDefine.DeleteAllItems();
		//m_useList.RemoveAll();
		//m_useList.AddTail( &(m_pAsmStep->m_useList) );
		//ReadProp();
		SetModify(true);
		//m_pTreeList->SetFocus();
		m_pPropListCtrl->SetFocus();
	}
	catch (...) 
	{
		//plh KMAPS3.0 ѡ��װ����̽ڵ�ʱ���㱣�棬������
		ASSERT(FALSE);
	}
}
void CProcessCustomPage::OnRclickListMutileValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pManager && m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	*pResult = 0;
}

LRESULT CProcessCustomPage::OnBCGGridRButtonUp(WPARAM wParam, LPARAM lParam)
{
	if (m_pManager && m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return FALSE;
	}

	//�������˵�
	if (!m_pProcess->GetAttrEdit())
	{
		return FALSE;
	}

	//LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	//�õ���ǰ������к���
	int iColIndex = -1;
	CPoint point(lParam);
	int iRowIndex = m_pPropListCtrl->GetCurSelItemID().m_nRow;
	CBCGPGridRow *pRow = m_pPropListCtrl->GetCurSel();

	CMenu cMenu;
	cMenu.LoadMenu(MAKEINTRESOURCE (IDR_APM_MENU_MODIFYUSEPROP));

	cMenu.m_hMenu = cMenu.GetSubMenu(0)->m_hMenu;
	if (cMenu.m_hMenu == NULL)
	{
		ASSERT(false);  

		return FALSE;
	}
		
	if (iRowIndex != -1 && pRow != NULL)
	{
		//m_strUsePropName = m_pTreeList->GetItemText(iRowIndex, 0);
		m_strUsePropName = pRow->GetItem(0)->GetValue();
	}

	if (iRowIndex < 0 || iRowIndex > m_pPropListCtrl->GetRowCount())
	{
		cMenu.EnableMenuItem(IDM_APM_DELETE_USEPROP, MF_GRAYED);
		cMenu.EnableMenuItem(IDM_APM_UPDATE_USEPROP, MF_GRAYED);
	}

	//begin add in 2018.12.04 for bug70282
	//int nSelCount = m_pTreeList->GetSelectedCount();
	int nSelCount = m_pPropListCtrl->GetSelectionCount();
	if (nSelCount>1)
	{
		cMenu.EnableMenuItem(IDM_APM_UPDATE_USEPROP, MF_GRAYED);
	}
	//end add

	GetCursorPos(&point);	
	cMenu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// ��    ʶ  : KmAps 3.0 bug10380 �������Ա�ǩҳ�е��Զ������Բ������á���ѡ�������ж��岽�������е��Զ�������Ҳ�������ã�������ӵ��Զ������Բ������ܱ��档
// ��������  : CProcessCustomPage::OnAddUseprop
// ��    ��  : 
// �� �� ֵ  : 
//      void -- 
// Ŀ��/���� : ����Զ�������
// ���ù�ϵ  : 
// �� �� ��  : ��˱�
// ��������  : 2009-10-23
// �����޸���:                        �޸����ڣ�
///////////////////////////////////////////////////////////////////////
void CProcessCustomPage::OnAddUseprop() 
{
	// Bug 765 Add by �¹ں�
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// End Add by �¹ں�

	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	if (m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	CAddUseProp dlgAddUseProp;

	m_bDoModel = true;
	if (dlgAddUseProp.DoModal() == IDOK)
	{
		CAPSProp propAdd;
		//if(m_pAsmStep->m_useList.SeekProp(dlgAddUseProp.m_strName, &propAdd))
		if(m_useList.SeekProp(dlgAddUseProp.m_strName, &propAdd))
		{
			KmMessageBox(ResourceString(IDS_APM_EXISTUSERPROP, "���Զ��������Ѵ���!"), APS_MSGBOX_TITIE, MB_ICONWARNING);
		}
		else
		{
			propAdd.SetName(dlgAddUseProp.m_strName);
			propAdd.SetType(dlgAddUseProp.GetSelType());
			propAdd.ModifyByType(dlgAddUseProp.m_strValue);

			switch(dlgAddUseProp.GetSelType())
			{
			case 0:
				break;
			case 1:
			case 2:
			case 3:
				::_tcscpy(propAdd.m_dftVal.m_str.GetBuffer(512), dlgAddUseProp.m_strDefaultVal);
				propAdd.m_dftVal.m_str.ReleaseBuffer();
				break;
			default:
				break;
			}
			m_useList.AddTail(propAdd);
			//m_pTreeList->DeleteAllItems();
			m_pPropListCtrl->RemoveAll();
			ReadProp();	
	
 			SetModify(true);
			WriteProp();

// 			/*
// 			switch(dlgAddUseProp.GetSelType())
// 			{
// 			case 0:
// 				break;
// 			case 1:
// 			case 2:
// 			case 3:
// 				::strcpy(propAdd.m_dftVal.m_str.GetBuffer(512), dlgAddUseProp.m_strDefaultVal);
// 				propAdd.m_dftVal.m_str.ReleaseBuffer();
// 				break;
// 				// 		case 2:
// 				// 			{
// 				// 				double dVal = atof((LPCTSTR)dlgAddUseProp.m_strDefaultVal);
// 				// 				propAdd.m_dftVal.m_d = dVal;	
// 				// 			}
// 				// 			break;
// 				// 		case 3:
// 				// 			{
// 				// 				int iVal = atoi((LPCTSTR)dlgAddUseProp.m_strDefaultVal);
// 				// 				propAdd.m_dftVal.m_i = iVal;	
// 				// 			}
// 				// 			break;
// 			default:
// 				break;
// 			}
// 			*/
// 			//m_pAsmStep->m_useList.AddTail(propAdd);
// 
// 			m_useList.AddTail(propAdd);
// 
// 			CList<KmResViewProxy::ResViewRecord, KmResViewProxy::ResViewRecord> RecordsetList;
// 			int nNewIndex = m_BackUpResDatas.Add(m_ColumnItems);
// 
// 			CAPSPropList & RefPropList = m_BackUpResDatas[nNewIndex];
// 			POSITION pos = RefPropList.GetHeadPosition();
// 			CAPSProp & prop = RefPropList.GetNext(pos);
// 
// 			if(RecordsetList.GetCount() <= 0)
// 			{
// 				prop.ModifyByType(propAdd.GetName());
// 			}                               
// 
// 			InsertPropListItem(RefPropList);
// 			//����һ��//
// 			// 			int nNewItem = m_pTreeList->InsertItem(m_pTreeList->GetItemCount(), _T(""));
// 			// 			UpdateItem(nNewItem, propAdd);
// 			// 
// 			// 			m_pTreeList->RedrawItems(nNewItem, nNewItem);
// 			//m_listDefine.DeleteAllItems();
// 			//m_useList.RemoveAll();
// 			//m_useList.AddTail( &(m_pAsmStep->m_useList) );
// 			//ReadProp();
// 
// 			SetModify(true);
		}
	}

	m_bDoModel = false;

	//�Ա�ʧȥ����ʱ����
	//m_pTreeList->SetFocus();
	m_pPropListCtrl->SetFocus();
}

///////////////////////////////////////////////////////////////////////
// ��    ʶ  : KmAps 3.0 bug10380 �������Ա�ǩҳ�е��Զ������Բ������á���ѡ�������ж��岽�������е��Զ�������Ҳ�������ã�������ӵ��Զ������Բ������ܱ��档
// ��������  : CProcessCustomPage::OnDeleteUseprop
// ��    ��  : 
// �� �� ֵ  : 
//      void -- 
// Ŀ��/���� : ɾ�����Զ�������
// ���ù�ϵ  : 
// �� �� ��  : ��˱�
// ��������  : 2009-10-23
// �����޸���:                        �޸����ڣ�
///////////////////////////////////////////////////////////////////////
void CProcessCustomPage::OnDeleteUseprop() 
{
	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	if (m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	//if(m_pTreeList->GetSelectedCount() == 0)
	if (m_pPropListCtrl->GetSelectionCount()==0)
		return;

	m_bDoModel = true;
	if (KmMessageBox(ResourceString(IDS_APM_DELUSERPROP, "ȷ��ɾ�����Զ������ԣ�"), ResourceString(IDS_APM_TIPS, APS_MSGBOX_TITIE), MB_OKCANCEL | MB_ICONQUESTION)
		== IDOK)
	{
		CAPSPropList useList;
		CAPSProp findProp;

		CList<CBCGPGridItem *, CBCGPGridItem *> arSel;
		m_pPropListCtrl->GetSelectedItems(arSel);
		POSITION pos = arSel.GetHeadPosition();
		while (pos != NULL)
		{
			CBCGPGridItem *pItem = arSel.GetNext(pos);
			CBCGPGridRow *pRow = pItem->GetParentRow();
			CString strProp = pRow->GetItem(0)->GetValue();
			POSITION pos2 = m_useList.SeekProp(strProp);
			if (pos2 != NULL)
			{
				m_useList.RemoveAt(pos2);
			}
		}
		//POSITION pos1 = m_pTreeList->GetFirstSelectedItemPosition();
		//while(pos1)
		//{
		//	int nItem = m_pTreeList->GetNextSelectedItem(pos1);
		//	CString strTemp = m_pTreeList->GetItemText(nItem, 0);

		//	POSITION pos2 = m_useList.SeekProp(strTemp);
		//	if(pos2 != NULL)
		//	{
		//		m_useList.RemoveAt(pos2);
		//	}
		//	/*
		//	POSITION pos = m_useList.GetHeadPosition();
		//	while (pos)
		//	{
		//		POSITION posTemp = pos;
		//		findProp = m_useList.GetNext(pos);
		//		if (findProp.GetName().CompareNoCase(strTemp) == 0 && posTemp)
		//		{
		//			useList.RemoveAt(posTemp);
		//			break;
		//		}
		//	}
		//	*/
		//}
		
		//m_pTreeList->DeleteAllItems();
		m_pPropListCtrl->RemoveAll();

		ReadProp();

		if(GetDlgItem(IDC_APM_EDIT_MUTILEVALUE) && GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->IsWindowVisible())
		{
			SetMutileEditShow(false);
			AdjustPlace();
		}

		SetModify(true);
		WriteProp();
	}

	m_bDoModel = false;

	//�Ա�ʧȥ����ʱ����
	//m_pTreeList->SetFocus();
	m_pPropListCtrl->SetFocus();
}


void CProcessCustomPage::OnUpdateUseprop() 
{
	// Bug 765 Add by �¹ں�
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// End Add by �¹ں�

	if (!m_pManager || !m_pProcess)
	{
		return;
	}

	CAPSPropList useList;
	CAPSProp findProp;

	//m_pAsmStep->m_useList.SeekProp(m_strUsePropName, &findProp);  
	m_useList.SeekProp(m_strUsePropName, &findProp);

	CAddUseProp dlgUseProp;
	CString strOrg = findProp.GetName();
	dlgUseProp.m_strName = findProp.GetName();
	dlgUseProp.SetSelType(findProp.GetType());
	dlgUseProp.m_strValue = findProp.ConvertTo();
	findProp.ModifyByType(_T(""));
	dlgUseProp.m_strDefaultVal = findProp.ConvertDefaultTo();

	m_bDoModel = true;

	if (dlgUseProp.DoModal() != IDOK)
	{
		m_bDoModel = false;
		return ;
	}

	m_bDoModel = false;

	//�Ա�ʧȥ����ʱ����
	//m_pTreeList->SetFocus();
	m_pPropListCtrl->SetFocus();


	bool bAdd = true;
	CAPSProp prop;
	if(/*m_pAsmStep->*/m_useList.SeekProp(dlgUseProp.m_strName, &prop) && dlgUseProp.m_strName.CompareNoCase(strOrg)!=0)
	{
		bAdd = false;

		m_bDoModel = true;
		KmMessageBox(ResourceString(IDS_APM_EXISTUSERPROP, "���Զ��������Ѵ���!"), APS_MSGBOX_TITIE, MB_ICONWARNING);
		m_bDoModel = false;
	}

	//�Ա�ʧȥ����ʱ����
	//m_pTreeList->SetFocus();
	m_pPropListCtrl->SetFocus();

	if (bAdd)
	{
		prop.SetName(dlgUseProp.m_strName);
		prop.SetType(dlgUseProp.GetSelType());
		prop.ModifyByType(dlgUseProp.m_strValue);
		switch(dlgUseProp.GetSelType())
		{
		case 0:
			break;
		case 1:
		case 2:
		case 3:
			::_tcscpy(prop.m_dftVal.m_str.GetBuffer(512), dlgUseProp.m_strDefaultVal);
			prop.m_dftVal.m_str.ReleaseBuffer();
			break;
			// 	case 2:
			// 		{
			// 			double dVal = _tstof((LPCTSTR)dlgUseProp.m_strDefaultVal);
			// 			prop.m_dftVal.m_d = dVal;	
			// 		}
			// 	    break;
			// 	case 3:
			// 		{
			// 			int iVal = _ttoi((LPCTSTR)dlgUseProp.m_strDefaultVal);
			// 			prop.m_dftVal.m_i = iVal;	
			// 		}
			// 	    break;
		default:
			break;
		}

		//m_pAsmStep->m_useList.ModifyProp(m_strUsePropName, prop);
		m_useList.ModifyProp(m_strUsePropName, prop);

		//m_pTreeList->DeleteAllItems();
		m_pPropListCtrl->RemoveAll();
		//m_useList.RemoveAll();
		//m_useList.AddTail( &(m_pAsmStep->m_useList) );
		ReadProp();

		SetModify(true);

		WriteProp();
	}
}


void CProcessCustomPage::AdjustPlace()
{
	CRect rect;
	GetWindowRect(rect);

	bool bVis = GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->IsWindowVisible();
	int cx = rect.Width();
	int cy = rect.Height();
	if(bVis)
	{
		//m_pTreeList->MoveWindow(5, 5, cx, cy/2+20);
		m_pPropListCtrl->MoveWindow(5, 5, cx, cy / 2 + 20);

		GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->MoveWindow(5, cy/2 +30, cx*0.6,24);

		//GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE)->MoveWindow(5+cx*0.6+5, cy/2 + 30,32,24);
		//GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE)->MoveWindow(5+cx*0.6+5+32+5, cy/2 + 30,32,24);

		CRect rect1,rect2;
		GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE)->GetWindowRect(rect1);
		GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE)->GetWindowRect(rect2);
		GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE)->MoveWindow(5+cx*0.6+5, cy/2 + 30,rect1.Width(),rect1.Height());
		GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE)->MoveWindow(5+cx*0.6+5+rect1.Width()+5, cy/2 + 30,rect2.Width(),rect2.Height());

		int i1 = cy-cy/2-30-24-5-5-30;
		GetDlgItem(IDC_APM_LIST_MUTILEVALUE)->MoveWindow(5,cy/2 +30+24+5, cx-10-10, i1);

		CWnd * pWnd = GetDlgItem(IDC_APM_CUSTOM_SAVE);
		if(pWnd)
		{
			pWnd->MoveWindow(cx- 62, cy - 30, 60, 24);
		}

		CWnd * pWndStatic = GetDlgItem(IDC_APM_PROJECTSYMBOL_STATIC);
		if(pWndStatic)
		{
			pWndStatic->MoveWindow(10, cy - 26, 60, 20);
		}
		CWnd * pWndC = GetDlgItem(IDC_APM_PROJECT_SYMBOL_C);
		if(pWndC)
		{
			pWndC->MoveWindow(70, cy - 30, 20, 20);
		}
		CWnd * pWndG = GetDlgItem(IDC_APM_PROJECT_SYMBOL_G);
		if(pWndG)
		{
			pWndG->MoveWindow(95, cy - 30, 20, 20);
		}
	}
	else
	{
		//m_pTreeList->MoveWindow(5, 5, cx-10, cy-10-30);
		m_pPropListCtrl->MoveWindow(5, 5, cx - 10, cy - 10 - 30);

		CWnd * pWnd = GetDlgItem(IDC_APM_CUSTOM_SAVE);
		if(pWnd)
		{
			pWnd->MoveWindow(cx- 62, cy - 30, 60, 24);
		}

		CWnd * pWndStatic = GetDlgItem(IDC_APM_PROJECTSYMBOL_STATIC);
		if(pWndStatic)
		{
			pWndStatic->MoveWindow(10, cy - 26, 60, 20);
		}
		CWnd * pWndC = GetDlgItem(IDC_APM_PROJECT_SYMBOL_C);
		if(pWndC)
		{
			pWndC->MoveWindow(70, cy - 30, 20, 20);
		}
		CWnd * pWndG = GetDlgItem(IDC_APM_PROJECT_SYMBOL_G);
		if(pWndG)
		{
			pWndG->MoveWindow(95, cy - 30, 20, 20);
		}
	}

	Invalidate();
}

CString CProcessCustomPage::GetFreePropName()
{
	CString re;

	for(int i = 1; i < 0x7fffffff; ++i)
	{
		CAPSProp prop;
		re.Format(ResourceString(IDS_APM_STEPACTION_PROPNAME,"����%d"), i);
		//re.Format(_T("����%d"), i);
		//if(m_pAsmStep->m_useList.SeekProp(re,&prop) == FALSE)
		if(m_useList.SeekProp(re,&prop) == FALSE)
		{
			break;
		}
	
	}
	return re;
}

void CProcessCustomPage::FormatStrByPropType(int type, CString & strInOut)
{
	switch(type)
	{
	case 1:
		break;
	case APSVT_DOUBLE: //��ֵ
		{
			double fL = _tstof(strInOut);
			//�Ƿ��ת��
			if(strInOut.CompareNoCase(ResourceString(IDS_APM_YES,"��")) == 0)
			{
				fL = 1.f;
			}
			else if(strInOut.CompareNoCase(ResourceString(IDS_APM_NO,"��")) == 0)
			{
				fL = 0.f;
			}
			strInOut.Format(_T("%g"), fL);
		}
		break;
	case APSVT_INT:
		{
			int n = _tstof(strInOut);
			//�Ƿ��ת��
			if(strInOut.CompareNoCase(ResourceString(IDS_APM_YES,"��")) == 0)
			{
				n = 1.f;
			}
			else if(strInOut.CompareNoCase(ResourceString(IDS_APM_NO,"��")) == 0)
			{
				n = 0.f;
			}

			strInOut.Format(_T("%d"), (int)n);
		}
		break;
	case APSVT_BOOL:
		{
			//����Ҫת��
			if(strInOut.CompareNoCase(ResourceString(IDS_APM_YES,"��")) == 0 || strInOut.CompareNoCase(ResourceString(IDS_APM_NO,"��")) == 0)
			{
				break;
			}

			//ת�����������ٴν����ж��Ƿ�
			int n = _tstoi(strInOut);

			if(n != 0)
				strInOut = ResourceString(IDS_APM_YES,"��");
			else
				strInOut = ResourceString(IDS_APM_NO,"��");
		}
		break;
	case APSVT_CHECK:
	{
		bool b = atoi(strInOut);
		//�Ƿ��ת��
		if (strInOut.CompareNoCase(ResourceString(IDS_APM_YES, "��")) == 0)
		{
			b = 1;
		}
		else if (strInOut.CompareNoCase(ResourceString(IDS_APM_NO, "��")) == 0)
		{
			b = 0;
		}
		strInOut.Format(_T("%d"), b);
		break;
	}
	}
}

//��ʾ������������Ϊ�б���Զ�������
void CProcessCustomPage::ShowTheList(int nCol)
{
	if (m_pProcess &&  nCol!= -1)// && pListView->iSubItem ==0)
	{		
		if(IsMutileType(nCol))
		{
			//m_strUsePropName = m_pTreeList->GetItemText(nCol,0);
			CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(nCol);
			m_strUsePropName = pRow->GetItem(0)->GetValue();

			m_nRowSel = nCol;

			//��ʾ������б�����
			SetMutileEditShow(true);
// 			GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->ShowWindow(SW_SHOW);
// 			GetDlgItem(IDC_APM_LIST_MUTILEVALUE)->ShowWindow(SW_SHOW);
// 			GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE)->ShowWindow(SW_SHOW);
// 			GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE)->ShowWindow(SW_SHOW);

			AdjustPlace();

			((CListBox*)GetDlgItem(IDC_APM_LIST_MUTILEVALUE))->ResetContent();

			UpdateMutileList();
		}
		else
		{
			//��ʾ������б�����
// 			GetDlgItem(IDC_APM_EDIT_MUTILEVALUE)->ShowWindow(SW_HIDE);
// 			GetDlgItem(IDC_APM_LIST_MUTILEVALUE)->ShowWindow(SW_HIDE);
// 			GetDlgItem(IDC_APM_BUTTON_MUTILEVALUE)->ShowWindow(SW_HIDE);
// 			GetDlgItem(IDC_APM_BUTTON_DEL_MUTILE_VALUE)->ShowWindow(SW_HIDE);
			SetMutileEditShow(false);
			AdjustPlace();
		}
	}
}

void CProcessCustomPage::InsertPropListItem(CAPSPropList & RefPropList)
{
	KmAPSTreeListCtrl::CItemInfo * pBaseSubItem = new CItemInfo;
	//m_ColumnItems;
	int index = 0;
	POSITION pos = m_ColumnItems.GetHeadPosition();
	while (pos != NULL)
	{
		const CAPSProp & tRefProp = m_ColumnItems.GetNext(pos);
		CString strValue = tRefProp.ConvertTo();

		CAPSProp prop;
		if(RefPropList.SeekDisplayNameProp(tRefProp.GetDisplayName(), &prop))
		{
			strValue = prop.ConvertTo();
		}
		else
		{
			RefPropList.AddTail(tRefProp);
		}

		if(tRefProp.GetAlias().IsEmpty())
		{
			pBaseSubItem->SetControlType(CItemInfo::edit, index);
		}
		else
		{
			pBaseSubItem->SetControlType(CItemInfo::edit, index);
			/*CStringList StrList;
			StrList.AddTail(strValue);
			StrList.AddTail(ResourceString(IDS_APM_RESDATA_SELFORMRESMAG, "*��Դ������"));
			pBaseSubItem->SetListData(index, &StrList);*/
		}
		if(tRefProp.GetType() == APSVT_BOOL)
		{
			pBaseSubItem->SetControlType(CItemInfo::combobox, index);
			CStringList strList;
			strList.AddTail(ResourceString(IDS_APM_YES,"��"));
			strList.AddTail(ResourceString(IDS_APM_NO,"��"));
			pBaseSubItem->SetListData(index,&strList);
		}

		if(index == 0)
			pBaseSubItem->SetItemText(strValue);
		else
			pBaseSubItem->AddSubItemText(strValue);

		index++;
	}

	//��Ӹ���
	//m_pTreeList->InsertRootItem(pBaseSubItem);
	//m_pPropListCtrl->InsertUsePropItem(prop);

}

void CProcessCustomPage::InsertUsePropItem(const CAPSProp & prop)
{
	CItemInfo * pBaseSubItem = new CItemInfo;

	pBaseSubItem->SetControlType(CItemInfo::edit, 0);

	pBaseSubItem->SetControlType(CItemInfo::edit, 1);
	if(prop.GetType() == APSVT_BOOL)
	{
		pBaseSubItem->SetControlType(CItemInfo::combobox, 1);
		CStringList StrList1;
		StrList1.AddTail(ResourceString(IDS_APM_YES,"��"));
		StrList1.AddTail(ResourceString(IDS_APM_NO,"��"));
		pBaseSubItem->SetListData(1, &StrList1);
	}

	pBaseSubItem->SetControlType(CItemInfo::combobox, 2);

	pBaseSubItem->SetItemText(prop.GetDisplayName()); // ����
	CString strPropVal = prop.m_Val.m_str;
	if(strPropVal.IsEmpty())
	{
		strPropVal = prop.m_dftVal.m_str;
	}
	pBaseSubItem->AddSubItemText(strPropVal);



	pBaseSubItem->AddSubItemText(prop.ConvertTypeTo());

	CStringList StrList2;
	StrList2.AddTail(ResourceString(IDS_APM_PROPTYPE_STRING, "�ַ���"));
	StrList2.AddTail(ResourceString(IDS_APM_PROPTYPE_NUMBER, "��ֵ"));
	StrList2.AddTail(ResourceString(IDS_APM_PROPTYPE_INT, "����"));
	StrList2.AddTail(ResourceString(IDS_APM_PROPTYPE_LIST, "�б�"));
	StrList2.AddTail(ResourceString(IDS_APM_PROPTYPE_LOGIC,"�߼�"));
	//begin add dm in 2019.04.17 for bug70905
	StrList2.AddTail(ResourceString(IDS_APM_PROPTYPE_CHECKBOX, "��ѡ��"));
	//end add
	pBaseSubItem->SetListData(2, &StrList2);

	if (!prop.GetEdit())
	{
		pBaseSubItem->SetControlType(CItemInfo::none, 0);
		pBaseSubItem->SetControlType(CItemInfo::none, 1);
		pBaseSubItem->SetControlType(CItemInfo::none, 2);
	}
	//m_pTreeList->InsertRootItem(pBaseSubItem);
	m_pPropListCtrl->InsertUsePropItem(prop);
	//if (iIndex >= 0 && iIndex < WIRING_BASEPROP_NUM)
	//{
	//	m_pGridCtrl->AddRow(pRow);
	//}
	//else
	//{
	//	m_pGridCtrl->InsertRowAfter(iIndex, pRow);
	//}
}

BOOL CProcessCustomPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	bool bDo = false;
	//try
	//{
	//	//��ʾΪTreeList����
	//	if((HWND)lParam == m_pTreeList->GetSafeHwnd())
	//	{
	//		bDo = true;
	//		//����հ״�,�����µ�����
	//		if ((HIWORD(wParam) == BN_CLICKED) && m_pProcess->GetAttrEdit())
	//		{

	//			//CTreeListCtrl::CTreeItem * pRootUse = static_cast<CTreeListCtrl::CTreeItem *>(m_RootUse);
	//			int nCol = LOWORD(wParam);
	//			if(nCol == 0/* && !m_pTreeList->IsCollapsed(pRootUse)*/)
	//			{
	//				SetModify(true);
	//				//����µ���б���
	//				CString strFreeName = GetFreePropName();
	//				CAPSProp prop(strFreeName, _T(""), strFreeName, APSVT_STR, _T(""));
	//				m_useList.AddTail(prop);
	//				InsertUsePropItem(prop);
	//				//m_pTreeList->RedrawWindow();
	//			}
	//		}
	//	}
	//}
	//catch(...)
	//{
	//	ASSERT(FALSE);
	//}

	return bDo ? bDo : CPropertyPage::OnCommand(wParam, lParam);
}

void CProcessCustomPage::OnBnClickedButtonC()
{
	CString str = _T("{0x19}901$$1$$C$$$");	
	if(m_nItemIndex >= 0 && m_nSubItemIndex == 1)
	{
		//if(m_nItemIndex <= m_pTreeList->GetItemCount())
		if (m_nItemIndex <= m_pPropListCtrl->GetRowCount())
		{
			//begin mod by dm
			//old
			//CString strVal = m_pTreeList->GetItemText(m_nItemIndex,m_nSubItemIndex);
			//strVal.Append(str);
			//m_pTreeList->SetItemText(m_nItemIndex, m_nSubItemIndex, strVal);
			//new
			CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(m_nItemIndex);
			CBCGPGridItem *pItem = pRow->GetItem(m_nSubItemIndex);
			if (pItem->GetRuntimeClass() == RUNTIME_CLASS(CBCGPGridCheckItem))
			{
				return;
			}
			CString strVal = pItem->GetValue();
			strVal.Append(str);
			pItem->SetValue(_variant_t(strVal));
			//end mod

			CAPSPropList & tmpPropList = m_useList;
			POSITION pos = tmpPropList.GetHeadPosition();
			while(pos)
			{
				CAPSProp &prop = tmpPropList.GetNext(pos);
				//if(prop.GetName() == m_pTreeList->GetItemText(m_nItemIndex,0))
				CString strName = pRow->GetItem(0)->GetValue();
				if (prop.GetName().CompareNoCase(strName) == 0)
				{
					prop.ModifyByType(strVal);
					//tmpPropList.ModifyProp(prop.GetName(), prop);
					SetModify(true);
					m_pProcess->SetModify();
					WriteProp();
					break;
				}
			}
		}
	}
}

void CProcessCustomPage::OnBnClickedButtonG()
{
	TRACE0("OnBnClickedButtonG\n");
	CString str = _T("{0x19}901$$1$$G$$$");	
	if(m_nItemIndex >= 0 && m_nSubItemIndex == 1)
	{
		//if(m_nItemIndex <= m_pTreeList->GetItemCount())
		if (m_nItemIndex <= m_pPropListCtrl->GetRowCount())
		{
			//begin mod by dm
			//old
			//CString strVal = m_pTreeList->GetItemText(m_nItemIndex,m_nSubItemIndex);
			//strVal.Append(str);
			//m_pTreeList->SetItemText(m_nItemIndex,m_nSubItemIndex,strVal);
			//TRACE0(strVal);//new
			CBCGPGridRow *pRow = m_pPropListCtrl->GetRow(m_nItemIndex);
			CBCGPGridItem *pItem = pRow->GetItem(m_nSubItemIndex);
			if (pItem->GetRuntimeClass() == RUNTIME_CLASS(CBCGPGridCheckItem))
			{
				return;
			}
			CString strVal = pItem->GetValue();
			strVal.Append(str);
			pItem->SetValue(_variant_t(strVal));
			//end mod
			CAPSPropList & tmpPropList = m_useList;
			POSITION pos = tmpPropList.GetHeadPosition();
			while(pos)
			{
				CAPSProp &prop = tmpPropList.GetNext(pos);
				//if(prop.GetName() == m_pTreeList->GetItemText(m_nItemIndex,0))
				CString strName = pRow->GetItem(0)->GetValue();
				if (prop.GetName().CompareNoCase(strName) == 0)
				{
					TRACE0("\n");
					TRACE0(prop.GetName());
					prop.ModifyByType(strVal);
					//tmpPropList.ModifyProp(prop.GetName(), prop);
					SetModify(true);
					m_pProcess->SetModify();
					WriteProp();
					break;
				}
			}
		}
	}
}

//void CProcessCustomPage::OnRButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
//	OnRclickListAsmstepDefine(point);
//
//	CKmPropertyPage::OnRButtonUp(nFlags, point);
//}

