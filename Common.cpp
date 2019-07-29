#include "stdafx.h"

#include "UtilityFunction.h"
#include "APSProp.h"
#include "Solid.h"
#include "Actionbase.h"
#include "AsmStep.h"
#include "CProduct.h"
#include "apsprocess.h"
#include "Equips.h"
#include "APSModel.h"
#include "tlhelp32.h"
#include "Common.h"
#include "WindChillSetting.h"

using namespace std;

DWORD GetProcessIDbyName(char *FileName)
{
	HANDLE myProcess;
	PROCESSENTRY32 mype;
	mype.dwSize = sizeof(PROCESSENTRY32);
	BOOL myBret;
	myProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	myBret = Process32First(myProcess, &mype);

	while (myBret)
	{
		if (strcmp(FileName, mype.szExeFile) == 0)
		{
			return mype.th32ProcessID;
		}
		else
			myBret = Process32Next(myProcess, &mype);
	}
	return 0;
}

//��ȡ��ǰ�ĵ����ļ���
BOOL GetCurFileName(CAPSModel* pModel, bool bPrompt, CString& strRes)
{
		BOOL bRet = FALSE;
		strRes = _T("");
		BOOL bIsSaved = TRUE;

		if (pModel == NULL)
		{
			return bRet;
		}

		if (pModel->GetView())
		{
			CDocument* pDoc = pModel->GetView()->GetDocument();
			if (pDoc)
			{
				strRes = pDoc->GetPathName();

				CString strExt = GetFileExt(strRes);
				if (strRes.IsEmpty())
				{
					/*if (bPrompt)
					{
					//AfxMessageBox(_T("�����ļ�ǰ�����ȱ���kmapx�ļ�!"));
					MessageBox(NULL,_T("����ǰ�����ȱ���Ϊkmapx�ļ�!"),APS_MSGBOX_TITIE,MB_OK);
					}
					return bRet;*/
					bIsSaved = FALSE;
				}

				if (strExt.CompareNoCase(_T("kmapx")) != 0)
				{
					/*if (bPrompt)
					{
					//AfxMessageBox(_T("�뱣��Ϊkmapx�ļ�������ļ�!"));
					MessageBox(NULL,_T("�뱣��Ϊkmapx�ļ����ٽ��д˲���!"),APS_MSGBOX_TITIE,MB_OK);
					}
					return bRet;*/
					bIsSaved = FALSE;
				}

				if (!bIsSaved)
				{
					// ��ʱĿ¼
					TCHAR TempDir[1024] = {};
					GetTempPath(1024, TempDir);
					CString sTempDir(TempDir);
					sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");
					if (!ExistDir(sTempDir))
					{
						if (!CreateDirectory(sTempDir, NULL))
							sTempDir = TempDir;
					}
					
					strRes = sTempDir + changePartName()+ ".kmapx";
				}

				pDoc->DoSave(strRes);
			}
			else
			{
				if (bPrompt)
				{
					//AfxMessageBox("�����쳣!");
					MessageBox(NULL, _T("�����쳣!"), APS_MSGBOX_TITIE, MB_OK);
				}
				return bRet;
			}
		}
		return TRUE;
}

//��ȡ��ǰ�ĵ����ļ���_1
BOOL GetCurFileName1(CAPSModel* pModel, bool bPrompt, CString& strRes)
{
	BOOL bRet = FALSE;
	strRes = _T("");

	if (pModel == NULL)
	{
		return bRet;
	}
	strRes = pModel->GetOpenedFileName();
	//begin del by xjt in 2018.11.29 for 70560 
	//if (pModel->GetView())
	//{
	//	CDocument* pDoc = (pModel->GetView())->GetDocument();
	//	ASSERT_VALID(pDoc);
	//end del by xjt in 2018.11.29 for 70560 
	if (!strRes.IsEmpty())
	{
		//strRes = pDoc->GetPathName();

		CString strExt = GetFileExt(strRes);

		if (strExt.CompareNoCase(_T("kmapx")) != 0)
		{
			if (bPrompt)
			{
				//AfxMessageBox(_T("�뱣��Ϊkmapx�ļ�������ļ�!"));
				MessageBox(NULL, _T("�뱣��Ϊkmapx�ļ����ٽ��д˲���!"), APS_MSGBOX_TITIE, MB_OK);
			}
			return bRet;
		}

		//pDoc->DoSave(strRes);
	}
	else
	{
		if (bPrompt)
		{
			//AfxMessageBox("�����쳣!");
			//MessageBox(NULL, _T("�����쳣!"), APS_MSGBOX_TITIE, MB_OK);
			MessageBox(NULL, _T("����ǰ�����ȱ���Ϊkmapx�ļ�!"), APS_MSGBOX_TITIE, MB_OK);
		}
		return bRet;
	}
	//}
	return TRUE;
}

