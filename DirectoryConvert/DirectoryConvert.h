#pragma once

#include "resource.h"
#include "afxwin.h"
#include <vector>
#include <algorithm>
#include "DBOperation.h"
#include "WindChillSetting.h"

using namespace std;

BOOL ExistFile(LPCTSTR strFileName)
{
	CFileFind tFind;
	if (tFind.FindFile(strFileName))
	{
		tFind.Close();
		return TRUE;
	}
	else
		return FALSE;
}

CString GetModuleDir()
{
	HINSTANCE hModule = AfxGetInstanceHandleHelper();
	if (!hModule)
	{
		hModule = GetModuleHandle(_T("std_WindChill_x86.dll"));
	}

	TCHAR lpBuffer[MAX_PATH];
	ZeroMemory(lpBuffer, sizeof(lpBuffer));
	GetModuleFileName(hModule, lpBuffer, MAX_PATH);
	ASSERT(_tcslen(lpBuffer) > 0);

	CString strPath(lpBuffer);

	int nIndex = strPath.ReverseFind(_T('\\'));
	ASSERT(nIndex != -1);

	return strPath.Mid(0, nIndex);
}

vector<CString> traverseFolderInfolder(CString path, CString type)
{
	CFileFind temFind;
	BOOL bFound;
	bFound = temFind.FindFile(path + type);
	vector<CString> Folder;
	CString strtmp;
	while (bFound)
	{
		bFound = temFind.FindNextFile();

		if (temFind.IsDots())
			continue;

		if (temFind.IsDirectory())
		{
			strtmp = temFind.GetFilePath();
			Folder.push_back(strtmp);
		}
	}

	temFind.Close();
	return Folder;
}

BOOL ConvertModel(CString strFileFullName, CString &strXml, CString strPath)
{
	char accredit[MAX_PATH];
	ZeroMemory(&accredit, sizeof(accredit));
	//GetCodeAccredit(accredit, sizeof(accredit));
	CWindChillSetting setting;
	setting.GetAppSettings();


	CString cmdinfo;
	//cmdinfo.Format(_T("\"%s\\ConvertFiles.exe\" \"%s\" \"%s\" \"%s\" \"%d\""), GetModuleDir(), "", strFileFullName, strPath, 0);
	cmdinfo.Format(_T("\"%s\\ConvertFiles.exe\" \"%s\" \"%s\" \"%s\" \"%d\" \"%s\""), setting.GetConverpath().GetBuffer(),"", strFileFullName, strPath, 0, "B////////wEA");
	STARTUPINFO   startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

	BOOL bCreate = CreateProcess(NULL,
		cmdinfo.GetBuffer(cmdinfo.GetLength()),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&startupInfo,
		&ProcessInfo);
	cmdinfo.ReleaseBuffer();

	if (bCreate)
	{
		bool bAbort = false;
		while (WaitForSingleObject(ProcessInfo.hProcess, (DWORD)18000000) == WAIT_TIMEOUT)
		{
			if (AfxMessageBox(_T("�ļ�ת��ʱ��������Ƿ����ת����"), MB_YESNO | MB_ICONQUESTION) == IDNO)
			{
				bAbort = true;
				break;
			}
		}

		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);

		if (bAbort)
		{
			return FALSE;
		}
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//����xml�ļ��Ƿ�����ȥ�ж�ת���Ƿ�ɹ�
	
	if (!ExistFile(strXml))
	{
		return FALSE;
	}

	return TRUE;
}

vector<CString> traverseFolder(CString path, CString type)
{
	CFileFind temFind;
	BOOL bFound;
	bFound = temFind.FindFile(path + type);
	vector<CString> file;
	CString strtmp;
	while (bFound)
	{
		bFound = temFind.FindNextFile();

		if (temFind.IsDots())
			continue;

		if (temFind.IsDirectory())
		{
			continue;
		}
		else
		{
			strtmp = temFind.GetFileName();
			file.push_back(strtmp);
		}
	}

	temFind.Close();
	return file;
}

BOOL JudgeFileConvert(CString dst, CString src)
{
	BOOL flag = FALSE;
	if (dst.CompareNoCase(src) ==0)
	{
		AfxMessageBox("�������·��������ͬ");
		return flag;
	}

	//����Ŀ��Ŀ¼
	auto dstfile = traverseFolder(dst+"\\", "*.CATPART");
	auto srcfile = traverseFolder(src + "\\", "*.xml");


	for (auto item =dstfile.begin();item!=dstfile.end();++item)
	{
		auto npost = item->ReverseFind('.');
		*item = item->Left(npost) + ".xml";
	}

	

	for (auto item =dstfile.begin();item!=dstfile.end();++item)
	{
		auto s = find(srcfile.begin(), srcfile.end(), *item);
		if (s == srcfile.end())
		{
			CString strXml = src + "\\" + *item;
			auto npost = (*item).ReverseFind('.');
			*item = item->Left(npost) + ".CATPART";
			CString strFileFullName = dst +"\\"+*item;

			flag = ConvertModel(strFileFullName, strXml, src);
			if (flag == FALSE)
			{
				return flag;
			}
		}
		else
			flag = TRUE;
	}

	 dstfile = traverseFolder(dst+"\\", "*.CATProduct");
	
	for (auto item =dstfile.begin();item!=dstfile.end();++item)
	{
		auto npost = item->ReverseFind('.');                                 
		*item = item->Left(npost) + ".xml";
	}

	for (auto item =dstfile.begin();item!=dstfile.end();++item)
	{
		auto s = find(srcfile.begin(), srcfile.end(), *item);
		if (s == srcfile.end())
		{
			CString strXml = src + "\\" + *item;
			auto npost = (*item).ReverseFind('.');
			*item = item->Left(npost) + ".CATProduct";
			CString strFileFullName = dst +"\\"+*item;

			flag = ConvertModel(strFileFullName, strXml, src);
			if (flag == FALSE)
			{
				return flag;
			}
		}
		else
			flag = TRUE;
	}
	return flag;
}