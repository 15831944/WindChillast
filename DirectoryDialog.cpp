#include "stdafx.h" 
#include "DirectoryDialog.h" 
//#include "ResourceString.h"
//#include "..\ResourceDef\APM_Res.h"
 
CDirectoryDialog::CDirectoryDialog(HWND hParaent)
{ 
	init();
	m_hwndOwner = hParaent;
} 
 
CDirectoryDialog::CDirectoryDialog(const CString& strIniKeyName, HWND hParaent)
{ 
	init(); 
	m_strIniKeyName = strIniKeyName;
	m_hwndOwner = hParaent;
} 
 
CDirectoryDialog::~CDirectoryDialog() 
{ 
 
} 
 
int CALLBACK CDirectoryDialog::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) 
{ 
	switch(uMsg) 
	{		 
	case BFFM_INITIALIZED://����lpDataѡ���ʼ�ļ���		 
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData); 
		return 1 ; 
	case BFFM_SELCHANGED: 
		{ 
			TCHAR szDir[1000]; 
			if (SHGetPathFromIDList((LPITEMIDLIST) lParam ,szDir)) 
			{//��ѡ����ļ�����ʾ�ھ�̬�ı�����,���̫��ֻ��ʾ��ͷ�ͽ�β 
				SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)(LPCTSTR)GetShortDir(szDir)); 
			}			 
		} 
		return 1 ; 
	} 
	return 1 ; 
} 
 
///////////////////////////////////////////////////////////////////////// 
//  ��������: CDirectoryDialog::ShowDirectoryTree 
//  ����˵��: ����ϵͳ�Ի������û�ѡ��һ�����ʵ��ļ��л��ļ� 
//  ��ڲ���: ��ʼ·�� 
//  ����ֵ  : �û�ѡ���·��,ȡ�����ؿմ� 
///////////////////////////////////////////////////////////////////////// 
CString CDirectoryDialog::ShowDirectoryTree(CString strIniDir) 
{ 
	CString strPath;//��¼�û�ѡ����ļ��� 
	TCHAR pszTemp[1000] = {_T("")}; 
	TCHAR pszTitle[1000] = {_T("")};//���� 

	if(m_strTitle.IsEmpty())	m_strTitle = GetDefaultTitle();//Ϊ��,�õ�Ĭ�ϵı��� 
	BROWSEINFO bi ={0}; 	 
	TCHAR pszDisplayName[MAX_PATH] = _T("\0");//�洢�û���ѡ�� 
	bi.hwndOwner = m_hwndOwner;
	bi.pszDisplayName = pszDisplayName;	 
	bi.ulFlags	 = GetStyle(m_ulFlags);	 
	bi.lpszTitle = m_strTitle ; 
	bi.lpfn		 = BrowseCallbackProc ; 
 
	if(strIniDir.IsEmpty()) 
		_tcscpy_s(pszTemp,(LPCTSTR)(AfxGetApp()->GetProfileString(m_strIniAppName,m_strIniKeyName,_T("")))); 
	else
		_tcscpy_s(pszTemp,strIniDir); 

	bi.lParam   = (LPARAM)pszTemp ;	 
	LPITEMIDLIST lpIL = SHBrowseForFolder(&bi);	 
	if(lpIL)
		SHGetPathFromIDList(lpIL,bi.pszDisplayName); 
	else
		return strIniDir;

	strPath = bi.pszDisplayName;//���������"\\"��β,����"\\" 

	if(strPath.Right(1)!='\\')		strPath += "\\"; 
	SetDefaultDirectory(strPath);//���ļ��д��ע����ini 
	return strPath; 
} 
 
CString CDirectoryDialog::GetDefaultDirectory() 
{ 
	//m_strIniAppName��m_strIniKeyNameΪ��,�����ע��� 
	if(m_strIniAppName.IsEmpty() || m_strIniKeyName.IsEmpty()) 
		return _T(""); 
	 
	return AfxGetApp()->GetProfileString(m_strIniAppName,m_strIniKeyName,_T("")); 
} 
 
void CDirectoryDialog::SetDefaultDirectory(CString strDefaultDirectory) 
{ 
	//m_strIniAppName��m_strIniKeyNameΪ��,�����ע��� 
	if(m_strIniAppName.IsEmpty() || m_strIniKeyName.IsEmpty()) 
	{ 
	//	ASSERT(false); 
		return ; 
	} 
	 
	AfxGetApp()->WriteProfileString(m_strIniAppName,m_strIniKeyName,strDefaultDirectory); 
} 
 
void CDirectoryDialog::init() 
{		 
	m_hwndOwner = NULL;     
    m_strTitle.Empty();  
    m_ulFlags  = SD_DIR;      
    	 
	m_strIniAppName = _T("InitDirectory");
	m_strIniKeyName.Empty(); 
} 
 
