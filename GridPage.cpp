
#include "stdafx.h"


#include "apsprocess.h"
#include "GridPage.h"
#include "APManager.h"
#include "AsmStep.h"
#include "AddUseProp.h"
#include "ResourceString.h"
#include "KmResViewProxy.h"

#include "../HistoryOp/HistoryOpMgr.h"
#include "HistoryAsmAction.h"
#include "UtilityFunction.h"

#include "APSModel.h"
#include "apsprocess.h"
#include "KmMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActionGridPage property page
#define ACT_IACTION 1
#define ACT_IPROCESS 2

IMPLEMENT_DYNCREATE(CActionGridPage, CResGridPage)

CActionGridPage::CActionGridPage()
{
	m_pAction = NULL;
	iAction = 1;
}

CActionGridPage::~CActionGridPage()
{

}

void CActionGridPage::InitColumn()
{
	CArray<CAPSPropList,CAPSPropList>* plistAry = NULL;
	int iType = 0;
	if(m_pAction)
	{
		plistAry = m_pAction->GetCurGridData(m_GridName);
	    UINT uID = m_pAction->GetID();
		if(::IsActionByID(uID))
		{
			iType = 1;
		}
		if(::IsAsmStep(uID))
		{
			iType = 2;
		}
		if(::IsAsmGroupID(uID))
		{
			iType = 3;
		}
	}
	m_ColumnItems.RemoveAll();
	if(iType == 1)
	{
		CProcess::GetActionGridHead(m_GridName,m_ColumnItems,m_pAction);
	}
	else if(iType ==2)
	{
		CProcess::GetStepGridHead(m_GridName,m_ColumnItems,m_pAction);
	}
	else if(iType ==3)
	{
		CProcess::GetGroupGridHead(m_GridName,m_ColumnItems,m_pAction);
	}
	CResGridPage::InitColumn();
}

void CActionGridPage::SetPropColumn(const CAPSPropList& proplist)
{
	m_ColumnItems.RemoveAll();

	POSITION pos = proplist.GetHeadPosition();
	while(pos)
	{
		const CAPSProp &prop = proplist.GetNext(pos);
		m_ColumnItems.AddTail(prop);
	}
}

void CActionGridPage::GetPropColumn(CActionBase* pAction, CAPSPropList& columnItems)
{
	CProcess::GetPropByID(RESOURCECONFIG, &columnItems);

	if (!pAction)
	{
		return;
	}

	CArray<CAPSPropList>* pResData = pAction->GetResourceDatas();
	if (pResData->GetSize() > 0)
	{
		const CAPSPropList& propList = pResData->GetAt(0);
		POSITION pos = propList.GetHeadPosition();
		while (pos)
		{
			const CAPSProp prop = propList.GetNext(pos);

			bool bFound = false;
			POSITION pos2 = columnItems.GetHeadPosition();
			while (pos2)
			{
				const CAPSProp propColumn = columnItems.GetNext(pos2);
				if (propColumn.GetDisplayName() == prop.GetDisplayName())
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				columnItems.AddTail(prop);
			}
		}
	}
}

void CActionGridPage::ReadProp(void)
{
	if (!m_pManager || !m_pAction)
	{
		return;
	}

	m_BackUpDatas.RemoveAll();

	CArray<CAPSPropList,CAPSPropList>* plistAry;
	plistAry = m_pAction->GetCurGridData(m_GridName);
	int cnt = 0;
	if(plistAry)
	{
		cnt = plistAry->GetCount();
	}
	for(int i=0;i<cnt;i++)
	{
		CAPSPropList proplist = plistAry ->GetAt(i);
		CAPSPropList proplist2;
		proplist2.Copy(proplist);
		m_BackUpDatas.Add(proplist2);
	}
	
	InitColumn();
	InitListCtrl();
}

void CActionGridPage::WriteProp(void)
{	
	if (!m_pManager || !m_pAction)
	{
		return;
	}

	if (m_pManager->m_iLaunchMode == EMBED_MODE)
	{
		return;
	}

	try
	{
		//正在回退过程中
		if (m_pManager->GetModel()->GetHistoryMgr()->GetNotify()) return;
		//HistoryOpMgr * pHistoryMgr = m_pManager->GetModel()->GetHistoryMgr();

		HistoryModifyData<CArray<CAPSPropList>> * pData = new HistoryModifyData<CArray<CAPSPropList>>(HISTORY_ACTION_MODIFYRESOURCE);
		//begin del by xieliang for bug 70499 保存网格数据为何要与resourcedata进行比较？这里导致了问题。而且不必要费时进行新旧比较判断，直接更新即可。
		//CArray<CAPSPropList> * pResData = m_pAction->GetResourceDatas();

		////begin cq in 2018.11.07 for bug 700018
		////pData->m_data.Copy(&pResData);
		//// 		pData->m_data.RemoveAll();
		///*
		//int cnt = 0;

		//if (pResData)
		//{
		//	cnt = pResData->GetCount();
		//}
		//for (int i = 0; i < cnt; i++)
		//{
		//	CAPSPropList proplist = pResData->GetAt(i);
		//	CAPSPropList pro;
		//	pro.Copy(proplist);
		//	pData->m_data.Add(pro);
		//}
		////比较新旧数据
		//bool bIsSame = true;
		//int propLen = pResData->GetCount();
		//for (int j = 0; j < propLen; ++j)
		//{
		//	CAPSPropList& pList = pResData->GetAt(j);
		//	POSITION pos = pList.GetHeadPosition();
		//	while (pos)
		//	{
		//		CAPSProp prop = pList.GetNext(pos);
		//		CString strAfterVal = prop.ConvertTo();

		//		int propBeforeLen = m_BackUpDatas.GetCount();
		//		for (int iBefore = 0; iBefore < propBeforeLen; ++iBefore)
		//		{
		//			CAPSPropList& pListBefore = m_BackUpDatas.GetAt(iBefore);
		//			POSITION posBefore = pListBefore.GetHeadPosition();
		//			while (posBefore)
		//			{
		//				CAPSProp propBefore = m_BackUpDatas.GetAt(iBefore).GetNext(posBefore);
		//				CString strBeforeVal = propBefore.ConvertTo();
		//				if (strAfterVal != strBeforeVal)
		//				{
		//					bIsSame = false;
		//				}
		//			}
		//		}
		//	}
		//}
		//*/
		////new
		//int propBeforeLen = m_BackUpDatas.GetCount();
		//int propLen = 0;
		//if (pResData)
		//{
		//	propLen = pResData->GetCount();

		//	for (int i = 0; i < propLen; i++)
		//	{
		//		CAPSPropList proplist = pResData->GetAt(i);
		//		CAPSPropList pro;
		//		pro.Copy(proplist);
		//		pData->m_data.Add(pro);
		//	}
		//}

		////比较新旧数据
		//bool bIsSame = true;
		//if (propLen == propBeforeLen)
		//{
		//	for (int j = 0; j < propLen; ++j)
		//	{
		//		if (!bIsSame)break;

		//		CAPSPropList& pResList = pResData->GetAt(j);
		//		POSITION pos = pResList.GetHeadPosition();
		//		while (pos)
		//		{
		//			CAPSProp resprop = pResList.GetNext(pos);
		//			CString strAfterVal = resprop.ConvertTo();

		//			for (int iBefore = 0; iBefore < propBeforeLen; ++iBefore)
		//			{
		//				CAPSPropList& pListBefore = m_BackUpDatas.GetAt(iBefore);
		//				POSITION posBefore = pListBefore.GetHeadPosition();
		//				while (posBefore)
		//				{
		//					CAPSProp propBefore = m_BackUpDatas.GetAt(iBefore).GetNext(posBefore);
		//					CString strBeforeVal = propBefore.ConvertTo();
		//					if (strAfterVal != strBeforeVal)
		//					{
		//						bIsSame = false;
		//						break;
		//					}
		//				}
		//			}

		//			if (!bIsSame)break;
		//		}
		//	}
		//}
		//else
		//{
		//	bIsSame = false;
		//}
		////end mod cq
		////如果不相同，才进行添加命令
		//if (!bIsSame)
		//end del by xieliang for bug 70499
		{
			HistoryModifyData<CAPSGridPropData> * pData1 = new HistoryModifyData<CAPSGridPropData>(HISTORY_ACTION_MODIFYRESOURCE);
			pData1->m_data.Copy(m_pAction->GetCurGrid(m_GridName));

			HistoryAsmActionPartModifyBase * pCmd = new HistoryAsmActionPartModifyBase(m_pAction->GetProcess(), m_pAction->GetID(), pData1, m_pManager->GetModel());
			m_pManager->GetModel()->AddCmd(pCmd);

			//m_pAction->SetResourceDatas(m_BackUpDatas);
			m_pAction->SetCurGridData(m_GridName, &m_BackUpDatas);
			m_pAction->SetCurGridTableHead(&m_ColumnItems, m_GridName);
			m_pManager->m_pProcess->SetModify();
		}
	}
	catch (...) 
	{
		//plh KMAPS3.0 选中装配过程节点时，点保存，死机。
		ASSERT(FALSE);
	}
}
//用于区分是活动还是process
int CActionGridPage::GetActType()
{
	return iAction;
}

