// DirectoryConvert.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "DirectoryConvert.h"
#include "unzip.h"
#include "zip.h"
#include "WindChillSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);
	CWindChillSetting setting;
	setting.GetAppSettings();

	CDBOperation db;
	auto isSuccess = db.ConnToDB();

	auto Path =traverseFolderInfolder(setting.GetPath(),"*.*");

	if (isSuccess&&!Path.empty())
	{
		for (auto item =Path.begin();item!=Path.end();++item)
		{
			auto path =*item+"\\";
			auto zip =traverseFolder(path.GetBuffer(),"*.zip");
			if (zip.empty())
			{
				continue;
			}

			auto zippath =path+zip[0];
			HZIP hz =OpenZip(zippath.GetBuffer(),0);
			SetUnzipBaseDir(hz,path.GetBuffer());
			ZIPENTRY ze;
			GetZipItem(hz,-1,&ze);
			int numitems =ze.index;
			for (int zi =0;zi<numitems;++zi)
			{
				GetZipItem(hz,zi,&ze);
				UnzipItem(hz,zi,ze.name);
			}
			CloseZip(hz);

			auto dirName =zip[0].Left(zip[0].GetLength()-4);
		
			auto src =*item+"\\"+dirName+"_hsf";
			CreateDirectory(src,NULL);
			auto des =*item+"\\"+dirName;
			auto flag =JudgeFileConvert(des, src);
			
			
			auto hzpath=des+"\\"+zip[0];
			hz =CreateZip(hzpath.GetBuffer(),0);
			auto srccontent =traverseFolder(src+"\\","*.*");
			if (srccontent.empty())
			{
				continue;
			}

			for(auto it=srccontent.begin();it!=srccontent.end();++it)
			{
				auto path =src+"\\"+*it;
				ZipAdd(hz,it->GetBuffer(),path.GetBuffer());
			}
			CloseZip(hz);

			
			auto s =CopyFile(hzpath,zippath,false);
			CString sql = CString("select * from EPMDocumentInfo where FileName =\'")+zip[0]+"\'";
			auto sqlresult = db.ExecuteWithResSQL(sql.GetBuffer());

			CString tempFileName, tempFolderPath, tempisConvert,tempEpmName,tempPartNumber;
			if (sqlresult)
			{
				_variant_t Part_Number, FILENAME, FolderPath,EpmName, isConvert;
				
				while (!sqlresult->adoEOF)
				{
					Part_Number = sqlresult->GetCollect("Part_Number");
					FILENAME = sqlresult->GetCollect("FILENAME");
					EpmName=sqlresult->GetCollect("EPM_NAME");
					FolderPath = sqlresult->GetCollect("FolderPath");
					isConvert = sqlresult->GetCollect("isConvert");

					
					if (FILENAME.vt != VT_NULL && FolderPath.vt != VT_NULL&&isConvert.vt != VT_NULL)
					{
						tempFileName = (LPCTSTR)(_bstr_t)FILENAME;
						tempFolderPath = (LPCTSTR)(_bstr_t)FolderPath;
						tempisConvert = (LPCTSTR)(_bstr_t)isConvert;
						tempEpmName =(LPCTSTR)(_bstr_t)EpmName;
						tempPartNumber=(LPCTSTR)(_bstr_t)Part_Number;
					}
					sqlresult->MoveNext();
				}
			}
			auto pos =tempFolderPath.ReverseFind('/');

			tempFolderPath=tempFolderPath.Left(pos);
			tempFolderPath +="/"+dirName;
			sql = CString("update EPMDocumentInfo set ISCONVERT='1',FolderPath=\'")+tempFolderPath.GetBuffer()+"\' where Part_Number =\'"+tempPartNumber.GetBuffer()+"\'";
			sqlresult = db.ExecuteWithResSQL(sql.GetBuffer());

			if (sqlresult)
			{

			}
		}
	}

	return nRetCode;
}