///////////////////////////////////////////////////////////////////////// 
//  ��������: CDirectoryDialog::GetShortDir 
//  ����˵��: ����·��ת�ɶ�·��;��·��=�̷�+1��6��"." + ���һ���ļ��л��ļ� 
//  ��ڲ���: ��·����,����� 
///////////////////////////////////////////////////////////////////////// 
CString CDirectoryDialog::GetShortDir(CString strDir,long nMaxLength/*=45*/) 
{ 
	ASSERT(nMaxLength > 5); 
	if(strDir.GetLength() < nMaxLength)	return strDir ;//û�г�����,���ô��� 
	CString strShortDir;//������̷�(��c:\)�����̷� 
	if(':'==strDir[1] && '\\'==strDir[2])	strShortDir = strDir.Left(3); 
	CString strLastDir ;//ȡ���һ���ļ��л��ļ� 
	int nPos = strDir.ReverseFind('\\'); 
	if(nPos != -1)		strLastDir = strDir.Mid(nPos); 
	if(strLastDir.GetLength() > nMaxLength - 5 ) 
	{		   
		strLastDir = strLastDir.Mid(strLastDir.GetLength() - (nMaxLength - 5)); 
		if('\\' != strLastDir[0])	strLastDir = '\\' + strLastDir ; 
	}//�м��������...,������� 
	int nSpace = nMaxLength - strShortDir.GetLength() - strLastDir.GetLength() ; 
	if(nSpace > 6)		nSpace = 6; 
	for(int i = 0 ; i< nSpace ; i++)		strShortDir +="."; 
	strShortDir += strLastDir ; 
	strLastDir.Empty(); 
	return strShortDir ;	 
} 
 
///////////////////////////////////////////////////////////////////////// 
//  ��������: CDirectoryDialog::ShowDirectoryTree 
//  ����˵��: ����ϵͳ�Ի������û�ѡ��һ��·�� 
//  ��ڲ���: ��ڲ����������� 
//  ��������Ϊ��,�򸸾��Ϊ�����ؼ��ĸ����� 
//	�������ѡ���·��д��ini,���ù����ؼ���ֵΪ��ʼֵ 
//	���û�ѡ���ֵ��ʾ�ڹ����ؼ���,��������������� 
///////////////////////////////////////////////////////////////////////// 
CString CDirectoryDialog::ShowDirectoryTree(CWnd *pBuddyWnd) 
{ 
	if(NULL == pBuddyWnd)	return "" ; 
	CString strDir ; 
	 
	if(NULL == m_hwndOwner)//�Թ����ؼ��ĸ�����Ϊ������ 
	{ 
		if(NULL != pBuddyWnd->GetParent()) 
			m_hwndOwner = pBuddyWnd->GetParent()->GetSafeHwnd(); 
	}		 
	if(m_strIniKeyName.IsEmpty())	pBuddyWnd->GetWindowText(strDir);//���û�н��ϴε�·��д��ע���,��ʼֵΪ�����ؼ������� 
	strDir = ShowDirectoryTree(strDir);		 
	if(!strDir.IsEmpty()) 
	{//���¹����ؼ�����ʾ������������ֵ 
		pBuddyWnd->SetWindowText(strDir); 
		if(NULL != pBuddyWnd->GetParent()) 
			pBuddyWnd->GetParent()->UpdateData(true); 
	}		 
	return strDir ; 
} 
 
CString CDirectoryDialog::GetDefaultTitle() 
{ 
	CString strTitle ; 
	if (m_strIniKeyName.IsEmpty())
		//strTitle = ResourceString(IDS_APM_SELDIRPATH, "��ѡ��Ŀ¼:");	
		strTitle = "��ѡ��Ŀ¼:";
	else 
	{ 
		TCHAR pszTitle[255]; 
		//wsprintf(pszTitle, ResourceString(IDS_APM_SELSUITDIRFOR, "��Ϊ%sѡ����ʵ�Ŀ¼:"),m_strIniKeyName); 
		wsprintf(pszTitle, "��ѡ����ʵ�Ŀ¼:");
		strTitle =  pszTitle ; 
	} 
	return strTitle ; 
} 
 
UINT CDirectoryDialog::GetStyle(enum Flag flag) 
{ 
	switch(flag) 
	{ 
//	case SD_DIR		 :	 
//		return BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT ;//ֻ��ʾ�ļ��� 
	case SD_DIR_FILE :	 
		return BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES;//��ʾ�ļ��к��ļ� 
	} 
	return BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT ;//ֻ��ʾ�ļ��� 
} 