void CActionGridPage::Update(CActionBase* pAction)
{
	m_bSave = false;
	// 更新之前先保存
	//OnBnClickedApmResSave();

	m_pAction = pAction;

	// KMAPS3.2 Add by 陈冠宏 2010-9-1	
	if (!IsWindow(this->GetSafeHwnd()))
	{
		return;
	}
	// End Add by 陈冠宏 2010-9-1

	// Bug 10141 KMAPS3.0 Add by 陈冠宏 2009-08-05
	// 使OnKillfocusEditBox被调用，结束编辑状态。
	//if (m_pManager->m_iLaunchMode != EMBED_MODE)
	//{
	//	this->SetFocus();
	//}
	// end Add by 陈冠宏 2009-08-05

	if(m_pManager && m_pAction)
	{
		ReadProp();
	}

	if (m_pAction!=NULL && IsAsmStep(m_pAction->GetID()))
	{
		bool bEnable = m_pAction->GetAttrEdit();
		CWnd *pWnd = GetDlgItem(IDC_APM_RES_ADD);
		if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
		{
			pWnd->EnableWindow(bEnable);
		}
		pWnd = GetDlgItem(IDC_APM_RES_DELETE);
		if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProcessGridPage property page

IMPLEMENT_DYNCREATE(CProcessGridPage, CResGridPage)

CProcessGridPage::CProcessGridPage() : CResGridPage()
{
	m_pProcess = NULL;
	iProcess = 2;
}

CProcessGridPage::~CProcessGridPage()
{

}

void CProcessGridPage::InitColumn()
{
	CArray<CAPSPropList,CAPSPropList>* plistAry = NULL;
	if(m_pProcess)
	{
		plistAry = m_pProcess->GetCurGridData(m_GridName);
	}

	m_ColumnItems.RemoveAll();	
	CProcess::GetProcessGridHead(m_GridName,m_ColumnItems,m_pProcess);

	CResGridPage::InitColumn();
}

void CProcessGridPage::GetPropColumn(CProcess* pProc, CAPSPropList& columnItems)
{
	CProcess::GetPropByID(RESOURCECONFIG, &columnItems);

	if (!pProc)
	{
		return;
	}

	CArray<CAPSPropList>* pResData = pProc->GetResourceDatas();
	if (pResData->GetSize() > 0)
	{
		const CAPSPropList& propList = pResData->GetAt(0);
		POSITION pos = propList.GetHeadPosition();
		while (pos)
		{
			const CAPSProp prop = propList.GetNext(pos);

			bool bFound = false;
			POSITION pos2 = columnItems.GetHeadPosition();
			while (pos2)
			{
				const CAPSProp propColumn = columnItems.GetNext(pos2);
				if (propColumn.GetDisplayName() == prop.GetDisplayName())
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				columnItems.AddTail(prop);
			}
		}
	}
}

int CProcessGridPage::GetActType()
{
	return iProcess;
}

void CProcessGridPage::SetPropColumn(const CAPSPropList& proplist)
{
	m_ColumnItems.RemoveAll();

	POSITION pos = proplist.GetHeadPosition();
	while(pos)
	{
		const CAPSProp &prop = proplist.GetNext(pos);
		m_ColumnItems.AddTail(prop);
	}
}

void CProcessGridPage::ReadProp(void)
{
	if (!m_pManager || !m_pProcess)
	{
		return;
	}
	m_BackUpDatas.RemoveAll();

	CArray<CAPSPropList,CAPSPropList>* plistAry;
	plistAry = m_pProcess->GetCurGridData(m_GridName);
	int cnt = 0;
	if(plistAry)
	{
		cnt = plistAry->GetCount();
	}
	for(int i=0;i<cnt;i++)
	{
		CAPSPropList proplist = plistAry ->GetAt(i);
		CAPSPropList proplist2;
		proplist2.Copy(proplist);
		m_BackUpDatas.Add(proplist2);
	}

	InitColumn();
	InitListCtrl();
}

void CProcessGridPage::WriteProp(void)
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
		//正在回退过程中
		if (m_pManager->GetModel()->GetHistoryMgr()->GetNotify()) return;
		//HistoryOpMgr * pHistoryMgr = m_pManager->GetModel()->GetHistoryMgr();

		HistoryModifyData<CArray<CAPSPropList>> * pData = new HistoryModifyData<CArray<CAPSPropList>>(HISTORY_ACTION_MODIFYRESOURCE);
		CArray<CAPSPropList> * pResData = m_pProcess->GetResourceDatas();
		// 		//pData->m_data.Copy(*pResData);
		// 		CArray<CAPSPropList,CAPSPropList> * pResData = m_pProcess->GetCurGridData(m_GridName);
		// 		pData->m_data.RemoveAll();

		//begin cq in 2018.11.07 for bug 700018
		/*
		int cnt = 0;
		// 
		// 		if(pResData)
		// 		{
		// 			cnt = pResData->GetCount();
		// 		}
		// 		for(int i=0;i<cnt;i++)
		// 		{
		// 			CAPSPropList proplist = pResData->GetAt(i);
		// 			CAPSPropList pro;
		// 			pro.Copy(proplist);
		// 			pData->m_data.Add(pro);
		// 		}

		if (pResData)
		{
			cnt = pResData->GetCount();
		}
		for (int i = 0; i < cnt; i++)
		{
			CAPSPropList proplist = pResData->GetAt(i);
			CAPSPropList pro;
			pro.Copy(proplist);
			pData->m_data.Add(pro);
		}
		//比较新旧数据
		bool bIsSame = true;
		int propLen = pResData->GetCount();
		for (int j = 0; j < propLen; ++j)
		{
			CAPSPropList& pList = pResData->GetAt(j);
			POSITION pos = pList.GetHeadPosition();
			while (pos)
			{
				CAPSProp prop = pList.GetNext(pos);
				CString strAfterVal = prop.ConvertTo();

				int propBeforeLen = m_BackUpDatas.GetCount();
				for (int iBefore = 0; iBefore < propBeforeLen; ++iBefore)
				{
					CAPSPropList& pListBefore = m_BackUpDatas.GetAt(iBefore);
					POSITION posBefore = pListBefore.GetHeadPosition();
					while (posBefore)
					{
						CAPSProp propBefore = m_BackUpDatas.GetAt(iBefore).GetNext(posBefore);
						CString strBeforeVal = propBefore.ConvertTo();
						if (strAfterVal != strBeforeVal)
						{
							bIsSame = false;
						}
					}
				}
			}
		}
		*/
		//new
		int propBeforeLen = m_BackUpDatas.GetCount();
		int propLen = 0;
		if (pResData)
		{
			propLen = pResData->GetCount();

			for (int i = 0; i < propLen; i++)
			{
				CAPSPropList proplist = pResData->GetAt(i);
				CAPSPropList pro;
				pro.Copy(proplist);
				pData->m_data.Add(pro);
			}
		}

		//比较新旧数据
		bool bIsSame = true;
		if (propLen == propBeforeLen)
		{
			for (int j = 0; j < propLen; ++j)
			{
				if (!bIsSame)break;

				CAPSPropList& pResList = pResData->GetAt(j);
				POSITION pos = pResList.GetHeadPosition();
				while (pos)
				{
					CAPSProp resprop = pResList.GetNext(pos);
					CString strAfterVal = resprop.ConvertTo();

					for (int iBefore = 0; iBefore < propBeforeLen; ++iBefore)
					{
						CAPSPropList& pListBefore = m_BackUpDatas.GetAt(iBefore);
						POSITION posBefore = pListBefore.GetHeadPosition();
						while (posBefore)
						{
							CAPSProp propBefore = m_BackUpDatas.GetAt(iBefore).GetNext(posBefore);
							CString strBeforeVal = propBefore.ConvertTo();
							if (strAfterVal != strBeforeVal)
							{
								bIsSame = false;
								break;
							}
						}
					}

					if (!bIsSame)break;
				}
			}
		}
		else
		{
			bIsSame = false;
		}
		//end mod cq
		

		//如果不相同，才进行添加命令
		if (!bIsSame)
		{
			HistoryModifyData<CAPSGridPropData> * pData1 = new HistoryModifyData<CAPSGridPropData>(HISTORY_ACTION_MODIFYRESOURCE);

			pData1->m_data.Copy(m_pProcess->GetCurGrid(m_GridName));

			HistoryProcessModifyBase * pCmd = new HistoryProcessModifyBase(m_pProcess, pData1);
			m_pManager->GetModel()->AddCmd(pCmd);

			m_pProcess->SetCurGridData(m_GridName, &m_BackUpDatas);
			m_pProcess->SetCurGridTableHead(&m_ColumnItems, m_GridName);
			m_pManager->m_pProcess->SetModify();
		}
	}
	catch (...) 
	{
		//plh KMAPS3.0 选中装配过程节点时，点保存，死机。
		ASSERT(FALSE);
	}
}