//ת��UTF8
CString ConvertUTF8(CString szAnsi)
{
	CString strRes = _T("");
	if (!szAnsi.IsEmpty())
	{
		int x = CP_ACP;
		//��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�������936Ϊ��������GB2312����ҳ
		UINT nLen = MultiByteToWideChar(CP_ACP, NULL, (const char*)szAnsi, -1, NULL, NULL);
		WCHAR *wszBuffer = new WCHAR[nLen + 1];
		nLen = MultiByteToWideChar(CP_ACP, NULL, (const char*)szAnsi, -1, wszBuffer, nLen);
		wszBuffer[nLen] = '\0';

		//��ȡתΪUTF8���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
		nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
		CHAR *szBuffer = new CHAR[nLen + 1];
		nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
		szBuffer[nLen] = '\0';

		strRes = szBuffer;
		if (wszBuffer)
		{
			delete[] wszBuffer;
			wszBuffer = NULL;
		}

		if (szBuffer)
		{
			delete[] szBuffer;
			szBuffer = NULL;
		}
	}

	return strRes;
}

bool LookUpInArray(CStringArray &arr, CString val)
{
	int nSize = arr.GetCount();
	for (int i = 0; i < nSize; i++)
	{
		if (arr.GetAt(i).CompareNoCase(val) == 0)
		{
			return true;
		}
	}
	return false;
}

//����ת��ģ��
BOOL ConvertModel(CString strFileFullName, CString &strXml)
{
	char accredit[MAX_PATH];
	ZeroMemory(&accredit, sizeof(accredit));
	//GetCodeAccredit(accredit, sizeof(accredit));
	int npos = strFileFullName.ReverseFind('\\');
	CString strPath = strFileFullName.Left(npos);

	CString cmdinfo;
	//cmdinfo.Format(_T("\"%s\\ConvertFiles.exe\" \"%s\" \"%s\" \"%s\" \"%d\""), GetModuleDir(), "", strFileFullName, strPath, 0);
	cmdinfo.Format(_T("\"%s\\ConvertFilesShow.exe\" \"%s\" \"%s\" \"%s\" \"%d\" \"%s\""), GetModuleDir(), "", strFileFullName, strPath, 0, "AAAAAAAAAAAA");
	STARTUPINFO   startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_SHOWDEFAULT;

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
			if (MessageBox(NULL,"�ļ�ת��ʱ��������Ƿ����ת����",APS_MSGBOX_TITIE ,MB_YESNO | MB_ICONQUESTION) == IDNO)
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
	npos = strFileFullName.ReverseFind(_T('.'));
	strXml = strFileFullName.Left(npos) + _T(".xml");
	if (!ExistFile(strXml))
	{
		return FALSE;
	}

	return TRUE;
}

//char ת wchar
std::string UTF8toMulitype(const char * utf8)
{
	
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t *wszGbk = new wchar_t[len + 1];
	memset(wszGbk, 0, len * 2+2);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wszGbk, len);

	len = WideCharToMultiByte(CP_ACP, 0, wszGbk, -1,NULL,0,NULL,NULL);
	char *szGbk = new char[len + 1];
	memset(szGbk, 0, len +1);
	WideCharToMultiByte(CP_ACP, 0, wszGbk, -1, szGbk, len, NULL, NULL);

	string strtemp(szGbk);
	if (wszGbk)
	{
		delete[] wszGbk;
	}
	if (szGbk)
	{
		delete[] szGbk;
	}

	return strtemp;
}

vector<string> split(string str, string pattern)
{
	string::size_type pos;
	vector<string> result;
	str += pattern;
	auto size = str.size();
	for (size_t i = 0; i < size; ++i)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			auto s = str.substr(i, pos - i);
			result.emplace_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}


PartInfo lookforpartbyNumber(std::vector<PartInfo> partInfo, string cstr)
{
	auto  part = partInfo.begin();
	for (auto partitem = partInfo.begin(); partitem != partInfo.end(); ++partitem)
	{
		if (partitem->value["Number"] == cstr)
		{
			part = partitem;
			return *partitem;
		}
	}
	return NULL;
}


void documentAnsiToutf8(CString strXml)
{
	CFile file;
	file.Open(strXml, CFile::modeReadWrite);
	
	auto dwFilelen = file.GetLength();
	char *buf;
	buf = new char[dwFilelen + 1];
	buf[dwFilelen] = 0;

	file.Read(buf, dwFilelen);

	file.SetLength(0);
	auto content = ConvertUTF8(buf);
	file.Write(content, content.GetLength());
	file.Close();
	if (buf)
	{
		delete[]buf;
		buf = NULL;
	}
}

CString changePartName(CString s)
{
	CString name;
	if (s.IsEmpty())
		name =CWindChillSetting::m_strpartFirstName;
	else
		name =s;
	char info[] ={'\\','/',':','?','*','<','>','|'};
	auto begin =std::begin(info);
	auto end =std::end(info);
	for (auto it =begin;it!=end;++it)
	{
		name.Replace(_T(*it),_T('_'));
	}

	return name;
}