void CProcessGridPage::Update(CProcess * pProcess)
{
	m_bSave = false;
	// 更新之前先保存
	//OnBnClickedApmResSave();

	m_pProcess = pProcess;

	// KMAPS3.2 Add by 陈冠宏 2010-9-1	
	if (!IsWindow(this->GetSafeHwnd()))
	{
		return;
	}
	// End Add by 陈冠宏 2010-9-1

	// Bug 10141 KMAPS3.0 Add by 陈冠宏 2009-08-05
	// 使OnKillfocusEditBox被调用，结束编辑状态。
	if (m_pManager->m_iLaunchMode != EMBED_MODE)
	{
		this->SetFocus();
	}
	// end Add by 陈冠宏 2009-08-05

	if(m_pManager && m_pProcess)
	{
		ReadProp();
		//AdjustPlace();
	}

	if (m_pProcess != NULL)
	{
		bool bEnable = m_pProcess->GetAttrEdit();
		CWnd *pWnd = GetDlgItem(IDC_APM_RES_ADD);
		if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
		{
			pWnd->EnableWindow(bEnable);
		}
		pWnd = GetDlgItem(IDC_APM_RES_DELETE);
		if (pWnd != NULL && IsWindow(pWnd->GetSafeHwnd()))
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//CResGridPage

CResGridPage::CResGridPage() : CGridPage(CResGridPage::IDD, IDS_APM_PROPCAPTION_RESOURCE)
{

}

void CResGridPage::InitColumn()
{
	if(!m_pGridCtrl)
	{
		ASSERT(false);
		return ;
	}

	int index = 0;

	POSITION pos = m_ColumnItems.GetHeadPosition();
	while (pos != NULL)
	{
		const CAPSProp& prop = m_ColumnItems.GetNext(pos);
		if (!prop.GetShow())
		{
			continue;
		}
		if (!prop.GetVisInProplistWnd())
		{
			continue;
		}

		//begin mod by dm
		//old
		//m_pGridCtrl->InsertColumn(index++, prop.GetDisplayName(), 80);
		//new
		int iType = this->GetActType();
		CString strRegpath;
		if (iType == ACT_IACTION)
		{
			CActionGridPage* pGrid = (CActionGridPage*)this;
			strRegpath = IsHaveResMgrPath(NULL, pGrid->GetAction(), pGrid->GetGridName(), prop.GetName());
		}
		else
		{
			CProcessGridPage* pGrid = (CProcessGridPage*)this;
			strRegpath = IsHaveResMgrPath(pGrid->GetProcess(), NULL, pGrid->GetGridName(), prop.GetName());
		}
		//目前只有中文版支持管理器功能。故添加限制 2014.05.19 for bug2144
		if (!strRegpath.IsEmpty() && KmAppSettings::GetAppLanguage() == APS_LANG_CHS)
		{
			m_pGridCtrl->InsertColumn(index++, prop.GetDisplayName(), 80, 0, FALSE);
		}
		else
		{
			m_pGridCtrl->InsertColumn(index++, prop.GetDisplayName(), 80);
		}
		//end mod
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGridPage property page

CGridPage::CGridPage()
{
	m_BVis = TRUE;
	m_bSave = true;
	m_bOpenKmRefManager = false;
	m_bModified = false;

	m_pManager = NULL;
	m_pGridCtrl = NULL;

}

CGridPage::CGridPage(UINT uID, UINT uCaptionID) : CKmPropertyPage(uID, uCaptionID/*CGridPage::IDD, IDS_APM_PROPCAPTION_RESOURCE*/)
{
	m_BVis = TRUE;
	m_bSave = true;
	m_bOpenKmRefManager = false;
	m_bModified = false;

	m_pManager = NULL;
	m_pGridCtrl = NULL;
}

CGridPage::~CGridPage()
{
	m_bSave = false;
	// 析构之前先保存
	//OnBnClickedApmResSave();
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}

	m_ImageList.DeleteImageList();
}

void CGridPage::DoDataExchange(CDataExchange* pDX)
{
	CKmPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APM_RES_DELETE, m_DeleteItemBtn);
	DDX_Control(pDX, IDC_APM_RES_ADD, m_AddItemBtn);
	//DDX_Control(pDX, IDC_APM_RES_CANCEL, m_CancelBtn);
	DDX_Control(pDX, IDC_APM_RES_SAVE, m_SaveBtn);
	DDX_Control(pDX, IDC_APM_RES_MOVEUP, m_MoveUpBtn);
	DDX_Control(pDX, IDC_APM_RES_MOVEDOWN, m_MoveDownBtn);
}


BEGIN_MESSAGE_MAP(CGridPage, CKmPropertyPage)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_APM_RES_ADD, &CGridPage::OnBnClickedApmResAdd)
	ON_BN_CLICKED(IDC_APM_RES_DELETE, &CGridPage::OnBnClickedApmResDelete)
	ON_BN_CLICKED(IDC_APM_RES_MOVEUP, &CGridPage::OnBnClickedApmResMoveUp)
	ON_BN_CLICKED(IDC_APM_RES_MOVEDOWN, &CGridPage::OnBnClickedApmResMoveDown)
	//ON_BN_CLICKED(IDC_APM_RES_CANCEL, &CGridPage::OnBnClickedApmResCancel)
	ON_BN_CLICKED(IDC_APM_RES_SAVE, &CGridPage::OnBnClickedApmResSave)
	ON_REGISTERED_MESSAGE(BCGM_GRID_SEL_CHANGED, OnBCGGridSelChanged)
		
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridPage message handlers
BOOL CGridPage::OnInitDialog()
{
	CKmPropertyPage::OnInitDialog();

 	m_ImageList.DeleteImageList();
 	m_ImageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 1, 2);
 	CBitmap bm;
 	bm.LoadBitmap(IDB_APM_HEADERIMAGE);
 	m_ImageList.Add(&bm, RGB(192, 192, 192));
	DeleteObject(bm);

	CRect rect;
	GetClientRect(&rect);

	//释放一次
	SafeDelete(m_pGridCtrl);
	m_pGridCtrl = new CBCGPGridCtrl;
	if (m_pGridCtrl)
	{
		m_pGridCtrl->SetVisualManagerColorTheme(FALSE);
		m_pGridCtrl->Create(WS_CHILD | WS_BORDER | WS_VISIBLE, rect, this, 1);

		m_pGridCtrl->SetClearInplaceEditOnEnter(FALSE);
		m_pGridCtrl->EnableInvertSelOnCtrl();
		m_pGridCtrl->EnableHeader(TRUE, 0);
		//EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS | BCGP_GRID_HEADER_SORT);
		//EnableMarkSortedColumn(TRUE, FALSE);
		m_pGridCtrl->EnableMarkSortedColumn(FALSE);
		m_pGridCtrl->EnableRowHeader(FALSE);

		//begin add by dm
		m_pGridCtrl->SetHeaderImageList(&m_ImageList); 
		m_pGridCtrl->SetSingleSel(TRUE);
		//end add
	}

	InitColumn();
	InitListCtrl();

	//加、减、上下移、保存图标初始化
	//begin mod cq in 2018.11.09
	/*
	CBitmap BitmapAdd;
	BitmapAdd.LoadBitmap(IDB_APM_PAGEADD);
	HBITMAP hBitmapAdd = (HBITMAP)BitmapAdd.Detach();
	m_AddItemBtn.SetBitmap(hBitmapAdd);

	CBitmap BitmapDelete;
	BitmapDelete.LoadBitmap(IDB_APM_PAGEDELETE);
	HBITMAP hBitmapDelete = (HBITMAP)BitmapDelete.Detach();
	m_DeleteItemBtn.SetBitmap(hBitmapDelete);

	CBitmap BitmapMoveUp;
	BitmapMoveUp.LoadBitmap(IDB_APM_PAGEMOVEUP);
	HBITMAP hBitmapMoveUp = (HBITMAP)BitmapMoveUp.Detach();
	m_MoveUpBtn.SetBitmap(hBitmapMoveUp);

	CBitmap BitmapMoveDown;
	BitmapMoveDown.LoadBitmap(IDB_APM_PAGEMOVEDOWN);
	HBITMAP hBitmapMoveDown = (HBITMAP)BitmapMoveDown.Detach();
	m_MoveDownBtn.SetBitmap(hBitmapMoveDown);

	CBitmap BitmapSave;
	BitmapSave.LoadBitmap(IDB_APM_PAGESAVE);
	HBITMAP hBitmapSave = (HBITMAP)BitmapSave.Detach();
	m_SaveBtn.SetBitmap(hBitmapSave);
	BitmapSave.DeleteObject();
	*/
	//new

	CBitmap BitmapAdd, BitmapDelete, BitmapMoveUp, BitmapMoveDown, BitmapSave;
	BitmapAdd.LoadBitmap(IDB_APM_PAGEADD);
	BitmapDelete.LoadBitmap(IDB_APM_PAGEDELETE);
	BitmapMoveUp.LoadBitmap(IDB_APM_PAGEMOVEUP);
	BitmapMoveDown.LoadBitmap(IDB_APM_PAGEMOVEDOWN);
	BitmapSave.LoadBitmap(IDB_APM_PAGESAVE);

	CImageList normallist;
	normallist.Create(16, 16, ILC_COLOR16 | ILC_MASK, 5, 1);
	normallist.Add(&BitmapAdd, RGB(192, 192, 192));
	normallist.Add(&BitmapDelete, RGB(192, 192, 192));
	normallist.Add(&BitmapMoveUp, RGB(192, 192, 192));
	normallist.Add(&BitmapMoveDown, RGB(192, 192, 192));
	normallist.Add(&BitmapSave, RGB(192, 192, 192));

    //begin mod by dm in 20190529
    //old
    //m_AddItemBtn.SetIcon(normallist.ExtractIcon(0));
	//m_DeleteItemBtn.SetIcon(normallist.ExtractIcon(1));
	//m_MoveUpBtn.SetIcon(normallist.ExtractIcon(2));
	//m_MoveDownBtn.SetIcon(normallist.ExtractIcon(3));
	//m_SaveBtn.SetIcon(normallist.ExtractIcon(4));

	//normallist.Detach();
    //new
	DeleteObject(BitmapAdd);
	DeleteObject(BitmapDelete);
	DeleteObject(BitmapMoveUp);
	DeleteObject(BitmapMoveDown);
	DeleteObject(BitmapSave);

	HICON hIcon = normallist.ExtractIcon(0);
	m_AddItemBtn.SetIcon(hIcon);
	DestroyIcon(hIcon);

	hIcon = normallist.ExtractIcon(1);
	m_DeleteItemBtn.SetIcon(hIcon);
	DestroyIcon(hIcon);

	hIcon = normallist.ExtractIcon(2);
	m_MoveUpBtn.SetIcon(hIcon);
	DestroyIcon(hIcon);

	hIcon = normallist.ExtractIcon(3);
	m_MoveDownBtn.SetIcon(hIcon);
	DestroyIcon(hIcon);

	hIcon = normallist.ExtractIcon(4);
	m_SaveBtn.SetIcon(hIcon);
	DestroyIcon(hIcon);

	normallist.DeleteImageList();
	//end

	AdjustPlace();
	//end_add

	return TRUE;
}

void CGridPage::OnDestroy()
{
	if (m_pGridCtrl)
	{
		if (IsWindow(m_pGridCtrl->GetSafeHwnd()))
		{
			m_pGridCtrl->DestroyWindow();
		}

		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

// BOOL CGridPage::OnEraseBkgnd(CDC* pDC)
// {
// 	if (pDC)
// 	{
// 		CRect rw;
// 		GetWindowRect(&rw);
// 		pDC->FillSolidRect(rw, ::GetSysColor(COLOR_3DFACE));
// 	}
// 
// 	return TRUE;
// }

//网格表头信息与KMAPS选项中的设置信息相匹配，如果选项配置中设置了资源管理器路径，则return 资源管理器路径
CString CGridPage::IsHaveResMgrPath(CProcess* pProcss,CActionBase* pAction,CString strGridName,CString strPropName)
{
	CString str;
	if(pProcss)
	{
	//	CArray<CAPSPropList,CAPSPropList>* plistAry = pProcss->GetCurGridData(strGridName);
		//UINT uID = pAction->GetID();
	
			for(int i=0;i<CProcess::s_processGridPropAry.GetCount();i++)
			{
				CAPSGridPropConfig propconfig = CProcess::s_processGridPropAry.GetAt(i);
				if(strGridName == propconfig.GetGridName())
				{
					const CAPSPropList proplist = propconfig.GetPropList();
					POSITION pos = proplist.GetHeadPosition();
					while(pos)
					{
						CAPSProp prop = proplist.GetNext(pos);
						if(prop.GetName() == strPropName)
						{
							return prop.GetRegMgrPath();
						}
					}
				}
			}
			return str;
	}
	if(pAction)
	{
		//CArray<CAPSPropList,CAPSPropList>* plistAry = pAction->GetCurGridData(strGridName);
		UINT uID = pAction->GetID();
		if(::IsActionByID(uID))
		{
			for(int i=0;i<CProcess::s_actionGridPropAry.GetCount();i++)
			{
				CAPSGridPropConfig propconfig = CProcess::s_actionGridPropAry.GetAt(i);
				if(strGridName == propconfig.GetGridName())
				{
					const CAPSPropList proplist = propconfig.GetPropList();
					POSITION pos = proplist.GetHeadPosition();
					while(pos)
					{
						CAPSProp prop = proplist.GetNext(pos);
						if(prop.GetName() == strPropName)
						{
							return prop.GetRegMgrPath();
						}
					}
				}
			}
		}
		if(::IsAsmStep(uID))
		{
			if (((CAsmStep *)pAction)->IsAsmSubStep())
			{
				for(int i=0;i<CProcess::s_substepGridPropAry.GetCount();i++)
				{
					CAPSGridPropConfig propconfig = CProcess::s_substepGridPropAry.GetAt(i);
					if(strGridName == propconfig.GetGridName())
					{
						const CAPSPropList proplist = propconfig.GetPropList();
						POSITION pos = proplist.GetHeadPosition();
						while(pos)
						{
							CAPSProp prop = proplist.GetNext(pos);
							if(prop.GetName() == strPropName)
							{
								return prop.GetRegMgrPath();
							}
						}
					}
				}
			}
			else
			{
				for(int i=0;i<CProcess::s_stepGridPropAry.GetCount();i++)
				{
					CAPSGridPropConfig propconfig = CProcess::s_stepGridPropAry.GetAt(i);
					if(strGridName == propconfig.GetGridName())
					{
						const CAPSPropList proplist = propconfig.GetPropList();
						POSITION pos = proplist.GetHeadPosition();
						while(pos)
						{
							CAPSProp prop = proplist.GetNext(pos);
							if(prop.GetName() == strPropName)
							{
								return prop.GetRegMgrPath();
							}
						}
					}
				}
			}
		}
		if(::IsAsmGroupID(uID))
		{
			for(int i=0;i<CProcess::s_groupGridPropAry.GetCount();i++)
			{
				CAPSGridPropConfig propconfig = CProcess::s_groupGridPropAry.GetAt(i);
				if(strGridName == propconfig.GetGridName())
				{
					const CAPSPropList proplist = propconfig.GetPropList();
					POSITION pos = proplist.GetHeadPosition();
					while(pos)
					{
						CAPSProp prop = proplist.GetNext(pos);
						if(prop.GetName() == strPropName)
						{
							return prop.GetRegMgrPath();
						}
					}
				}
			}
		}
		return str;
	}
	return str;
}

void CGridPage::FormatStrByPropType(int type, CString & strInOut)
{
	switch(type)
	{
	case 1:
		break;
	case APSVT_DOUBLE: //数值
		{
			double fL = _tstof(strInOut);
			strInOut.Format(_T("%g"), fL);
		}
		break;
	case APSVT_INT:
		{
			int n = _tstoi(strInOut);
			strInOut.Format(_T("%d"), n);
		}
	case APSVT_BOOL:
		{
		}
		break;
	}
}

void CGridPage::OnSize(UINT nType, int cx, int cy)
{
	//CKmPropertyPage::OnSize(nType, cx, cy);
	AdjustPlace();
}


//在表头中查找该列是否显示
bool CGridPage::GetColShow(int nCol)
{
	int nCount = m_ColumnItems.GetCount();
	if (nCol<0||nCol>=nCount)
	{
		return false;
	}
	
	POSITION pos = m_ColumnItems.FindIndex(nCol);
	if (pos)
	{
		const CAPSProp & prop = m_ColumnItems.GetNext(pos);
		if (prop.GetShow() && prop.GetVisInProplistWnd())
		{
			return true;
		}
	}

	return false;
}

int CGridPage::GetColByCtrlCol(int nCtrlCol)
{
	if (nCtrlCol < 0)
		return -1;

	int nCol = -1;
	int nShowCol = -1;
	POSITION pos = m_ColumnItems.GetHeadPosition();
	while (pos != NULL)
	{
		++nCol;
		const CAPSProp & tRefProp = m_ColumnItems.GetNext(pos);
		if (tRefProp.GetShow())
		{
			++nShowCol;
			if (nShowCol == nCtrlCol)
			{
				return nCol;
			}
		}
	}

	return -1;
}

void CGridPage::InitListCtrl()
{
	if(!m_pGridCtrl)
	{
		ASSERT(false);
		return ;
	}

	//if(m_pAsmStep == NULL) return ;

	m_pGridCtrl->RemoveAll();
	
	int nCount = m_BackUpDatas.GetCount();
	for (int i = 0; i < nCount; ++i)
	{
		CAPSPropList & tRefPropList = m_BackUpDatas.GetAt(i);

		InsertPropListItem(tRefPropList);
	}
}

CBCGPGridRow *CGridPage::InsertPropListItem(CAPSPropList& listAPSProp, int iPos)
{
	int iCol = 0;
	std::vector<CAPSProp> vecCol;
	vecCol.reserve(m_ColumnItems.GetSize());

	POSITION pos = m_ColumnItems.GetHeadPosition();
	while (pos != NULL)
	{
		const CAPSProp& stAPSProp = m_ColumnItems.GetNext(pos);

		bool bShow = GetColShow(iCol++);
		if (!bShow)
		{
			continue;
		}

		vecCol.push_back(stAPSProp);
	}

	const int iColCnt = vecCol.size();

	//添加该项,列数为动态
	CBCGPGridRow* pNewRow = m_pGridCtrl->CreateRow(iColCnt);
	CString strValue;
	for (int i = 0; i < iColCnt; ++i)
	{
		CBCGPGridItem* pItem = pNewRow->GetItem(i);
		if (!pItem)
		{
			continue;
		}

		const CAPSProp& stProp = vecCol.at(i);
		strValue = stProp.ConvertTo();

		CAPSProp stTempProp;
		if (listAPSProp.SeekDisplayNameProp(stProp.GetDisplayName(), &stTempProp))
		{
			strValue = stTempProp.ConvertTo();
		}
		else
		{
			listAPSProp.AddTail(stProp);
		}
		pItem->SetValue(_variant_t(strValue), FALSE);
		pItem->AllowEdit(TRUE);
		pItem->Enable(TRUE);
		pItem->SetReadOnly(FALSE);
		
		// bool类型加combox
		if (stProp.GetType() == APSVT_BOOL)
		{
			pItem->AddOption(ResourceString(IDS_APM_YES, "是"));
			pItem->AddOption(ResourceString(IDS_APM_NO, "否"));
			pItem->SetReadOnly(TRUE);
		}
		// 列表型加combox
		else if (stProp.GetType() == APSVT_MUTILE)
		{
			CString strVal = stProp.ConvertDefaultTo();

			//处理字符串
			int iFind = strVal.Find("|");
			int iLen = strVal.GetLength();
			CString strPropDefault = strVal.Left(iFind);
			pItem->SetValue(_variant_t(strPropDefault), FALSE);

			strValue = strVal.Right(iLen - iFind - 1);
			CString strRealValue;
			while (strValue.GetLength() != 0)
			{
				iFind = strValue.Find(";");
				strRealValue = strValue.Left(iFind);
				if (strRealValue != "")
				{
					pItem->AddOption(strRealValue);
				}
				if (iFind == -1)
				{
					pItem->AddOption(strValue);
					break;
				}
				iLen = strValue.GetLength();
				strValue = strValue.Right(iLen - iFind - 1);
			}
			pItem->SetReadOnly(TRUE);
		}
		//begin add by dm in 2019.04.23 for bug70905,bug70909
		else if (stProp.GetType() == APSVT_CHECK)
		{
			bool bVal = atoi(strValue);

			pItem = new CBCGPGridCheckItem(bVal);
			pNewRow->ReplaceItem(i,pItem);
		}	
		//end add
		else if (!stProp.GetRegMgrPath().IsEmpty())
		{
			pItem->SetReadOnly(TRUE);
			pItem->AllowEdit(FALSE);
		}

		if (!stProp.GetEdit())
		{
			pItem->AllowEdit(FALSE);
		}
	}

	
	if (-1 == iPos)
	{
		m_pGridCtrl->AddRow(pNewRow);
	}
	else
	{
		m_pGridCtrl->InsertRowBefore(iPos, pNewRow);
	}

	return pNewRow;
}

void CGridPage::SaveData()
{
	if (!IsWindowVisible() || !m_pGridCtrl)
	{
		return;
	}
	// 读取数据保存
	int iDataCnt = m_BackUpDatas.GetSize();
	if (m_pGridCtrl->GetRowCount() != iDataCnt)
	{
		return;
	}

	// 获取列名
	std::vector<CString> vecColName;
	for (int i = 0; i < m_pGridCtrl->GetColumnCount(); ++i)
	{
		vecColName.push_back(m_pGridCtrl->GetColumnName(i));
	}

	for (int iRow = 0; iRow < iDataCnt; ++iRow)
	{
		CBCGPGridRow* pRow = m_pGridCtrl->GetRow(iRow);

		CAPSPropList& listProp = m_BackUpDatas.GetAt(iRow);

		for (int iCol = 0; iCol < vecColName.size(); ++iCol)
		{
			CBCGPGridItem* pItem = pRow->GetItem(iCol);
			POSITION pos = listProp.FindProp(vecColName[iCol]);
			if (!pos)
			{
				continue;
			}
			CAPSProp& stProp = listProp.GetAt(pos);

			CString strValue(pItem->GetValue());
			//begin add by dm in 2019.04.17 for bug70905
			if (CBCGPGridCheckItem* pCheck = dynamic_cast<CBCGPGridCheckItem*>(pItem))
			{
				int nState = pCheck->GetState();
				strValue.Format("%d",nState);
			}	
			//end add
			
			stProp.ModifyByType(strValue);
			//stProp.SetDisplayName(pItem->GetValue());
		}
	}

	WriteProp();
}

BOOL CGridPage::IsSamePropList(CAPSPropList & PropList1, CAPSPropList & PropList2)
{
	if(PropList1.GetCount() != PropList2.GetCount()) return FALSE;

	POSITION pos1 = NULL, pos2 = NULL;
	pos1 = PropList1.GetHeadPosition();
	pos2 = PropList2.GetHeadPosition();
	while(pos1 && pos2)
	{
		const CAPSProp & prop1 = PropList1.GetNext(pos1);
		const CAPSProp & prop2 = PropList2.GetNext(pos2);

		if(prop1.GetName() != prop2.GetName())
		{
			return FALSE;
		}

		bool b1 = prop1.GetAlias().IsEmpty();
		bool b2 = prop2.GetAlias().IsEmpty();
		if(b1 != b2)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CGridPage::SetAPManager(CAPManager* pManager)
{
	m_pManager = pManager;
}

CString CGridPage::GetFreePropName()
{
	CString re;

	for(int i = 1; i < 0x7fffffff; ++i)
	{
		re.Format(ResourceString(IDS_APM_RESOURCE_FREE_PROP_NAME, "列项%d"), i);
		if(IsFreePropName(re))
		{
			break;
		}
	}

	return re;
}

BOOL CGridPage::IsFreePropName(const CString & name, int nIgnoreIndex/* = -1*/)
{
	BOOL re = TRUE;

	int nIndex = 0;
	for(int i = 0; i < m_BackUpDatas.GetCount(); ++i)
	{
		CAPSPropList & tRefPropList = m_BackUpDatas[i];

		POSITION pos = tRefPropList.GetHeadPosition();
		const CAPSProp & prop = tRefPropList.GetNext(pos);

		if(name.CompareNoCase(prop.ConvertTo()) == 0)
		{
			re = FALSE;
			break;
		}
	}

	return re;
}

void CGridPage::OnBnClickedApmResDelete()
{
	if (m_pGridCtrl)
	{
		RemoveAllSelRows();
		m_pGridCtrl->SetFocus();
		m_bModified = true;
	}
}

void CGridPage::OnBnClickedApmResAdd()
{
	CString strRegpath;
	CString strFreeName;// = GetFreePropName();
	CString strResMgrPath;

	bool bOk = false;
	CList<KmResViewProxy::ResViewFieldRecord, KmResViewProxy::ResViewFieldRecord> fieldSet;

	//int nType;
	if (POSITION pos = m_ColumnItems.GetHeadPosition())
	{
		const CAPSProp & prop = m_ColumnItems.GetAt(pos);

		int iType = this->GetActType();

		if (iType == ACT_IACTION)
		{
			CActionGridPage* pGrid = (CActionGridPage*)this;
			strRegpath = IsHaveResMgrPath(NULL, pGrid->GetAction(), pGrid->GetGridName(), prop.GetName());
		}
		else
		{
			CProcessGridPage* pGrid = (CProcessGridPage*)this;
			strRegpath = IsHaveResMgrPath(pGrid->GetProcess(), NULL, pGrid->GetGridName(), prop.GetName());
		}



		//分析出管理器路径
		GetResMgrInfo(strRegpath, strResMgrPath);

		if (strRegpath.IsEmpty())
		{
			strFreeName = prop.ConvertDefaultTo();

			if (strFreeName.IsEmpty())
			{
				strFreeName = GetFreePropName();
				FormatStrByPropType(prop.GetType(), strFreeName);
			}
		}
		else
		{
			strFreeName = prop.ConvertDefaultTo();

			//目前只有中文版支持管理器功能。故添加限制 2014.05.19 for bug2144
			if (KmAppSettings::GetAppLanguage() == APS_LANG_CHS)
			{
				m_bOpenKmRefManager = true;

				KmResViewProxy kmResView;
				kmResView.SetParentWnd(m_hWnd);
				kmResView.SetRootNodeName(strResMgrPath);
				if (kmResView.DoModal())
				{
					strFreeName = kmResView.GetValue();

					bOk = true;
					kmResView.GetRecordset(fieldSet);
				}

				m_bOpenKmRefManager = false;
			}
			else
			{
				if (strFreeName.IsEmpty())
				{
					strFreeName = GetFreePropName();
					FormatStrByPropType(prop.GetType(), strFreeName);
				}
			}
		}

		//设置焦点
		m_pGridCtrl->SetFocus();
		//nType = prop.GetType();
	}

	//不为空 才进行添加
	if (!strFreeName.IsEmpty())
	{
		int nNewIndex = m_BackUpDatas.Add(m_ColumnItems);

		CAPSPropList & RefPropList = m_BackUpDatas[nNewIndex];
		POSITION pos = RefPropList.GetHeadPosition();
		CAPSProp & prop = RefPropList.GetNext(pos);

		if (fieldSet.GetCount() <= 1)
		{
			prop.ModifyByType(strFreeName);
		}

		CBCGPGridRow *pNewRow = InsertPropListItem(RefPropList);
		if (pNewRow == NULL)
		{
			return;
		}

		//如果确实是打开了资源管理器,并且点击的是一列
		if (fieldSet.GetCount() > 1)
		{
			//清空第一项的名称

			POSITION pos1 = fieldSet.GetHeadPosition();
			while (pos1)
			{
				const KmResViewProxy::ResViewFieldRecord & Node = fieldSet.GetNext(pos1);

				//开始寻找位置
				int nTmpIndex = 0;
				POSITION pos2 = m_ColumnItems.GetHeadPosition();
				while (pos2)
				{
					const CAPSProp & tmpConstPropItem = m_ColumnItems.GetNext(pos2);

					//获取配置的列的名称
					CString strTmpFieldName, strTmpResPath;
					CString strTmpFullResPath = tmpConstPropItem.GetRegMgrPath();
					strTmpFieldName = GetResMgrInfo(strTmpFullResPath, strTmpResPath);

					//找到了合适的位置nTmpIndex
					//两者都相等才认为找到了一项
					if (strResMgrPath == strTmpResPath && strTmpFieldName == Node._FieldName)
					{
						CAPSProp tmpPropItem;
						//修改属性
						if (RefPropList.SeekProp(tmpConstPropItem.GetName(), &tmpPropItem))
						{
							//if(1 || tmpPropItem.ConvertTo().IsEmpty()) //为空才进行修改
							{
								m_bModified = true;

								//修改属性的值
								CString strTmpNewValue = Node._FiledValue;
								FormatStrByPropType(tmpConstPropItem.GetType(), strTmpNewValue);
								tmpPropItem.ModifyByType(strTmpNewValue);

								RefPropList.ModifyProp(tmpPropItem.GetName(), tmpPropItem);

								//begin add by dm
								//更新树节点
								CBCGPGridItem *pItem = pNewRow->GetItem(nTmpIndex);
								if (pItem != NULL)
								{
									pItem->SetValue(_variant_t(strTmpNewValue));
								}
								//end add
							}
						}
						else
						{
							ASSERT(false);
						}

						//break;
					}
					++nTmpIndex;
				}
			}
		}
	}
}

void CGridPage::OnBnClickedApmResCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ReadProp();
	if(m_pGridCtrl)
	{
		m_pGridCtrl->RedrawWindow();
	}
}

void CGridPage::OnBnClickedApmResSave()
{
	// TODO: 在此添加控件通知处理程序代码
	//fix bug 4641 
	if (m_bSave)
	{
		CString strFileName = m_pManager->GetModel()->GetOpenedFileName();
		//m_pManager->GetModel()->Save(strFileName, 0, SERIAL_NORMAL);
	}
	m_bSave = true;
	
	SaveData();
}

void CGridPage::RemoveAllSelRows()
{
	if (!m_pGridCtrl)
	{
		return;
	}

	std::vector<int> vecRow = GetSelectedRows();
	if (vecRow.empty())
	{
		return;
	}

	for (std::vector<int>::const_iterator itrVec = vecRow.begin(); vecRow.end() != itrVec; ++itrVec)
	{
		int iRow = *itrVec;
		m_pGridCtrl->RemoveRow(iRow, FALSE);
		m_BackUpDatas.RemoveAt(iRow);
	}

	m_pGridCtrl->AdjustLayout();
}

std::vector<int> CGridPage::GetSelectedRows()
{
	std::vector<int> vecRow;
	if (!m_pGridCtrl)
	{
		return vecRow;
	}

	int iRowCnt = m_pGridCtrl->GetRowCount();
	vecRow.reserve(iRowCnt);
	for (int i = iRowCnt - 1; i >= 0; --i)
	{
		CBCGPGridRow* pRow = m_pGridCtrl->GetRow(i);
		if (!pRow)
		{
			continue;
		}
		if (!pRow->IsSelected())
		{
			continue;
		}
		vecRow.push_back(i);
	}

	return vecRow;
}

void CGridPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CKmPropertyPage::OnShowWindow(bShow, nStatus);

	BOOL bVisible = IsWindowVisible();

	// 保存数据
	if (!bShow && bVisible)
	{
		//begin del in 2018.12.10 xjt for 70105 
			//SaveData();
		//end del in 2018.12.10 xjt for 70105 
	}
}

void CGridPage::OnBnClickedApmResMoveUp()
{
	bool bRedraw = 1;
	CBCGPGridRow* pCurRow = NULL;
	int iMax = m_pGridCtrl->GetRowCount(0) - 1;
	int iCount = m_pGridCtrl->GetSelectionCount();
		if (iCount == 1)
		{
			pCurRow = m_pGridCtrl->GetCurSel();
		}
		if (pCurRow)
		{
			int iRow = pCurRow->GetRowId();
			if (iRow == 0)
			{
				return;
			}
			CBCGPGridRow* pNewRow = new CBCGPGridRow;
			m_pGridCtrl->InsertRowBefore(iRow - 1, pNewRow, bRedraw);
			int iItemCount = pCurRow->GetItemCount();
			for (int i = 0; i < iItemCount; ++i)
			{
				CBCGPGridItem* pNewItem = pNewRow->CreateItem(iRow - 1, i);
				pNewRow->AddItem(pNewItem);

				CBCGPGridItem* pItem = pCurRow->GetItem(i);
				int iOptionCount = pItem->GetOptionCount();
				_variant_t varVal = pItem->GetValue();
				if (iOptionCount > 0)
				{
					for (int j = 0; j < iOptionCount; ++j)
					{
						LPCTSTR szOptionData = pItem->GetOption(j);
						pNewItem->AddOption(szOptionData);
					}
					pNewItem->SetValue((_bstr_t)varVal);
				}
				else
				{
					pNewItem->SetValue(varVal, bRedraw);
				}
			}
			m_pGridCtrl->RemoveRow(iRow + 1, bRedraw);
			m_pGridCtrl->AdjustLayout();
			m_pGridCtrl->SetCurSel(iRow - 1);
		}
	
}

void CGridPage::OnBnClickedApmResMoveDown()
{
	bool bRedraw = 1;
	CBCGPGridRow* pCurRow = NULL;
	int iMax = m_pGridCtrl->GetRowCount(0) - 1;
	int iCount = m_pGridCtrl->GetSelectionCount();
		if (iCount == 1)
		{
			pCurRow = m_pGridCtrl->GetCurSel();
		}
		if (pCurRow)
		{
			int iRow = pCurRow->GetRowId();
			if (iRow == iMax)
			{
				return;
			}
			CBCGPGridRow* pNewRow = new CBCGPGridRow;
			m_pGridCtrl->InsertRowAfter(iRow + 1, pNewRow, bRedraw);
			int iItemCount = pCurRow->GetItemCount();
			for (int i = 0; i < iItemCount; ++i)
			{
				CBCGPGridItem* pNewItem = pNewRow->CreateItem(iRow + 1, i);
				pNewRow->AddItem(pNewItem);

				CBCGPGridItem* pItem = pCurRow->GetItem(i);
				int iOptionCount = pItem->GetOptionCount();
				_variant_t varVal = pItem->GetValue();
				if (iOptionCount > 0)
				{
					for (int j = 0; j < iOptionCount; ++j)
					{
						LPCTSTR szOptionData = pItem->GetOption(j);
						pNewItem->AddOption(szOptionData);
					}
					pNewItem->SetValue((_bstr_t)varVal);

				}
				else
				{
					pNewItem->SetValue(varVal, bRedraw);
				}
			}
			m_pGridCtrl->RemoveRow(iRow, bRedraw);
			m_pGridCtrl->AdjustLayout();
			m_pGridCtrl->SetCurSel(iRow + 1);
		}
}

CBCGPGridCtrl* CGridPage::GetGridCtrl()
{
	return m_pGridCtrl;
}

void CGridPage::AdjustPlace()
{
	if (!IsWindow(GetSafeHwnd()))
	{
		return;
	}
	/*
	CRect rect, rc;
	GetClientRect(&rect);
	rc = rect;

	rect.left += 2;
	rect.top += 10;
	rect.right -= 50;
	rect.bottom -= 30;

	int width = 24, height = 21;
	if (::IsWindow(m_AddItemBtn.GetSafeHwnd()))m_AddItemBtn.MoveWindow(rc.right - 30, rc.top + 10, width, height);
	if (::IsWindow(m_DeleteItemBtn.GetSafeHwnd()))m_DeleteItemBtn.MoveWindow(rc.right - 30, rc.top + height + 12, width, height);
	if (::IsWindow(m_MoveUpBtn.GetSafeHwnd()))m_MoveUpBtn.MoveWindow(rc.right - 30, rc.top + height * 2 + 14, width, height);
	if (::IsWindow(m_MoveDownBtn.GetSafeHwnd()))m_MoveDownBtn.MoveWindow(rc.right - 30, rc.top + height * 3 + 16, width, height);
	if (::IsWindow(m_SaveBtn.GetSafeHwnd()))m_SaveBtn.MoveWindow(rc.right - 30, rc.top + height * 4 + 18, width, height);
	if (::IsWindow(m_CancelBtn.GetSafeHwnd()))m_CancelBtn.MoveWindow(rc.right - 30, rc.top + height * 2 + 20, width, height);

	if (m_pGridCtrl && m_pGridCtrl->GetSafeHwnd())
	{
		m_pGridCtrl->MoveWindow(&rect);

		int iColCnt = m_pGridCtrl->GetColumnCount();
		if (iColCnt>0)
		{
			CRect rtWnd;
			m_pGridCtrl->GetWindowRect(&rtWnd);
			int iWidth = (rtWnd.Width() - 20) / iColCnt;
			for (int i = 0; i < iColCnt; ++i)
			{
				m_pGridCtrl->SetColumnWidth(i, iWidth);
			}
			m_pGridCtrl->AdjustLayout();
		}
	}
	*/

	CRect rect, grid_rect, ctrl_rect, add_rc, del_rc, up_rc, down_rc, save_rc, Associate_rc;
	int button_width = 0;
	int button_heigt = 0;
	GetClientRect(&rect);

	ctrl_rect = rect;
	ctrl_rect.InflateRect(-5, -20, -45, -10);

	if (m_pGridCtrl && m_pGridCtrl->GetSafeHwnd())
	{
		UpdateData();
		m_pGridCtrl->MoveWindow(&ctrl_rect);
		m_pGridCtrl->RedrawWindow();
	}

	if (m_AddItemBtn.GetSafeHwnd())
	{
		UpdateData();
		m_AddItemBtn.GetWindowRect(add_rc);
		ScreenToClient(add_rc);

		button_width = add_rc.Width();
		button_heigt = add_rc.Height();
		if (button_width < 16)button_width = 16;
		if (button_heigt < 16)button_heigt = 16;

		add_rc.left = ctrl_rect.right + 10;
		if (add_rc.left<10)
		{
			add_rc.left = 10;
		}
		add_rc.right = add_rc.left + button_width;
		add_rc.top = ctrl_rect.top;
		add_rc.bottom = add_rc.top + button_heigt;

		m_AddItemBtn.MoveWindow(add_rc);
		m_AddItemBtn.RedrawWindow();
	}

	if (m_DeleteItemBtn.GetSafeHwnd())
	{
		del_rc = add_rc;
		del_rc.top = add_rc.bottom + 3;
		del_rc.bottom = del_rc.top + button_heigt;
		m_DeleteItemBtn.MoveWindow(del_rc);
	}

	if (m_MoveUpBtn.GetSafeHwnd())
	{
		up_rc = del_rc;
		up_rc.top = del_rc.bottom + 3;
		up_rc.bottom = up_rc.top + button_heigt;
		m_MoveUpBtn.MoveWindow(up_rc);
	}

	if (m_MoveDownBtn.GetSafeHwnd())
	{
		down_rc = up_rc;
		down_rc.top = up_rc.bottom + 3;
		down_rc.bottom = down_rc.top + button_heigt;
		m_MoveDownBtn.MoveWindow(down_rc);
	}

	if (m_SaveBtn.GetSafeHwnd())
	{
		save_rc = down_rc;
		save_rc.top = down_rc.bottom + 3;
		save_rc.bottom = save_rc.top + button_heigt;
		m_SaveBtn.MoveWindow(save_rc);
	}
}


LRESULT CGridPage::OnBCGGridSelChanged(WPARAM wp, LPARAM lp)
{
	if (m_pGridCtrl == (CWnd *)lp)
	{
		CBCGPGridRow *pRow = m_pGridCtrl->GetCurSel();
		CBCGPGridItem *pItem = m_pGridCtrl->GetCurSelItem();
		if ((pItem == NULL) || (pRow == NULL))
		{
			return S_FALSE;
		}

		CAPSPropList & RefPropList = m_BackUpDatas[pRow->GetRowId()];
		POSITION pos = m_ColumnItems.FindIndex(pItem->GetColumnId());

		const CAPSProp & OriProp = m_ColumnItems.GetAt(pos);

		CAPSProp Prop;
		CString strPropName = OriProp.GetName();
		if (RefPropList.SeekDisplayNameProp(OriProp.GetDisplayName(), &Prop))
		{
			int iType = this->GetActType();
			CString strRegpath;
			if (iType == ACT_IACTION)
			{
				CActionGridPage* pGrid = (CActionGridPage*)this;
				strRegpath = IsHaveResMgrPath(NULL, pGrid->GetAction(), pGrid->GetGridName(), strPropName);
			}
			else
			{
				CProcessGridPage* pGrid = (CProcessGridPage*)this;
				strRegpath = IsHaveResMgrPath(pGrid->GetProcess(), NULL, pGrid->GetGridName(), strPropName);
			}
			CString strResMgrPath;
			GetResMgrInfo(strRegpath, strResMgrPath);
			//如果是 是从资源管理器 取 文字
			//目前只有中文版支持管理器功能。故添加限制 2014.05.19 for bug2144
			if (!strResMgrPath.IsEmpty() && KmAppSettings::GetAppLanguage() == APS_LANG_CHS)
			{
				//从资源管理器得到文字
				CString strResText;
				CList<KmResViewProxy::ResViewFieldRecord, KmResViewProxy::ResViewFieldRecord> fieldSet;

				m_bOpenKmRefManager = true;

				KmResViewProxy kmResView;
				kmResView.SetParentWnd(m_hWnd);
				kmResView.SetRootNodeName(strResMgrPath);
				if (kmResView.DoModal())
				{
					strResText = kmResView.GetValue();
					kmResView.GetRecordset(fieldSet);
				}

				m_bOpenKmRefManager = false;

				if (fieldSet.GetCount() > 1)		// 是整行
				{
					POSITION pos1 = fieldSet.GetHeadPosition();
					while (pos1)
					{
						const KmResViewProxy::ResViewFieldRecord & Node = fieldSet.GetNext(pos1);

						//开始寻找位置
						int nTmpIndex = 0;
						POSITION pos2 = m_ColumnItems.GetHeadPosition();
						while (pos2)
						{
							const CAPSProp & tmpConstPropItem = m_ColumnItems.GetNext(pos2);
							//找到了合适的位置nTmpIndex

							//获取配置的列的名称
							CString strTmpFieldName, strTmpResPath;
							CString strTmpFullResPath = tmpConstPropItem.GetRegMgrPath();
							strTmpFieldName = GetResMgrInfo(strTmpFullResPath, strTmpResPath);

							//找到了合适的位置nTmpIndex
							//两者都相等才认为找到了一项
							if (strResMgrPath == strTmpResPath && strTmpFieldName == Node._FieldName)
							{
								CAPSProp tmpPropItem;
								//修改属性
								if (RefPropList.SeekProp(tmpConstPropItem.GetName(), &tmpPropItem))
								{
									m_bModified = true;

									//修改属性的值
									CString strTmpNewValue = Node._FiledValue;
									FormatStrByPropType(tmpConstPropItem.GetType(), strTmpNewValue);
									tmpPropItem.ModifyByType(strTmpNewValue);

									RefPropList.ModifyProp(tmpConstPropItem.GetName(), tmpPropItem);

									//更新树节点
									CBCGPGridItem *pSubItem = pRow->GetItem(nTmpIndex);
									if (pSubItem != NULL)
									{
										pSubItem->SetValue(_variant_t(strTmpNewValue));
									}
								}
								else
								{
									ASSERT(false);
								}
							}
							++nTmpIndex;
						}
					}
				}
				else
				{
					CString strText;
					if (strResText.IsEmpty())
					{
						strText = Prop.ConvertTo();
					}
					else
					{
						strText = strResText;
						FormatStrByPropType(Prop.GetType(), strText);

						Prop.ModifyByType(strText);
						//修改属性
						RefPropList.ModifyProp(Prop.GetName(), Prop);

						m_bModified = true;
					}

					pItem->SetValue(_variant_t(strText));
				}
			}
		}
	}
	return S_OK;
}
