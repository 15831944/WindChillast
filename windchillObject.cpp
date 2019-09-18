// WindChillObject.cpp : CWindChillObject 的实现

#include "stdafx.h"
#include <direct.h>
#include <afxinet.h>

#include "APSProp.h"
#include "Solid.h"
#include "Actionbase.h"
#include "AsmStep.h"
#include "CProduct.h"
#include "apsprocess.h"
#include "Equips.h"
#include "APSModel.h"

#include "tinyxml.h"
#include "common.h"
#include "DlgLogIn.h"
#include "DlgPBOMEdit.h"
#include "DlgMaterialEdit.h"
#include "WindChill_i.h"
#include "WindChillObject.h"
#include "WindChillSetting.h"
#include "KmZipUtil.h"
#include "UtilityFunction.h"
#include "WindChillXml.h"
#include "DlgSavePdf.h"
#include "DlgProcEdit.h"
#include "AsmInst.h"
#include "PBOMUtil.h"
#include "APSWaitCursor.h"
#include "DlgProcRule.h"
#include "EquipActionIn.h"
#include "Equips.h"

#include "DlgAddMaterial.h"
#include "DlgPDFExport.h"
#include "DlgPDFConfig.h"
#include "Partinfo.h"
#include "PBomCreate.h"
#include "PBOMUtil.h"
#include "DlgCollect.h"
#include "DlgChange.h"
#include "PdfDataToXMl.h"
#include "PDFData.h"


using namespace std;
#define  MSG_FILE_NEW WM_USER + 2089
#define  MSG_UPDATE_PART WM_USER + 2091


#ifdef DEV_TMP
	bool g_islogin = true;   //
#else
	bool g_islogin = false;   //
#endif

STDMETHODIMP CwindchillObject::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] =
	{
		&IID_IWindChillObject
	};

	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i], riid))
			return S_OK;
	}
	return S_FALSE;
}
STDMETHODIMP CwindchillObject::raw_IsChangeGetDownFileMode(void)
{
	return S_OK;

}


STDMETHODIMP CwindchillObject::raw_GetDownFilePath(long* strkey, BSTR* bstrFile)
{
	//在dll中调用对话框
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDlgAddMaterial dlg(ResourceType_EQUIPTOOL);
	if (dlg.DoModal() == IDOK)
	{
		auto contentXml = dlg.GetXMl();
		TiXmlPrinter printer;
		contentXml.Accept(&printer);
		CString str = "";
		str.Format("%s", printer.CStr());

		auto path = CWindChillXml::GetPath(str);

		
		CString strFTPPath, strFTPName , strModelName ;
		//获取xml读取的文件，引入产品；打开模型文件
		//ftp下载. 

		

		if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
		{
			MessageBox(NULL, _T("ftp连接失败！"), APS_MSGBOX_TITIE, MB_OK);
			return False;
		}

		TCHAR TempDir[1024];
		GetTempPath(1024, TempDir);
		CString sTempDir(TempDir);
		CString tempdir = sTempDir + _T("KM3DCAPP-AWORK");

		if (!ExistDir(tempdir))
		{
			CreateDir(tempdir);
		}
		sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");
		CString strlocal = sTempDir + strFTPName;

		BOOL bSucc = m_FTPInterface.DownLoad(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
		if (!bSucc)
		{
			CString strErrMsg = _T("下载文件失败！");
			//strErrMsg.Format(_T("下载文件【%s】失败！"), strFTPName);
			MessageBox(NULL, strErrMsg, APS_MSGBOX_TITIE, MB_OK);

			return FALSE;
		}

		*bstrFile = (_bstr_t)strlocal;


//是否删除ftp文件
#ifdef	DeleteFTPFILE
						if (bSucc)
						{
							bSucc = m_FTPInterface.Delete(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
							if (!bSucc)
							{
								CString strErrMsg = _T("删除FTP文件失败！");
								MessageBox(NULL,strErrMsg, APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
								return FALSE;
							}
						}
#endif
		return S_OK;
	}

	return S_FALSE;
}



STDMETHODIMP CwindchillObject::raw_AddOrUpdateSolid(UINT ID, BSTR bstrFile)
{
	IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
	if (!doc)
	{
		ASSERT(FALSE);
		return S_FALSE;
	}
	//AfxMessageBox(CString(bstrFile));
	CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();

	auto Product = pModel->GetProduct();
	if (Product)
	{
		CSolid* Solid = Product->LookupSolid(ID);

		if (Solid)
		{
			
			for (int i = 0; i < CWindChillSetting::m_strRelMatch.GetSize(); ++i)
			{
				CString strName = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;
				CString strValue = CWindChillSetting::m_strRelMatch.GetAt(i).m_strValue;
				bool bShow = 1;
				bool bEdit = 0;

				CAPSProp Baseprop(strName, "", strName, 1, 1);

				CAPSProp prop;
				Solid->SeekObjProp(strName,prop);
				Solid->SeekObjPropByDisName(strName,prop);
				Solid->SeekObjProp2(strName,prop);
				auto str =prop.ConvertTo();
				Solid->UpdateProp(Baseprop.GetName(), prop.ConvertTo());
				Solid->CSolid::ModifyPropStateByDisName(Baseprop.GetName(), 1, 1);
			}

			CAPSProp prop("#solid_add_file", "", "addFile", 1, 0);
			Solid->UpdateProp(prop.GetName(), CString(bstrFile));
			Solid->CSolid::ModifyPropStateByDisName(prop.GetName(), 0, 1);
		}
	}
	CWindChillSetting::GYJID.push_back(to_string((long long)(ID)));
	UserPropIsEdit();
	return S_OK;
}

STDMETHODIMP CwindchillObject::raw_GetName(BSTR* bstrName)
{
	*bstrName = SysAllocString(L"WindChill");

	return S_OK;
}

STDMETHODIMP CwindchillObject::raw_GetComment(BSTR* bstrName)
{
	return S_OK;
}

STDMETHODIMP CwindchillObject::raw_Connect(IDispatch* app)
{
	HRESULT hr = app->QueryInterface(__uuidof(IAPSApplication), (void**)&m_pApplication);
	if (m_pApplication)
	{
		DWORD dwCookie = 0;
		hr = DispEventAdvise(app);
		//return hr;
	}
	hr = app->QueryInterface(__uuidof(IPSManager), (void**)&m_pPSM);
	if (m_pPSM)
	{
		DWORD dwCookie = 0;
		hr = DispEventAdvise(app);
		return hr;
	}

	return S_FALSE;
}

STDMETHODIMP CwindchillObject::raw_Disconnect()
{
	if (m_pApplication)
	{
		HRESULT hr = DispEventUnadvise(m_pApplication);
		m_pApplication->Release();
		m_pApplication = NULL;
		//return hr;
	}

	if (m_pPSM)
	{
		HRESULT hr = DispEventUnadvise(m_pPSM);
		m_pPSM->Release();
		m_pPSM = NULL;

		return hr;
	}

	return S_FALSE;
}

STDMETHODIMP CwindchillObject::raw_GetEnable(long bEnable)
{
	return S_OK;
}
////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CwindchillObject::OnAppInit(IAPSApplication* pdispApp)
{
	return S_OK;
}

STDMETHODIMP CwindchillObject::OnAppClose()
{
	return S_OK;
}

STDMETHODIMP CwindchillObject::OnDocumentOpening(BSTR bstrPath)
{
	return S_OK;
}

STDMETHODIMP CwindchillObject::OnDocumentOpened(IAPSDocument* doc)
{
	return S_OK;
}

STDMETHODIMP CwindchillObject::OnDocumentSaving(IAPSDocument* doc)
{
	return S_OK;
}

//输出xml文件
STDMETHODIMP CwindchillObject::OnDocumentSaved(IAPSDocument* doc, BSTR bstrPath)
{
	CAPSModel* pModel = (CAPSModel*)doc->GetAPSModel();
	return S_OK;
}


int CwindchillObject::OutputPDF(CString sPathFile, int iType, CString &sOutputFilePath)
{
	CString strlocal;
	BOOL bRet = GetCurFileName(m_pModel, true, strlocal);
	if (!bRet || strlocal.IsEmpty())
	{
		return FALSE;
	}

	CString strDllPath = GetModuleDir();
	strDllPath += _T("\\Output3DPdf.dll");
	HMODULE hLib = LoadLibrary(strDllPath);
	if (hLib)
	{
		typedef BOOL(WINAPI *InitA3DPDF)();

		InitA3DPDF pInitA3DPDF = (InitA3DPDF)GetProcAddress(hLib, _T("InitA3DPDF"));
		if (pInitA3DPDF)
		{
			pInitA3DPDF();
		}

		typedef BOOL(WINAPI *OutPutPdf)(CAPSModel*, LPCTSTR, LPCTSTR, LPCTSTR, bool, int, int, int&);

		OutPutPdf pOutPutPdf = (OutPutPdf)GetProcAddress(hLib, _T("OutPutPdf"));

		if (pOutPutPdf == NULL)
		{
			FreeLibrary(hLib);
			hLib = NULL;

			MessageBox(NULL, "未能导出PDF,请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
			return FALSE;
		}

		while (1)
		{
			CDlgPDFConfig dlgConfig;
			dlgConfig.setTemplateName(sPathFile);
			dlgConfig.SetProcess(m_pProcess);
			dlgConfig.setTemplateType(iType);

			if (dlgConfig.DoModal())
			{

			}

			int iMode = dlgConfig.getMode();
			if (iMode == 0)//取消
			{
				return 0;
			}
			if (iMode == 2)//上一步
			{
				return 2;
			}

			CDlgPDFExport dlgExport;
			dlgExport.setFilePath(strlocal);
			if (dlgExport.DoModal())
			{

			}

			iMode = dlgExport.getMode();
			if (iMode == 2)//上一步
			{
				continue;
			}
			else if (iMode == 1)//完成
			{
				dlgExport.getOutputFilePath(sOutputFilePath);
				break;
			}
			else//取消
			{
				return 0;
			}

			dlgExport.getOutputFilePath(sOutputFilePath);
			break;
		}

		if (sOutputFilePath.IsEmpty() || sPathFile.IsEmpty())
			return 0;

		CApsWaitCur waiter;

		int nCodeError = 0;

		TCHAR TempDir[1024] = {};
		GetTempPath(1024, TempDir);
		CString sTempDir(TempDir);
		sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");

		auto collectXmlPath = sTempDir + "save.xml";

		if (!ExistFile(collectXmlPath))  //是否存在汇总信息和更改单信息
		{
			BOOL bOutput = pOutPutPdf(m_pModel, sPathFile, "", sOutputFilePath, true, 1, iType, nCodeError);
			if (!bOutput)
			{
				MessageBox(NULL, "PDF导出失败.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
				return FALSE;
			}
			else
			{
				CWindChillSetting::SetPdfPath_Rule(sOutputFilePath);
			}

			FreeLibrary(hLib);
			hLib = NULL;
		}
		else
		{

			XmlToPdfdata pdfdata;
			pdfdata.GetData(collectXmlPath);

			auto m_PageData = pdfdata.GetPageData();

			typedef BOOL(WINAPI *GetPutPdfInfo)(CAPSModel*, LPCTSTR, LPCTSTR, LPCTSTR, bool, int, int, int&, COutPut3DPdf *&);

			GetPutPdfInfo pGetPutPdf = (GetPutPdfInfo)GetProcAddress(hLib, _T("GetPutPdfInfo"));

			if (pGetPutPdf == NULL)
			{
				FreeLibrary(hLib);
				hLib = NULL;
				MessageBox(NULL, "未能导出PDF,请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
			}

			COutPut3DPdf *m_putpdf;

			pGetPutPdf(m_pModel, sPathFile, "", sOutputFilePath, true, 1, iType, nCodeError,m_putpdf);

			if (m_putpdf)
			{
				auto config = m_putpdf->GetPdfConfig();
				auto data = m_putpdf->GetPdfData();

				if (data)
				{
					for (auto it = m_PageData.begin(); it != m_PageData.end(); ++it)
					{
						auto vpages = it->second;

						for (auto pageItem = vpages.begin(); pageItem != vpages.end(); ++pageItem)
						{
							auto name = (*pageItem)->m_strDefName;
							for (auto i = 0; i < data->GetPage()->GetSize(); ++i)
							{
								auto &page = data->GetPage()->GetAt(i);
								if (name.CompareNoCase(page->m_strDefName) == 0 && (*pageItem)->m_No == page->m_No)
								{
									page = *pageItem;

								}
							}
						}
					}

				}
			}

			auto changePath = sTempDir + "changeinfo.xml";
			if (ExistFile(changePath))
			{
				ChangeXmlToPageData temp(changePath);

				temp.GenPageData();
				auto changinfo = temp.GetChangInfo();
				auto data = m_putpdf->GetPdfData();
				data->AddPage(changinfo);
				BOOL bOutPut;

				m_putpdf->SetOutPath(sOutputFilePath);
				bOutPut = m_putpdf->OutPutNew();

				if (!bOutPut)
				{
					MessageBox(NULL, "PDF导出失败.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
				
				}
				else
				{
					CWindChillSetting::SetPdfPath_Rule(sOutputFilePath);
				}

			}
			else
			{
				BOOL bOutPut;

				m_putpdf->SetOutPath(sOutputFilePath);
				bOutPut = m_putpdf->OutPutNew();

				if (!bOutPut)
				{
					MessageBox(NULL, "PDF导出失败.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);

				}
				else
				{
					CWindChillSetting::SetPdfPath_Rule(sOutputFilePath);
				}
			}

			FreeLibrary(hLib);
			hLib = NULL;

			
		}
	
	}
	else
	{
		MessageBox(NULL, "未能导出PDF,请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
		return 0;
	}

	return 1;
}

CString CwindchillObject::GetModuleDir()
{
	HINSTANCE hModule = AfxGetInstanceHandleHelper();
	if (!hModule)
	{
		hModule = GetModuleHandle(_T("APSModelView.dll"));
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


void CwindchillObject::GetXMLConfig(std::map<int, CString> &mapConfig)
{
	mapConfig = m_mapConfig;
}

void CwindchillObject::ClearXMLConfig()
{
	m_mapConfig.erase(m_mapConfig.begin(), m_mapConfig.end());
	m_mapConfig.clear();
}

void CwindchillObject::SetParam(CAPSModel* pModel, CProcess* pProcess, CString sCfgPath, CString sFilePath)
{
	m_pModel = pModel;
	m_pProcess = pProcess;
	m_sCfgPath = sCfgPath;
	m_sOutPath = sFilePath;
	m_iPageNums = 0;
}

BOOL CwindchillObject::OpenPdfEditPage(CString sPathFile, CAPSModel* pModel, int iType)
{
	CString sOutputFilePath = "", sLoadFilePath = "";
	CDlgProcRule dlg;
	dlg.setTemplateName(sPathFile);
	dlg.SetProcess(m_pProcess);
	CString strlocal = "";
	BOOL bRet = GetCurFileName(pModel, true, strlocal);
	if (!bRet || strlocal.IsEmpty())
	{
		return FALSE;
	}

	//begin add by xjt in 2019.2.19 for 70780
	//用来记录上次打开的文件路径
	static CString path = "";

	if (path == "")
	{
		dlg.setFilePath(strlocal);
	}
	else
	{
		dlg.setFilePath(strlocal);
	}

	//end add
	//if (pModel->GetOpenedFileName().IsEmpty())

	dlg.SetProcTempConfig(m_PdfConfigDatas);
	dlg.setTemplateType(iType);

	CApsWaitCur waiter;
	CString strDllPath = GetModuleDir();
	strDllPath += _T("\\Output3DPdf.dll");
	HMODULE hLib = LoadLibrary(strDllPath);
	if (hLib)
	{
		typedef BOOL(WINAPI *InitA3DPDF)();

		InitA3DPDF pInitA3DPDF = (InitA3DPDF)GetProcAddress(hLib, _T("InitA3DPDF"));
		if (pInitA3DPDF)
		{
			pInitA3DPDF();
		}
		
		if (dlg.DoModal())
		{
			dlg.getPath(sOutputFilePath, sLoadFilePath);
			path = sOutputFilePath;
			m_ConfigProcPDFName->clear();
			m_PdfConfigDatas->RemoveAll();
			dlg.GetProcTempConfig(m_PdfConfigDatas);
		}
		
		IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
		if (!doc)
		{
			ASSERT(FALSE);
			return FALSE;
		}

		CAPSModel *pModel1 = (CAPSModel*)doc->GetAPSModel();
	
		TCHAR TempDir[1024] = {};
		GetTempPath(1024, TempDir);
		CString sTempDir(TempDir);
		sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");

		auto collectXmlPath = sTempDir + "save.xml";

		if (!ExistFile(collectXmlPath))  //是否存在汇总信息和更改单信息
		{

			if (sOutputFilePath.IsEmpty() || sLoadFilePath.IsEmpty())
				return FALSE;

			SetParam(pModel1, m_pProcess, sLoadFilePath, sOutputFilePath);

			int nCodeError = 0;

			CApsWaitCur waiter1;
			typedef BOOL(WINAPI *OutPutPdf)(CAPSModel*, LPCTSTR, LPCTSTR, LPCTSTR, bool, int, int, int&);

			OutPutPdf pOutPutPdf = (OutPutPdf)GetProcAddress(hLib, _T("OutPutPdf"));

			if (pOutPutPdf == NULL)
			{
				FreeLibrary(hLib);
				hLib = NULL;

				MessageBox(NULL, "请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
				return FALSE;
			}


			BOOL bOutput = pOutPutPdf(pModel1, sPathFile, "", sOutputFilePath, true, 1, iType, nCodeError);
			if (!bOutput)
			{
				MessageBox(NULL, "PDF导出失败.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
				return FALSE;
			}
			else
			{
				CWindChillSetting::SetPdfPath_Rule(sOutputFilePath);
			}

			FreeLibrary(hLib);
			hLib = NULL;

			if (!bOutput)
			{
				MessageBox(NULL, "导出失败.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
				return FALSE;
			}
			else
			{
				MessageBox(NULL, "导出完成.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
				CWindChillSetting::SetPdfPath_Rule(sOutputFilePath);
			}
		}
		else
		{

			XmlToPdfdata pdfdata;
			pdfdata.GetData(collectXmlPath);

			auto m_PageData = pdfdata.GetPageData();

			typedef BOOL(WINAPI *GetPutPdfInfo)(CAPSModel*, LPCTSTR, LPCTSTR, LPCTSTR, bool, int, int, int&, COutPut3DPdf *&);

			GetPutPdfInfo pGetPutPdf = (GetPutPdfInfo)GetProcAddress(hLib, _T("GetPutPdfInfo"));

			if (pGetPutPdf == NULL)
			{
				FreeLibrary(hLib);
				hLib = NULL;
				MessageBox(NULL, "未能导出PDF,请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
			}

			COutPut3DPdf *m_putpdf;
			int nCodeError=0;
			pGetPutPdf(pModel1, sPathFile, "", sOutputFilePath, true, 1, iType, nCodeError, m_putpdf);

			if (m_putpdf)
			{
				auto config = m_putpdf->GetPdfConfig();
				auto data = m_putpdf->GetPdfData();

				if (data)
				{
					for (auto it = m_PageData.begin(); it != m_PageData.end(); ++it)
					{
						auto vpages = it->second;

						for (auto pageItem = vpages.begin(); pageItem != vpages.end(); ++pageItem)
						{
							auto name = (*pageItem)->m_strDefName;
							for (auto i = 0; i < data->GetPage()->GetSize(); ++i)
							{
								auto &page = data->GetPage()->GetAt(i);
								if (name.CompareNoCase(page->m_strDefName) == 0 && (*pageItem)->m_No == page->m_No)
								{
									page = *pageItem;

								}
							}
						}
					}

				}
			}
			BOOL bOutPut = false;
			auto changePath = sTempDir + "changeinfo.xml";
		
		
			if (ExistFile(changePath))
			{
	
				ChangeXmlToPageData temp(changePath);

				temp.GenPageData();
				auto changinfo = temp.GetChangInfo();
				auto data = m_putpdf->GetPdfData();
				data->AddPage(changinfo);
				
				m_putpdf->m_pData = data;
				m_putpdf->SetOutPath(sOutputFilePath);
				bOutPut = m_putpdf->OutPutNew();
			}
			else
			{
				m_putpdf->SetOutPath(sOutputFilePath);
				bOutPut = m_putpdf->OutPutNew();

			}
			if (!bOutPut)
			{
				MessageBox(NULL, "导出失败.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
			}
			else
			{
				MessageBox(NULL, "导出完成.\n", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
				CWindChillSetting::SetPdfPath_Rule(sOutputFilePath);
			}
			FreeLibrary(hLib);
			hLib = NULL;


		}
	}
	else
	{
		MessageBox(NULL, "请检查Output3DPdf.dll是否存在.\n", APS_MSGBOX_TITIE, MB_OK);
		return FALSE;
	}

	return TRUE;
}


std::map<CString, CString> CwindchillObject::GetStepinfo(CAsmStep * step)
{
	map<CString, CString>result;
	//基础属性
	auto baselist = step->GetBasePropList();
	if (baselist)
	{
		POSITION pos = baselist->GetHeadPosition();
		auto ID = step->GetID();
		result["id"] = to_string(static_cast<_LONGLONG>(ID)).c_str();

		while (pos != NULL)
		{
			CAPSProp prop = baselist->GetNext(pos);
			CString sVal = prop.ConvertTo();

			auto Name = prop.GetName();
			if (Name.CompareNoCase("#prop_name") == 0)
			{
				Name = "name";
			}

			if (Name.CompareNoCase("#prop_asm_content") == 0)
			{
				Name = "content";
			}
			result[Name] = sVal;
		}
	}

	auto pids = step->GetConstObjs();
	vector<UINT> IDS;
	for (auto i = 0; i < pids->GetCount(); ++i)
	{
		int nId = pids->GetAt(i);
		IDS.push_back(nId);
	}

	result["predecessors"] = "";
	auto Product = m_pModel->GetProduct();
	for (auto ID = IDS.begin(); ID != IDS.end(); ++ID)
	{
		if (Product)
		{
			CSolid* Solid = Product->LookupSolid(*ID);
			if (!Solid)
			{
				//前置工序
				if (result["predecessors"].IsEmpty())
				{
					result["predecessors"] = to_string(static_cast<_LONGLONG>(*ID)).c_str();
				}
				else
					result["predecessors"] += (";" + to_string(static_cast<_LONGLONG>(*ID))).c_str();
			}

		}
	}
	return result;
}


std::map<CString, CString> CwindchillObject::GetUserStepinfo(CAsmStep * step)
{
	map<CString, CString>result;
	//自定义属性

	auto UserList = step->GetUserPropList();

	auto pos = UserList->GetHeadPosition();
	while (pos != NULL)
	{
		CAPSProp prop = UserList->GetNext(pos);
		CString sVal = prop.ConvertTo();
		auto Name = prop.GetName();
		result[Name] = sVal;
	}

	return result;
}


std::vector<std::map<CString, CString>> CwindchillObject::GetIdinfo(CAsmStep * step)
{
	auto pids = step->GetConstObjs();
	vector<UINT> IDS;
	for (auto i = 0; i < pids->GetCount(); ++i)
	{
		int nId = pids->GetAt(i);
		IDS.push_back(nId);
	}
	vector<map<CString, CString>> container;
	auto Product = m_pModel->GetProduct();
	for (auto ID = IDS.begin(); ID != IDS.end(); ++ID)
	{
		if (Product)
		{
			map<CString, CString>result;
			CSolid* Solid = Product->LookupSolid(*ID);

			if (Solid)
			{
				POSITION pos = Solid->m_baseList.GetHeadPosition();
				while (pos != NULL)
				{
					CAPSProp prop = Solid->m_baseList.GetNext(pos);
					CString sVal = prop.ConvertTo();
					auto Name = prop.GetName();
					result[Name] = sVal;
				}

				pos = Solid->m_useList.GetHeadPosition();
				while (pos != NULL)
				{
					CAPSProp prop = Solid->m_baseList.GetNext(pos);
					CString sVal = prop.ConvertTo();
					auto Name = prop.GetName();
					result[Name] = sVal;
				}
				container.push_back(result);
			}
		}
	}
	return container;
}

std::vector<std::map<CString, CString>> CwindchillObject::GetSolidinfo(CAsmStep * step)
{
	auto pids = step->GetConstObjs();
	vector<UINT> IDS;
	for (auto i = 0; i < pids->GetCount(); ++i)
	{
		int nId = pids->GetAt(i);
		IDS.push_back(nId);
	}
	vector<map<CString, CString>> container;
	auto Product = m_pModel->GetProduct();
	for (auto ID = IDS.begin(); ID != IDS.end(); ++ID)
	{
		if (Product)
		{
			map<CString, CString>result;
			CSolid* Solid = Product->LookupSolid(*ID);
			if (Solid)
			{
				result["id"] = to_string(static_cast<_LONGLONG>(*ID)).c_str();

				if (result["id"].Left(1) == "2")
					result["type"] = "prt";
				else
					result["type"] = "asm";

				auto Name = Solid->GetShowName();
				result["name"] = Name;

				auto fileName = Solid->GetEntityName();

				if (!fileName.IsEmpty())
				{
					fileName = Solid->GetEntityName() + ".hsf";
				}

				result["fileName"] = fileName;

				container.push_back(result);
			}

		}
	}
	return container;
}

std::map<CString, std::vector<std::map<CString, CString>>>  CwindchillObject::GetGridData(CArray<CAPSGridPropData*>* actionGridDatas)
{
	std::map<CString, std::vector<std::map<CString, CString>>> content;
	if (!actionGridDatas)
	{
		return content;
	}

	int len = actionGridDatas->GetCount();
	for (int i = 0; i < len; ++i)
	{
		std::vector<std::map<CString, CString>> container;
		CAPSGridPropData* pData = actionGridDatas->GetAt(i);
		CString sGridName = pData->GetGridName();
		CArray<CAPSPropList, CAPSPropList>* val = actionGridDatas->GetAt(i)->GetPropDataAry();
		int row = val->GetCount();
		map<CString, CString>result;
		for (int j = 0; j < row; ++j)
		{
			auto col = val->GetAt(j).GetSize();

			POSITION pos = val->GetAt(j).GetHeadPosition();

			while (pos)
			{
				CAPSProp prop = val->GetAt(j).GetNext(pos);
				CString sVal = prop.ConvertTo();
				auto Name = prop.GetName();
				result[Name] = sVal;
			}
			container.push_back(result);
		}

		content[sGridName] = container;
	}

	return content;
}

std::vector<CSolid *> CwindchillObject::GetStepSolid(CAsmStep * step)
{
	std::vector<CSolid *> container;

	if (step)
	{
		CArray<CAsmStep *, CAsmStep *>* pSubSteps = step->GetSubSteps();
		for (int m = 0; m < pSubSteps->GetSize(); m++)
		{
			auto pStep = pSubSteps->GetAt(m);
			if (pStep)
			{
				std::map<CString, CString> result;
				CArray<CActionBase *, CActionBase *>* pActions = pStep->GetActions();
				for (int i = 0; i < pActions->GetSize(); i++)
				{
					CActionBase* psAct = pActions->GetAt(i);
					if (CEquipActionIn* pEquipAct = dynamic_cast<CEquipActionIn*>(psAct))
					{
						CArray<UINT, UINT> * pObjs = pEquipAct->GetObjs();
						for (int j = 0; j < pObjs->GetCount(); ++j)
						{
							int nId = pObjs->GetAt(j);
							CEquips* pEquipsUion = m_pModel->GetEquips();
							if (pEquipsUion)
							{
								CSolid* pSolid = pEquipsUion->FindEquipSolid(nId);
								if (pSolid)
								{
									container.push_back(pSolid);
								}
							}
						}
					}
				}
			}
		}
	}

	return container;
}

std::map<CAsmStep *,std::map<CString, CString>>  CwindchillObject::GetChildStepInfo(CAsmStep * step)
{
	std::map<CAsmStep *,std::map<CString, CString>> container;

	if (step)
	{
		CArray<CAsmStep *, CAsmStep *>* pSubSteps = step->GetSubSteps();

	  //步骤下的所有子步骤 
		for (int m = 0; m < pSubSteps->GetSize(); m++)
		{
			auto pStep = pSubSteps->GetAt(m);
			if (pStep)
			{
				auto stepinfo =GetStepinfo(pStep);
				container[pStep]=stepinfo;
			}
		}
	}

	return container;
}

//获取子步骤下的工艺装备活动
std::vector<std::map<CString, CString>> CwindchillObject::GetChildStepEquies(CAsmStep * pStep)
{
	std::vector<std::map<CString, CString>> container;
	if (pStep)
	{
		std::map<CString, CString> result;
		CArray<CActionBase *, CActionBase *>* pActions = pStep->GetActions();
		for (int i = 0; i < pActions->GetSize(); i++)
		{
			CActionBase* psAct = pActions->GetAt(i);
			if (CEquipActionIn* pEquipAct = dynamic_cast<CEquipActionIn*>(psAct))
			{
				CArray<UINT, UINT> * pObjs = pEquipAct->GetObjs();
				for (int j = 0; j < pObjs->GetCount(); ++j)
				{
					int nId = pObjs->GetAt(j);
					CEquips* pEquipsUion = m_pModel->GetEquips();
					if (pEquipsUion)
					{
						CSolid* pSolid = pEquipsUion->FindEquipSolid(nId);
						if (pSolid)
						{
							result["id"] = to_string(static_cast<_LONGLONG>(nId)).c_str();
							auto Name = pSolid->GetShowName();
							result["name"] = Name;

							auto fileName = pSolid->GetShowName() + ".hsf";

							result["fileName"] = fileName;

							POSITION pos = pSolid->m_baseList.GetHeadPosition();
							while (pos != NULL)
							{
								CAPSProp prop = pSolid->m_baseList.GetNext(pos);
								CString sVal = prop.ConvertTo();
								auto Name1 = prop.GetName();
								result[Name1] = sVal;
							}

							pos = pSolid->m_useList.GetHeadPosition();
							while (pos != NULL)
							{
								CAPSProp prop = pSolid->m_useList.GetNext(pos);
								CString sVal = prop.ConvertTo();
								auto Name1 = prop.GetName();
								result[Name1] = sVal;
							}

						}

						container.push_back(result);
					}
				}
			}
		}
	}
	
	

	return container;
}

void CwindchillObject::GenMbomXml(CString strNewSubPath)
{
	TiXmlDocument doc;

	TiXmlElement * KmAssemblyProcess = new TiXmlElement("KmAssemblyProcess");

	TiXmlElement * ProcInfo = new TiXmlElement("ProcInfo");
	if (m_pProcess)
	{
		std::map<CString, CString> baseprop;

		std::map<CString, CString> userprop;
		POSITION pos = m_pProcess->GetBasePropList()->GetHeadPosition();
		while (pos)
		{
			CAPSProp prop = m_pProcess->GetBasePropList()->GetNext(pos);
			CString sVal = prop.ConvertTo();
			auto Name = prop.GetName();

			if (Name.CompareNoCase("#prop_name") == 0)
			{
				Name = "name";
			}

			if (Name.CompareNoCase("#prop_asm_content") == 0)
			{
				Name = "content";
			}


			baseprop[Name] = sVal;
		}

		for (auto it = baseprop.begin(); it != baseprop.end();++it)
		{
			ProcInfo->SetAttribute(it->first,it->second);
		}
		ProcInfo->SetAttribute("number",CWindChillSetting::m_strpartFirstName);

		pos = m_pProcess->GetUserPropList()->GetHeadPosition();
		while (pos)
		{

			CAPSProp prop = m_pProcess->GetBasePropList()->GetNext(pos);
			CString sVal = prop.ConvertTo();
			auto Name = prop.GetName();
			userprop[Name] = sVal;
		}
		TiXmlElement * ExtPropList = new TiXmlElement("ExtPropList");
		for (auto it = userprop.begin(); it != userprop.end(); ++it)
		{
			TiXmlElement * ExtProp = new TiXmlElement("ExtProp");

			ExtProp->SetAttribute("name", it->first);
			ExtProp->SetAttribute("value", it->second);

			ExtPropList->LinkEndChild(ExtProp);
		}

		ProcInfo->LinkEndChild(ExtPropList);

		CArray<CAPSGridPropData*>* actionGridDatas = m_pProcess->GetProcessGridDatas();
		auto GridData = GetGridData(actionGridDatas);

		TiXmlElement * GridPropList = new TiXmlElement("GridPropList");
		for (auto x = GridData.begin(); x != GridData.end(); ++x)
		{
			TiXmlElement * GridProp = new TiXmlElement("GridProp");
			GridProp->SetAttribute("name", x->first);

			TiXmlElement * GridRecordList = new TiXmlElement("GridRecordList");
			for (auto Gridit = (x->second).begin(); Gridit != (x->second).end(); ++Gridit)
			{
				TiXmlElement * GridRecord = new TiXmlElement("GridRecord");
				for (auto it = (*Gridit).begin(); it != (*Gridit).end(); ++it)
				{
					TiXmlElement * GridCell = new TiXmlElement("GridCell");

					GridCell->SetAttribute("name", it->first);
					GridCell->SetAttribute("value", it->second);

					GridRecord->LinkEndChild(GridCell);
				}
				GridRecordList->LinkEndChild(GridRecord);
			}
			GridProp->LinkEndChild(GridRecordList);
			GridPropList->LinkEndChild(GridProp);
		}

		ProcInfo->LinkEndChild(GridPropList);

	}
	else
		return; 

	auto steps = m_pProcess->GetAsmSteps();
	auto size = steps->GetSize();
	for (auto count = 0; count < steps->GetSize(); ++count)
	{
		CAsmStep* pSimple = steps->GetAt(count);
		//获取该步骤下信息
		auto stepinfo = GetStepinfo(pSimple);
		TiXmlElement * ProcedureInfo = new TiXmlElement("ProcedureInfo");

		for (auto it = stepinfo.begin(); it!= stepinfo.end();++it)
		{
			ProcedureInfo->SetAttribute(it->first,it->second);
		}

		//获取该步骤下所有的装配对象，及装配对象信息
		auto Solidinfo = GetSolidinfo(pSimple);

		auto IDinfo = GetIdinfo(pSimple);

		TiXmlElement * AsmObjList = new TiXmlElement("AsmObjList");

		for (auto itID = IDinfo.begin(), itSolid = Solidinfo.begin(); itID != IDinfo.end(); ++itID, ++itSolid)
		{
			TiXmlElement * AsmObj = new TiXmlElement("AsmObj");

			for (auto it = (*itSolid).begin(); it != (*itSolid).end(); ++it)
			{
				AsmObj->SetAttribute(it->first, it->second);
			}

			TiXmlElement * Params = new TiXmlElement("Params");
			for (auto it = (*itID).begin(); it != (*itID).end(); ++it)
			{
				TiXmlElement * Param = new TiXmlElement("Param");
				Param->SetAttribute("name", it->first);
				Param->SetAttribute("value", it->second);
				Params->LinkEndChild(Param);
			}
			AsmObj->LinkEndChild(Params);
			AsmObjList->LinkEndChild(AsmObj);
		}

		ProcedureInfo->LinkEndChild(AsmObjList);

		TiXmlElement * ExtPropList = new TiXmlElement("ExtPropList");

		//自定义信息
		auto UserStepinfo = GetUserStepinfo(pSimple);
		for (auto it = UserStepinfo.begin(); it != UserStepinfo.end(); ++it)
		{
			TiXmlElement * ExtProp = new TiXmlElement("ExtProp");

			ExtProp->SetAttribute("name", it->first);
			ExtProp->SetAttribute("value", it->second);

			ExtPropList->LinkEndChild(ExtProp);
		}

		ProcedureInfo->LinkEndChild(ExtPropList);

		TiXmlElement * GridPropList = new TiXmlElement("GridPropList");

		//网格属性

		auto actionGridDatas = pSimple->GetActionGridDatas();
		auto GridData = GetGridData(actionGridDatas);

		for (auto x = GridData.begin(); x != GridData.end();++x)
		{
			TiXmlElement * GridProp = new TiXmlElement("GridProp");
			GridProp->SetAttribute("name", x->first);

			TiXmlElement * GridRecordList = new TiXmlElement("GridRecordList");
			for (auto Gridit = (x->second).begin(); Gridit != (x->second).end(); ++Gridit)
			{
				TiXmlElement * GridRecord = new TiXmlElement("GridRecord");
				for (auto it = (*Gridit).begin(); it != (*Gridit).end(); ++it)
				{
					TiXmlElement * GridCell = new TiXmlElement("GridCell");

					GridCell->SetAttribute("name", it->first);
					GridCell->SetAttribute("value", it->second);

					GridRecord->LinkEndChild(GridCell);
				}
				GridRecordList->LinkEndChild(GridRecord);
			}
			GridProp->LinkEndChild(GridRecordList);
			GridPropList->LinkEndChild(GridProp);
		}

		ProcedureInfo->LinkEndChild(GridPropList);

		TiXmlElement * StepList = new TiXmlElement("StepList");

		
		auto chidstepinfo =GetChildStepInfo(pSimple);
		
		for (auto childstep =chidstepinfo.begin();childstep!=chidstepinfo.end();++childstep)
		{
			auto childstepinfo =childstep->second;
			TiXmlElement * StepInfo = new TiXmlElement("StepInfo");
			for (auto childstepattr =childstepinfo.begin();childstepattr!=childstepinfo.end();++childstepattr)
			{
				StepInfo->SetAttribute(childstepattr->first,childstepattr->second);
			}

			auto childAsmstep =childstep->first;


			auto  childAsmstepinfo=GetChildStepEquies(childAsmstep);

			for (auto it =childAsmstepinfo.begin();it!=childAsmstepinfo.end();++it)
			{
				TiXmlElement * AsmObj = new TiXmlElement("AsmObj");
				AsmObj->SetAttribute("fileName", (*it)["fileName"]);
				(*it).erase("fileName");
				AsmObj->SetAttribute("id", (*it)["id"]);
				(*it).erase("id");
				AsmObj->SetAttribute("name", (*it)["name"]);
				(*it).erase("name");

				TiXmlElement * Params = new TiXmlElement("Params");
				for (auto itParam = (*it).begin(); itParam != (*it).end();++itParam)
				{
					TiXmlElement * Param = new TiXmlElement("Param");
					Param->SetAttribute("name", itParam->first);
					Param->SetAttribute("value", itParam->second);
					Params->LinkEndChild(Param);
				}
				AsmObj->LinkEndChild(Params);

				StepInfo->LinkEndChild(AsmObj);
			}
			StepList->LinkEndChild(StepInfo);
			/*TiXmlElement * AsmObj = new TiXmlElement("AsmObj");
				AsmObj->SetAttribute("fileName", (*it)["fileName"]);
				(*it).erase("fileName");
				AsmObj->SetAttribute("id", (*it)["id"]);
				(*it).erase("id");
				AsmObj->SetAttribute("name", (*it)["name"]);
				(*it).erase("name");

				TiXmlElement * Params = new TiXmlElement("Params");
				for (auto itParam = (*it).begin(); itParam != (*it).end();++itParam)
				{
					TiXmlElement * Param = new TiXmlElement("Param");
					Param->SetAttribute("name", itParam->first);
					Param->SetAttribute("value", itParam->second);
					Params->LinkEndChild(Param);
				}
				AsmObj->LinkEndChild(Params);*/
		}
		
		ProcedureInfo->LinkEndChild(StepList);

		ProcInfo->LinkEndChild(ProcedureInfo);
			/*
			TiXmlElement * AsmObj = new TiXmlElement("AsmObj");
			AsmObj->SetAttribute("fileName", (*it)["fileName"]);
			(*it).erase("fileName");
			AsmObj->SetAttribute("id", (*it)["id"]);
			(*it).erase("id");
			AsmObj->SetAttribute("name", (*it)["name"]);
			(*it).erase("name");

			TiXmlElement * Params = new TiXmlElement("Params");
			for (auto itParam = (*it).begin(); itParam != (*it).end();++itParam)
			{
				TiXmlElement * Param = new TiXmlElement("Param");
				Param->SetAttribute("name", itParam->first);
				Param->SetAttribute("value", itParam->second);
				Params->LinkEndChild(Param);
			}
			AsmObj->LinkEndChild(Params);
			StepList->LinkEndChild(AsmObj);
		}

		ProcedureInfo->LinkEndChild(StepList);

		ProcInfo->LinkEndChild(ProcedureInfo);*/
	}

	KmAssemblyProcess->LinkEndChild(ProcInfo);
	doc.LinkEndChild(KmAssemblyProcess);

	doc.SaveFile(strNewSubPath+"mbom.xml");

	

	//多字节xml转utf-8
	documentAnsiToutf8(strNewSubPath+"mbom.xml");

}


void CwindchillObject::GenProcXml(CString strNewSubPath)
{
	//遍历3dgx文件，除了0.3dgx
	//std::vector<CString> vctXmlName;
	CFileFind finder;
	BOOL working = finder.FindFile(strNewSubPath + "\\*.3dgx");
	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			continue;
		}
		/*if (finder.GetFileName().CompareNoCase("0.3dgx") == 0)
		{
		continue;
		}*/
		else
		{
			//解压*.3dgx
			KmZipUtil kmZipUtil;
			if (kmZipUtil.OpenZipFile(strNewSubPath + finder.GetFileName(), KmZipUtil::ZipOpenForUncompress))
			{
				if (!ExistDir(strNewSubPath))
				{
					if (!::CreateDirectory(strNewSubPath, NULL))
					{
					}
				}
				kmZipUtil.UnZipAll(strNewSubPath);
				kmZipUtil.CloseZipFile();
			}
		}
	}
	MSXML2::IXMLDOMDocumentPtr	pProcXmlDoc(__uuidof(MSXML2::DOMDocument60));
	if (pProcXmlDoc == NULL)
	{
		ASSERT(FALSE);
		return;
	}
	//根节点
	MSXML2::IXMLDOMElementPtr  Root;
	pProcXmlDoc->raw_createElement((_bstr_t)(char*)"KmAssemblyProcess", &Root);
	pProcXmlDoc->raw_appendChild(Root, NULL);
	//遍历*.3dgxs
	working = finder.FindFile(strNewSubPath + "\\*.3dgxs");
	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}
		if (finder.IsDirectory())
		{
			continue;
		}
		/*if (finder.GetFileName().CompareNoCase("0.3dgxs") == 0)
		{
		continue;
		}*/
		else
		{
			//在当前3dgxs里的AsmObj节点加入属性
			MSXML2::IXMLDOMDocumentPtr	pXmlDoc(__uuidof(MSXML2::DOMDocument60));
			pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
			if (!pXmlDoc)
			{
				ASSERT(FALSE);
				continue;
			}
			CString tmp = strNewSubPath + finder.GetFileName();
			if (pXmlDoc->load((LPCTSTR)(tmp)) == VARIANT_FALSE)
			{
				ASSERT(FALSE);
				continue;
			}

			// 读取XML文档根节点
			MSXML2::IXMLDOMNodeListPtr pGroupList = pXmlDoc->GetchildNodes();
			int a = pGroupList->Getlength();
			if (pGroupList->Getlength() != 2)
				continue;

			MSXML2::IXMLDOMNodePtr pRoot = pGroupList->Getitem(1);
			if (pRoot == NULL)
			{
				ASSERT(FALSE);
				continue;
			}
			TraverseAsmObj(pRoot, strNewSubPath);
			MSXML2::IXMLDOMNodePtr pProcedure = pRoot->GetfirstChild();
			Root->appendChild(pProcedure);
		}
	}
	pProcXmlDoc->save(_variant_t(strNewSubPath + "Proc.xml"));
}


void CwindchillObject::TraverseAsmObj(MSXML2::IXMLDOMNodePtr pProcedure, CString strNewSubPath)
{
	MSXML2::IXMLDOMNodeListPtr pGroupList = pProcedure->GetchildNodes();
	if (pGroupList->Getlength() == 0)
	{
		CString strName = pProcedure->GetnodeName();
		if (strName.CompareNoCase(_T("AsmObj")) != 0)
		{
			return;
		}
		//从PBOM.xml取当前asmobj的属性
		MSXML2::IXMLDOMElementPtr pItem = pProcedure;
		CString strID = pItem->getAttribute(_T("id"));
		AttachAsmObjAttr(pProcedure, strID, strNewSubPath);

	}
	else
	{
		for (int i = 0; i < pGroupList->Getlength(); i++)
		{
			MSXML2::IXMLDOMNodePtr pSub = pGroupList->Getitem(i);
			TraverseAsmObj(pSub, strNewSubPath);
		}
	}
}

void CwindchillObject::AttachAsmObjAttr(MSXML2::IXMLDOMNodePtr pProcedure, CString strID, CString strNewSubPath)
{
	MSXML2::IXMLDOMDocumentPtr	pXmlDoc(__uuidof(MSXML2::DOMDocument60));
	pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));

	if (!pXmlDoc)
	{
		ASSERT(FALSE);
		return;
	}

	if (pXmlDoc->load((LPCTSTR)(strNewSubPath + "PBOM.xml")) == VARIANT_FALSE)
	{
		ASSERT(FALSE);
		return;
	}

	// 读取XML文档根节点

	MSXML2::IXMLDOMNodeListPtr pGroupList = pXmlDoc->GetchildNodes();
	if (pGroupList->Getlength() != 2)
		return;


	MSXML2::IXMLDOMNodePtr pGroup = pGroupList->Getitem(1);
	MSXML2::IXMLDOMNodeListPtr pNodeList = pGroup->GetchildNodes();
	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
		CString strName = pNode->GetnodeName();
		if (strName.CompareNoCase("Node") != 0)
			continue;
		if (TraverseNodeParams(pProcedure, pNode, strID))
			break;
	}
}

BOOL CwindchillObject::TraverseNodeParams(MSXML2::IXMLDOMNodePtr pProcedure, MSXML2::IXMLDOMNodePtr pNode, CString strID)
{
	MSXML2::IXMLDOMElementPtr pItem = pNode;
	CString strType = pItem->getAttribute(_T("type"));
	CString strInsID = pItem->getAttribute(_T("instanceid"));
	CString strName = pItem->getAttribute(_T("name"));

	MSXML2::IXMLDOMNodeListPtr pParamList = pNode->GetchildNodes();
	if (strInsID.CompareNoCase(strID) == 0)
	{
		MSXML2::IXMLDOMElementPtr pDest = pProcedure;
		pDest->setAttribute("name", (LPCTSTR)strName);
		if (pParamList != NULL)
		{
			for (int i = 0; i < pParamList->Getlength(); i++)
			{
				MSXML2::IXMLDOMNodePtr pParam = pParamList->Getitem(i);
				CString strParam = pParam->GetnodeName();
				if (strParam.CompareNoCase("Params") != 0)
					continue;
				pProcedure->appendChild(pParam);
				break;
			}
		}
		return TRUE;
	}
	if (strType.CompareNoCase("asm") == 0)
	{
		MSXML2::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
		for (int i = 0; i < pNodeList->Getlength(); i++)
		{
			MSXML2::IXMLDOMNodePtr pSubNode = pNodeList->Getitem(i);
			CString strName1 = pSubNode->GetnodeName();
			if (strName1.CompareNoCase("Node") != 0)
				continue;
			if (TraverseNodeParams(pProcedure, pSubNode, strID))
				break;
		}
	}
	return FALSE;
}


CString CwindchillObject::GetRelativeDir()
{
	SYSTEMTIME tmptime;
	GetLocalTime(&tmptime);

	CString sUid = GetUid();
	char sTime[32] = { 0 };
	sprintf_s(sTime, "%.4d%.2d%.2d%s/", tmptime.wYear, tmptime.wMonth, tmptime.wDay, sUid);
	return sTime;
}

CString CwindchillObject::GetUid()
{
	CString strTmp = "";
	GUID guid;
	CoInitialize(NULL);
	if (::CoCreateGuid(&guid) == S_OK)
	{
		strTmp.Format("%.8x", guid.Data4);
	}
	CoUninitialize();
	return strTmp;
}
extern vector<string> split(string str, string pattern);

void CwindchillObject::CheckinPBom(CString strlocal)
{
	if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
	{
		MessageBox(NULL, "ftp连接失败！", APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	auto s =GetRelativeDir().FindOneOf("//");
	CString strFileName =  GetRelativeDir().Left(s)+ ".zip";

	CString strDir = strlocal.Left(strlocal.ReverseFind(_T('\\')) + 1);
	CString strNewSubPath;

	if (!GenAlonePath(strDir, strNewSubPath))
	{
		//strErrorInfo = CString(ResourceString(IDS_KMAPSTOOL2_FAILEDCD, "创建目录失败！"));
		//break;
		return;
	}

	//创建路径
	if (!CreateDirectory(strNewSubPath, NULL))
	{
		//strErrorInfo = CString(ResourceString(IDS_KMAPSTOOL2_FAILEDCD, "创建目录失败！"));
		//break;
		return;
	}

	//在此处修改上传至ftp的相对路径
	CString relativpath = "checkin/Pbom/";
	CString strRelDir = relativpath;//+ GetRelativeDir();

	//工艺件模型检入

	KmZipUtil kmZipUtil;

	if (kmZipUtil.OpenZipFile(strlocal, KmZipUtil::ZipOpenForUncompress))
	{
		if (!ExistDir(strNewSubPath))
		{
			if (!::CreateDirectory(strNewSubPath, NULL))
			{
			}
		}
		kmZipUtil.UnZipAll(strNewSubPath);
		//kmZipUtil.CloseZipFile();
	}
	kmZipUtil.CloseZipFile();


	CString strXml = strNewSubPath + "PBOM.xml";

	MSXML2::IXMLDOMDocumentPtr	pXmlDoc(__uuidof(MSXML2::DOMDocument60));
	pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
	if (!pXmlDoc)
	{
		ASSERT(FALSE);
	}

	if (pXmlDoc->load((LPCTSTR)(strXml)) == VARIANT_FALSE)
	{
		ASSERT(FALSE);
		//return false;
		return;
	}

	MSXML2::IXMLDOMNodeListPtr pGroupList = pXmlDoc->GetchildNodes();
	if (pGroupList->Getlength() != 2)
		return;

	MSXML2::IXMLDOMNodePtr pGroup = pGroupList->Getitem(1);
	MSXML2::IXMLDOMNodeListPtr pNodeList = pGroup->GetchildNodes();
	std::vector<CString> Path;

	std::vector<MSXML2::IXMLDOMNodeListPtr> Nodelist;

	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
		CString strName = pNode->GetnodeName();
		if (strName.CompareNoCase("Node") != 0)
			continue;
		CAstXml::TraverseAstXmlNodeParam(pXmlDoc, pNode, Path,Nodelist, TRUE);
	}


	//新添加的零部件需要手动填写一些属性信息，有一些属性为必填属性，在配置文件中已经填写；如果没有填写属性，则提示
	//将必填属性填写完成，才能检入；
	//检入属性校验
	for (auto it = Nodelist.begin(); it != Nodelist.end(); ++it)
	{
		auto pNodeList1 = *it;
		for (int i = 0; i < pNodeList1->Getlength(); i++)
		{
			MSXML2::IXMLDOMNodePtr pParam = pNodeList1->Getitem(i);
			MSXML2::IXMLDOMElementPtr pItem = pParam;
			if (pItem)
			{
				CString strAttrName = pItem->getAttribute(_T("name"));
				CString strAttrValue = pItem->getAttribute(_T("value"));

				for (auto j = 0; j < CWindChillSetting::m_strCheckPropMatch.GetCount(); ++j)
				{
					CString strTmp = CWindChillSetting::m_strCheckPropMatch.GetAt(j).m_strName;
					if (strAttrName.CompareNoCase(strTmp) == 0 && strAttrValue.IsEmpty())
					{
						CString Prompt = _T("\"") + strTmp + _T("\"") + _T("为必须填写属性");

						MessageBox(NULL, Prompt, APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
						return;
					}
				}
			}
		}
	}

	//add by xjt in 2019.1.19	
	//删除vector中的重复元素
	for (auto it = Path.begin(); it != Path.end();)
	{
		auto it1 = find(Path.begin(), Path.end(), *it);
		if (it1 != it)
			it = Path.erase(it);
		else
			++it;
	}
	//end


	if(kmZipUtil.OpenZipFile(strlocal, KmZipUtil::ZipOpenForCompress))
	{
		if (Path.size() > 0)
		{
			//	kmZipUtil.AddDirInZip(_T("epmInfo"), KmZipUtil::ZipOpenForCompress);
			for (auto it = Path.begin(); it != Path.end(); it++)
			{
				kmZipUtil.ZipFile(it->GetBuffer(), NULL);
			}
		}
	}

	kmZipUtil.CloseZipFile();

	BOOL bSucc = m_FTPInterface.UpLoad(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strRelDir, strFileName);
	if (!bSucc)
	{
		MessageBox(NULL, "上传失败！", APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	/*
	KmZipUtil kmZipUtil;

	if (kmZipUtil.OpenZipFile(strlocal, KmZipUtil::ZipOpenForUncompress))
	{
	if (!ExistDir(strNewSubPath))
	{
	if (!::CreateDirectory(strNewSubPath, NULL))
	{
	}
	}
	kmZipUtil.UnZipAll(strNewSubPath);
	//kmZipUtil.CloseZipFile();
	}

	kmZipUtil.CloseZipFile();

	CString strXml = strNewSubPath + "PBOM.xml";

	MSXML2::IXMLDOMDocumentPtr	pXmlDoc(__uuidof(MSXML2::DOMDocument60));
	pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
	if (!pXmlDoc)
	{
	ASSERT(FALSE);
	}

	if (pXmlDoc->load((LPCTSTR)(strXml)) == VARIANT_FALSE)
	{
	ASSERT(FALSE);
	//return false;
	return;
	}

	MSXML2::IXMLDOMNodeListPtr pGroupList = pXmlDoc->GetchildNodes();
	if (pGroupList->Getlength() != 2)
	return;

	MSXML2::IXMLDOMNodePtr pGroup = pGroupList->Getitem(1);
	MSXML2::IXMLDOMNodeListPtr pNodeList = pGroup->GetchildNodes();
	std::vector<CString> Path;
	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
	MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
	CString strName = pNode->GetnodeName();
	if (strName.CompareNoCase("Node") != 0)
	continue;
	CAstXml::TraverseAstXmlNodeParam(pXmlDoc, pNode, Path, TRUE);
	}

	//add by xjt in 2019.1.19	
	//删除vector中的重复元素
	for (auto it = Path.begin(); it != Path.end();)
	{
	auto it1 = find(Path.begin(), Path.end(), *it);
	if (it1 != it)
	it = Path.erase(it);
	else
	++it;
	}
	//end

	KmZipUtil zip;
	auto temppath = strNewSubPath + "AddorUpdate.zip";
	if (Path.size() > 0)
	{
	if (zip.CreateZipFile(temppath))
	{
	for (auto it = Path.begin(); it != Path.end(); it++)
	{
	zip.ZipFile(it->GetBuffer(), NULL);
	}
	zip.CloseZipFile();
	}

	bSucc = m_FTPInterface.UpLoad(temppath, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strRelDir, "AddorUpdate.zip");
	if (!bSucc)
	{
	MessageBox(NULL, "上传失败！", APS_MSGBOX_TITIE, MB_OK);
	return;
	}
	}

	*/

	CString strInput = "<?xml version = \"1.0\" encoding = \"UTF-8\"?><part><xmlFileName>PBOM.xml</xmlFileName><fileName>"
		+ strFileName + "</fileName>" +
		"<partNumber>" + CWindChillSetting::m_strpartFirstName + "</partNumber>" +
		"</part>";

	auto Xmlcontent = m_WebserverInterface.checkInPbom(strInput);
	bool bSucess = CWindChillXml::ParseResult(Xmlcontent);
	if (bSucess)
	{
		MessageBox(NULL, "检入成功", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
	}
	else
	{
		MessageBox(NULL, "检入失败", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST );
	}

	if (ExistDir(strNewSubPath)) 
	{
		DeleteDir(strNewSubPath);
	}
}


static CArray<CAsmStep*, CAsmStep*> *steps = NULL;


void CwindchillObject::UserPropIsEdit()
{
	IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();

	if (!doc)
	{
		//begin add by xjt in 2019.2.19 for 70785
		auto m_hwnd = AfxGetMainWnd()->m_hWnd;
		::SendMessage(m_hwnd, MSG_FILE_NEW, NULL, LPARAM(""));
		doc = m_pApplication->GetCurrentDocment();
		//end add
	}

	CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();
	
	auto product =pModel->GetProduct();

	for (int i = 0; i < CWindChillSetting::m_strRelMatch.GetSize(); ++i)
	{
		CString strName = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;
		bool bShow = 1;
		bool bEdit = 0;
		auto Process = pModel->GetProcess();
		CAsmInst *pAsmRoot = Process->GetAsmInst();

		
		if (pAsmRoot != NULL)
		{
			bool flag =false;;
			/*for (int i = 0; i < CWindChillSetting::m_strRelMatch.GetSize(); ++i)
			{
				CString strName1 = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;
				if(strName1 == strName)
				{
					flag = true;
					break;
				}
			}

			if (flag)
				pAsmRoot->CAsmInst::ModifyPropStateByDisName(strName, 1, 1);
			else*/
				pAsmRoot->CAsmInst::ModifyPropStateByDisName(strName, bShow,bEdit );
		}
	}
	

	/*
	for (auto it = CWindChillSetting::PROCUDEID.begin(); it != CWindChillSetting::PROCUDEID.end(); ++it)
	{
		CSolid* Solid = product->LookupSolid(atoi(it->c_str()));

		if (Solid)
		{
			for (int i = 0; i < CWindChillSetting::m_strRelMatch.GetSize(); ++i)
			{
				CString strName = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;
				CString strValue = CWindChillSetting::m_strRelMatch.GetAt(i).m_strValue;
				
				bool bShow = 1;
				bool bEdit = 0;

				CAPSProp Baseprop(strName, "", strValue, 1, 0);
				
				Solid->SeekObjPropByDisName(strName, Baseprop);
				//Solid->UpdateProp(Baseprop.GetName(), CString("1"));
				Solid->CSolid::ModifyPropStateByDisName(Baseprop.GetName(), 1, 0);
			}
		}
	}
	
	*/
	for (auto it =CWindChillSetting::GYJID.begin();it!=CWindChillSetting::GYJID.end();++it)
	{
		CSolid* Solid = product->LookupSolid(atoi((*it).c_str()));

		if (Solid)
		{
			for (auto i = 0; i < CWindChillSetting::m_strUpdatePropMatch.GetCount() - 1; ++i)
			{
				CString strName = CWindChillSetting::m_strUpdatePropMatch.GetAt(i).m_strName;
				CString strValue = CWindChillSetting::m_strUpdatePropMatch.GetAt(i).m_strValue;

				CAPSProp Baseprop(strName, "", "", 1, 1);


				CAPSProp prop;
				Solid->SeekObjProp(strName,prop);
				Solid->SeekObjPropByDisName(strName,prop);
				Solid->SeekObjProp2(strName,prop);
				auto str =prop.ConvertTo();


				Solid->UpdateProp(Baseprop.GetName(), str);
				Solid->CSolid::ModifyPropStateByDisName(Baseprop.GetName(), 1, 1);


				for (int i = 0; i < CWindChillSetting::m_strRelMatch.GetSize(); ++i)
				{
					CString strName = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;

					bool bShow = 1;
					bool  bEdit= 1;
					//从windhcill传过来的工艺件属性 的 编号 不能修改
					if (strName.CompareNoCase("编号")==0)
					{
						Solid->CSolid::ModifyPropStateByDisName(strName, bShow,bEdit );
					}
					else
					{
						Solid->CSolid::ModifyPropStateByDisName(strName, bShow, bEdit);
					}
				}
			}
		}
	}

}
//add by xingjt for 更新零部件
BOOL CwindchillObject::UpdatePart()
{
	IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();

	//test();

	if (!doc)
	{
		//begin add by xjt in 2019.2.19 for 70785
		auto m_hwnd = AfxGetMainWnd()->m_hWnd;
		::SendMessage(m_hwnd, MSG_FILE_NEW, NULL, LPARAM(""));
		doc = m_pApplication->GetCurrentDocment();
		//end add
	}
	CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();

	auto array = pModel->GetSelects(1);

	for (auto i = 0; i < array->GetCount(); ++i)
	{
		auto ID = array->GetAt(i);

		auto Product = pModel->GetProduct();

		auto Solid = Product->LookupSolid(ID);
		if (Solid)
		{
			CString Type = "6";

			CAPSProp prop;

			Solid->SeekObjPropByDisName("*编号",prop);
			auto Name =prop.ConvertTo();

			auto  Xmlcontent = m_WebserverInterface.getDocInfoOfContent(Type, Name);

			if (Xmlcontent.IsEmpty())
			{
				return S_FALSE;
			}

			auto path = CWindChillXml::GetPath(Xmlcontent);
			//返回压缩包名称  需要在ftp下载

			//CWindChillSetting::m_strpartFirstName = path;

			CString strFTPPath, strFTPName;
			//下载文件的路径
			strFTPPath = "/downloadContent/";
			strFTPName = path;

			TCHAR TempDir[1024];
			GetTempPath(1024, TempDir);
			CString sTempDir(TempDir);
			sTempDir = sTempDir + _T("KM3DCAPP-AWORK\\");
			CString strlocal = sTempDir + strFTPName;
			if (!ExistDir(sTempDir))
			{
				if (!::CreateDirectory(sTempDir, NULL))
				{
				}
			}
			if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
			{
				MessageBox(NULL, _T("ftp连接失败！"), APS_MSGBOX_TITIE, MB_OK);

				return S_FALSE;
			}

			BOOL bSucc = m_FTPInterface.DownLoad(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
			if (!bSucc)
			{
				CString strErrMsg = _T("下载文件失败！");
				//strErrMsg.Format(_T("下载文件【%s】失败！"), strFTPName);
				MessageBox(NULL, strErrMsg, APS_MSGBOX_TITIE, MB_OK);
				return S_FALSE;
			}

			if (strFTPName.Find(_T(".zip")) >= 0)  //传递的压缩文件包
			{
				int nP = strFTPName.ReverseFind(_T('.'));
				CString strDir = sTempDir + strFTPName.Left(nP) + "\\";  //压缩文件解压后的文件夹
				KmZipUtil kmZipUtil;
				if (kmZipUtil.OpenZipFile(strlocal, KmZipUtil::ZipOpenForUncompress))
				{
					if (!ExistDir(strDir))
					{
						if (!::CreateDirectory(strDir, NULL))
						{
						}
					}
					kmZipUtil.UnZipAll(strDir);
					kmZipUtil.CloseZipFile();
					//DeleteFile(strlocal);
					CFileFind find;


					auto testcontent = m_WebserverInterface.getBom("1", Name);

					TiXmlDocument xml;

					auto isopen = xml.Parse(testcontent);

					if (!isopen)
					{
						return S_FALSE;
					}

					std::vector<PartInfo> partInfo;
					TiXmlElement *root = xml.FirstChildElement();
					if (root == NULL || root->FirstChildElement() == NULL)
					{
						xml.Clear();
						return FALSE;
					}

					auto elem = root->FirstChildElement()->FirstChildElement();
					while (elem != NULL)
					{
						PartInfo part(elem);
						partInfo.push_back(part);
						elem = elem->NextSiblingElement();
					}

					auto  parttop = partInfo.begin();
					for (auto partitem = partInfo.begin(); partitem != partInfo.end(); ++partitem)
					{
						if (CString(partitem->value["Number"].c_str()).CompareNoCase(Name) == 0)
						{
							parttop = partitem;
						}
					}

					CString ProductPath = parttop->value["epmfilename"].c_str();
					
					auto partdir = strDir + GetMainFileName(path) + _T("\\");
					auto isFind = find.FindFile(partdir + ProductPath);
					if (isFind)
					{
						isFind = find.FindNextFile();
						auto partpath = find.GetFilePath();
						auto m_hwnd = AfxGetMainWnd()->m_hWnd;

						CString strxml;
						if (!ConvertModel(partpath, strxml))
						{
							return FALSE;
						}
						
						if (PathFileExists(strxml))
						{

							if (CWindChillSetting::m_iConvertMode != 1)//exchange 
							{
								//把属性写入xml中

								auto LowPath =ProductPath.MakeLower();

	
								if (LowPath.Find("catpart")!=-1)  //如果为零件
								{
									CPBOMUtil util;
									util.PbomExchange(strxml,testcontent,Name);
									//documentAnsiToutf8(strxml);
								}
								else
								{
									CPBOMUtil util;
									util.CreatePbomExchange(strxml, testcontent, Name);
									documentAnsiToutf8(strxml);
								}
							}
							else
							{
								CPBOMUtil util;
								//strXml = "C:\\Users\\Administrator\\Desktop\\39909B84-206E-4BE4-BD29-631CD9B81839\\D6101000.xml";

								util.CreatePbomIop(strxml, testcontent, Name);
								documentAnsiToutf8(strxml);
							}

							CStringArray value;
							value.Add(to_string(static_cast<long long>(ID)).c_str());
							value.Add(strxml);
							::SendMessage(m_hwnd, MSG_UPDATE_PART, NULL, LPARAM(&value));
							
							
							{
								IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();

								if (!doc)
								{
									//begin add by xjt in 2019.2.19 for 70785
									auto m_hwnd = AfxGetMainWnd()->m_hWnd;
									::SendMessage(m_hwnd, MSG_FILE_NEW, NULL, LPARAM(""));
									doc = m_pApplication->GetCurrentDocment();
									//end add
								}

								CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();

								auto product =pModel->GetProduct();

								if(product)
								{
									auto Solid =product->LookupSolid(ID);

									if (Solid)
									{
										for (int i = 0; i < CWindChillSetting::m_strRelMatch.GetSize(); ++i)
										{
											CString strName = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;

											bool bShow = 1;
											bool  bEdit= 1;
											//从windhcill传过来的工艺件属性 的 编号 不能修改
											if (strName.CompareNoCase("编号")==0)
											{
												Solid->CSolid::ModifyPropStateByDisName(strName, bShow,bEdit );
											}
											else
											{
												Solid->CSolid::ModifyPropStateByDisName(strName, bShow, bEdit);
											}
										}
									}

								}
							}
							
							
						}
					}
				}
				else
				{
					MessageBox(NULL, _T("解压失败！"), APS_MSGBOX_TITIE, MB_OK);
					return S_FALSE;
				}
			}



//是否删除ftp文件
#ifdef	DeleteFTPFILE
			if (bSucc)
			{
				bSucc = m_FTPInterface.Delete(strlocal, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strFTPPath, strFTPName, CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd());
				if (!bSucc)
				{
					CString strErrMsg = _T("删除FTP文件失败！");
					MessageBox(NULL,strErrMsg, APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
					return FALSE;
				}
			}
#endif
		}
	}
	return TRUE;
}
//end add

void CwindchillObject::CheckinProc(CString kmapxpath, CString pdfpath)
{

	auto  PropList = m_pProcess->GetUserPropList();
	auto pos = PropList->SeekProp(_T("工艺方式"));
	CString craftMethod;
	if (pos)
	{
		auto prop = PropList->GetAt(pos);
		craftMethod = prop.ConvertTo();
	}
	

LAST:
	CString strPDFName = "";
	//PDF文件
	CString strOutPDF;

	CString TemplateAry = _T("PDFTemplate");
	CString sPath = "";
	sPath.Format(_T("%s%s%s"), GetModuleDir(), _T("\\Resources\\Chs\\"), TemplateAry);
	CString sPathFile = sPath;

	sPathFile.Append(_T("\\"));
	sPathFile.Append(_T("工艺规程模板.xml"));

	CApsWaitCur waiter;
	int iRet = OutputPDF(sPathFile, 1, strOutPDF);
	
	if (0 == iRet)
	{
		return;
	}
	if (2 == iRet)//返回上一步
	{
		goto LAST;
	}

	strPDFName = strOutPDF.Right(strOutPDF.GetLength() - strOutPDF.ReverseFind(_T('\\')) - 1);
	if (strPDFName.IsEmpty())
		return;

	pdfpath = strOutPDF;
	if (!ExistFile(kmapxpath))
	{
		MessageBox(NULL, _T("kmapx文件不存在！"), APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	auto dirty=m_pModel->m_bModify;
	

	if (!ExistFile(pdfpath))
	{
		MessageBox(NULL, _T("请输出pdf文件！"), APS_MSGBOX_TITIE, MB_OK);
		return;
	}

	CString strFileName = GetFileName(kmapxpath);

	CString strDir = kmapxpath.Left(kmapxpath.ReverseFind(_T('\\')) + 1);
	CString strNewSubPath;

	if (!GenAlonePath(strDir, strNewSubPath))
	{
		return;
	}

	//创建路径
	if (!CreateDirectory(strNewSubPath, NULL))
	{
		return;
	}

	//在此处修改上传至ftp的相对路径
	CString relativpath = _T("checkin/doc/");
	CString strRelDir = relativpath ;//+ GetRelativeDir();

	KmZipUtil kmZipUtil;
	if (kmZipUtil.OpenZipFile(kmapxpath, KmZipUtil::ZipOpenForUncompress))
	{

		if (!ExistDir(strNewSubPath))
		{
			if (!::CreateDirectory(strNewSubPath, NULL))
			{
			}
		}

		kmZipUtil.UnZipAll(strNewSubPath);
	}
	kmZipUtil.CloseZipFile();

	CString strXmlName = "";
	//组装工艺xml，不是PBOM.xml
	GenProcXml(strNewSubPath);
	GenMbomXml(strNewSubPath);

	if (PathFileExists(strNewSubPath + "Proc.xml") && PathFileExists(strNewSubPath + "mbom.xml"))
	{
		KmZipUtil zip;
		CString zipPath = pdfpath.Left(pdfpath.GetLength() - 3) + "zip";
		if (zip.CreateZipFile(zipPath))
		{
			zip.ZipFile(kmapxpath, NULL);

			auto docInfoPath =GetFilePath(pdfpath)+GetMainFileName(pdfpath)+".xml";
			if (ExistFile(pdfpath) &&ExistFile(docInfoPath))
			{
				zip.ZipFile(pdfpath, NULL);
				auto NewFileName =GetFilePath(pdfpath)+"docInfo.xml";
				rename(docInfoPath,NewFileName);
				zip.ZipFile(NewFileName, NULL);
			}
			
			zip.ZipFile(strNewSubPath + "Proc.xml", NULL);
			zip.ZipFile(strNewSubPath + "mbom.xml", NULL);
		}
		zip.CloseZipFile();


		if (!m_FTPInterface.Connect(CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), CWindChillSetting::GetStrFTPUserName(), CWindChillSetting::GetStrFTPPasswd()))
		{
			MessageBox(NULL, _T("ftp连接失败！"), APS_MSGBOX_TITIE, MB_OK);
			return;
		}


		strFileName = GetFileName(zipPath);
		BOOL bSucc = m_FTPInterface.UpLoad(zipPath, CWindChillSetting::GetStrFTPURL(), CWindChillSetting::GetStrFTPPort(), strRelDir, strFileName);
		if (!bSucc)
		{
			MessageBox(NULL, "上传失败！", APS_MSGBOX_TITIE, MB_OK);
			return;
		}

		
		CString strInput =  
			CString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
			"<checkInDoc>"+
			"<partNumber>"+CWindChillSetting::m_strpartFirstName+"</partNumber>"+
			"<fileName>"+GetFileName(zipPath)+"</fileName>"+
			"<craftMethod>"+ craftMethod +"</craftMethod>"
			"</checkInDoc>";

		auto Xmlcontent = m_WebserverInterface.CheckInDoc("4", strInput);
		bool bSucess = CWindChillXml::ParseResult(Xmlcontent);
		if (bSucess)
		{
			MessageBox(NULL, "检入成功", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST );
		}
		else
		{
			MessageBox(NULL, "检入失败", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST );
		}

		if (ExistDir(strNewSubPath))
		{
			DeleteDir(strNewSubPath);
		}
	}
}




STDMETHODIMP CwindchillObject::OnPluginCommand(long nID)
{
	IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();

	if (!doc)
	{
		//begin add by xjt in 2019.2.19 for 70785
		auto m_hwnd = AfxGetMainWnd()->m_hWnd;
		::SendMessage(m_hwnd, MSG_FILE_NEW, NULL, LPARAM(""));
		doc = m_pApplication->GetCurrentDocment();
		//end add
	}
	CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();
	m_pProcess = pModel->GetCurApsProcess();

	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		//CwindchillObjectXml("D:\\mbom\\减速机.kmapx\\");
		if (nID == m_nLoginCmd)
		{
			CDlgLogIn dlg(g_islogin);
			dlg.DoModal();
			g_islogin = dlg.GetloginStae();
		}
		else if (nID == m_nPBomEditCmdl)
		{
			CDlgPBOMEdit dlg(pModel, QueryType_PBOM, m_pApplication);
			dlg.SetCaption("PBOM编辑查询");
			dlg.DoModal();
			UserPropIsEdit();
		}
		else if (nID == m_nPBOMCmd)
		{
			CDlgPBOMEdit dlg(pModel, QueryType_EBOM, m_pApplication);
			dlg.SetCaption("PBOM创建查询");
			dlg.DoModal();
			UserPropIsEdit();
		}
		else if (nID == m_nMaterialCmd)
		{
			CDlgMaterialEdit dlg(pModel);
			dlg.DoModal();
		}
		else if (nID == m_nCheckinPBomCmd)
		{
			auto pProduct = pModel->GetProduct();
			if (pProduct)
			{
				auto isEmpty = pProduct->GetIsEmpty();
				if (isEmpty)
				{
					MessageBox(NULL, "当前为空文档，请引入产品或打开文件！", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
					return S_OK;
				}
			}

			CString strlocal = "";
			auto bRet = GetCurFileName(pModel, true, strlocal);
			if (!bRet || strlocal.IsEmpty())
			{
				return S_OK;
			}
			CheckinPBom(strlocal);
		}
		else if (nID == m_CreatProcCmd)
		{
			CDlgPBOMEdit dlg(pModel, QueryType_RULE, m_pApplication);
			dlg.SetCaption("PBOM查询");
			dlg.DoModal();

			UserPropIsEdit();
		}
		else if (nID == m_ProcQueryCmd)
		{
			CDlgProcEdit dlg;
			dlg.DoModal();
			
			UserPropIsEdit();
		}
		else if (nID == m_ProcCheckInCmd)
		{
			auto pProduct = pModel->GetProduct();
			if (pProduct)
			{
				auto isEmpty =pProduct->GetIsEmpty();
				if (isEmpty)
				{
					MessageBox(NULL, "当前为空文档，请引入产品或打开文件！", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
					return S_OK;
				}
			}


			CString strlocal = "";
			m_pModel = pModel;
			auto bRet = GetCurFileName(pModel, true, strlocal);
			if (!bRet || strlocal.IsEmpty())
			{
				return S_OK;
			}

			//将更改单的信息添加到kmapx文件
			if (m_pProcess)
			{	
				auto userlist = m_pProcess->GetUserPropList();
				for (auto it = CWindChillSetting::ReviewValue.begin(); it != CWindChillSetting::ReviewValue.end(); ++it)
				{
					CAPSProp propAdd;

					auto pos =userlist->SeekProp(it->first,&propAdd);
					if (pos)   //说明有配置
					{
						propAdd.SetType(1);
						propAdd.ModifyByType(it->second);
						userlist->ModifyProp(it->first, propAdd);
					}
				}
			}

			CheckinProc(strlocal, CWindChillSetting::GetPdfPath_Rule());
		}
		else if (nID == m_ProcPdfEditCmd)
		{
			auto pProduct = pModel->GetProduct();
			if (pProduct)
			{
				auto isEmpty = pProduct->GetIsEmpty();
				if (isEmpty)
				{
					MessageBox(NULL, "当前为空文档，请引入产品或打开文件！", APS_MSGBOX_TITIE, MB_OK | MB_TOPMOST);
					return S_OK;
				}

			}

			CString TemplateAry = _T("PDFTemplate");
			CString sPath = _T("");//::GetConfigFileFullPath(TemplateAry);
			sPath.Format(_T("%s%s%s"), GetModuleDir(), _T("\\Resources\\Chs\\"), TemplateAry);
			CString sPathFile = sPath;
			sPathFile.Append(_T("\\"));
			sPathFile.Append(_T("工艺规程模板.xml"));
			OpenPdfEditPage(sPathFile, pModel, 1);
		}
		else if (nID == m_ImportEquiesCmd)
		{
			auto array = pModel->GetSelects(2);
			for (auto i = 0; i < array->GetCount(); ++i)
			{
				auto ID = array->GetAt(i);

				CDlgAddMaterial dlg(ResourceType_EQUIPTOOL, ID);
				if (dlg.DoModal() == IDOK)
				{

				}
			}
		}
		else if (nID == m_UpdatePartCmd)
		{
			UpdatePart();
			//UserPropIsEdit();
		}
		else if (nID==m_CollectCmd)
		{
			if (pModel)
			{
				CDlgCollect dlg(m_pApplication);
				if (dlg.DoModal()==IDOK)
				{
				}
			}
			
		}
		else if (nID ==m_ChangeCmd)
		{
			if (pModel)
			{
				CDlgChange dlg(m_pApplication);
				if (dlg.DoModal()==IDOK)
				{
				}
				
			}
		}
		return S_OK;
}

STDMETHODIMP CwindchillObject::OnUpdatePluginCommandUI(long nID, long* state)
{
	//如果同时加载多个插件，会引发问题
	if (nID == m_nLoginCmd || nID == m_nPBOMCmd || nID == m_nPBomEditCmdl ||
		nID == m_nMaterialCmd || nID == m_nProcRuleCmd || nID == m_nCheckinPBomCmd ||
		nID == m_CreatProcCmd || nID == m_ProcQueryCmd || nID == m_ProcCheckInCmd ||
		nID == m_ProcPdfEditCmd ||nID == m_ImportEquiesCmd ||nID == m_UpdatePartCmd||
		nID==m_CollectCmd || nID==m_ChangeCmd
		)
	{
		IAPSDocumentPtr doc = m_pApplication->GetCurrentDocment();
		if (!doc)
		{
			return S_OK;
		}

		CAPSModel *pModel = (CAPSModel*)doc->GetAPSModel();
		auto isshowImportEquies = false;
		auto isshowUpdatePart = false;

		auto arrayUpdatePart = pModel->GetSelects(1);
		auto arrayImportEquies = pModel->GetSelects(2);
		//auto arrayImportEquies = m_pProcess->GetCurStep();

		if (arrayUpdatePart->GetCount() > 0)
		{
			isshowUpdatePart = true;
		}

		/*
		if (arrayImportEquies->GetCount() > 0)
		{
			auto ID = arrayImportEquies->GetAt(0);
			if (IsAsmStep(ID))
			{
				if (m_pProcess)
				{
					auto step = m_pProcess->GetCurStep();

					if (step)
					{
						CArray<CAsmStep *, CAsmStep *>* pSubSteps = step->GetSubSteps();
						if (pSubSteps->GetSize() == 0)
						{
							isshowImportEquies = true;
						}
					}
				}
			}
		}
		*/
		if (nID == m_nLoginCmd)
		{
			*state |= PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_nPBOMCmd && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_nMaterialCmd && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_nPBomEditCmdl && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_nCheckinPBomCmd && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_CreatProcCmd && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_ProcQueryCmd && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_ProcCheckInCmd && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_ProcPdfEditCmd && g_islogin)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_ImportEquiesCmd && g_islogin /*&& isshowImportEquies*/)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_UpdatePartCmd && g_islogin &&isshowUpdatePart)
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_CollectCmd && g_islogin )
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else if (nID == m_ChangeCmd && g_islogin )
		{
			*state = PLUGIN_CMD_UI_ENABLE;
		}
		else
		{
			*state &= ~PLUGIN_CMD_UI_ENABLE;
		}
	}

	return S_OK;
}

STDMETHODIMP CwindchillObject::OnMainFrameCreated()
{
	IAPSRibbonBarPtr pRibbonBar = m_pApplication->GetRibbonBar();
	IAPSRibbonFactoryPtr pFactory = pRibbonBar->GetRibbonFactory();
	IAPSRibbonTabPtr pTab = pFactory->CreateRibbonTab();

	IAPSRibbonGroupPtr pPanel1 = pFactory->CreateRibbonGroup();
	IAPSRibbonButtonPtr pBtn10 = pFactory->CreateRibbonButton();

	pBtn10->put_Label(_bstr_t("登录Windchill"));
	pBtn10->put_Enable(true);
	m_nLoginCmd = pBtn10->GetCmdID();		//记录命令ID
	pPanel1->AddChild(pBtn10);
	//pPanel1->put_Visible(VARIANT_FALSE);
	pPanel1->put_Label(_bstr_t("登录"));

	IAPSRibbonGroupPtr pPanel2 = pFactory->CreateRibbonGroup();
	IAPSRibbonButtonPtr pBtn20 = pFactory->CreateRibbonButton();

	pBtn20->put_Label(_bstr_t("创建PBOM"));
	(IAPSRibbonElementPtr)pBtn20->put_Enable(false);
	m_nPBOMCmd = pBtn20->GetCmdID();		//记录命令ID
	pPanel2->AddChild(pBtn20);
	IAPSRibbonButtonPtr pBtn21 = pFactory->CreateRibbonButton();
	pBtn21->put_Label(_bstr_t("PBOM编辑"));
	(IAPSRibbonElementPtr)pBtn21->put_Enable(false);
	m_nPBomEditCmdl = pBtn21->GetCmdID();		//记录命令ID
	pPanel2->AddChild(pBtn21);

	IAPSRibbonButtonPtr pBtn22 = pFactory->CreateRibbonButton();
	pBtn22->put_Label(_bstr_t("PBOM检入"));
	(IAPSRibbonElementPtr)pBtn22->put_Enable(false);
	m_nCheckinPBomCmd = pBtn22->GetCmdID();		//记录命令ID
	pPanel2->AddChild(pBtn22);
	pPanel2->put_Label(_bstr_t("PBOM"));

	IAPSRibbonGroupPtr pPanel3 = pFactory->CreateRibbonGroup();
	IAPSRibbonButtonPtr pBtn30 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn30->put_Enable(false);
	pBtn30->put_Label(_bstr_t("材料定额编辑"));
	m_nMaterialCmd = pBtn30->GetCmdID();		//记录命令ID
	pPanel3->AddChild(pBtn30);
	pPanel3->put_Label(_bstr_t("材料定额"));

	IAPSRibbonGroupPtr pPanel4 = pFactory->CreateRibbonGroup();
	IAPSRibbonButtonPtr pBtn40 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn40->put_Enable(false);
	pBtn40->put_Label(_bstr_t("装配工艺创建"));
	m_CreatProcCmd = pBtn40->GetCmdID();		//记录命令ID
	pPanel4->AddChild(pBtn40);

	IAPSRibbonButtonPtr pBtn41 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn41->put_Enable(false);
	pBtn41->put_Label(_bstr_t("装配工艺编辑"));
	m_ProcQueryCmd = pBtn41->GetCmdID();		//记录命令ID
	pPanel4->AddChild(pBtn41);

	IAPSRibbonButtonPtr pBtn42 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn42->put_Enable(false);
	pBtn42->put_Label(_bstr_t("装配工艺检入"));
	m_ProcCheckInCmd = pBtn42->GetCmdID();		//记录命令ID
	pPanel4->AddChild(pBtn42);
	pPanel4->put_Label(_bstr_t("装配工艺"));


	IAPSRibbonGroupPtr pPanel5 = pFactory->CreateRibbonGroup();
	IAPSRibbonButtonPtr pBtn50 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn50->put_Enable(false);
	pBtn50->put_Label(_bstr_t("工艺规程pdf编辑"));
	m_ProcPdfEditCmd = pBtn50->GetCmdID();		//记录命令ID
	pPanel5->AddChild(pBtn50);

	IAPSRibbonButtonPtr pBtn51 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn51->put_Enable(false);
	pBtn51->put_Label(_bstr_t("引入工艺装备"));
	m_ImportEquiesCmd = pBtn51->GetCmdID();		//记录命令ID
	pPanel5->AddChild(pBtn51);

	IAPSRibbonButtonPtr pBtn52 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn52->put_Enable(false);
	pBtn52->put_Label(_bstr_t("更新零部件"));
	m_UpdatePartCmd = pBtn52->GetCmdID();		//记录命令ID
	pPanel5->AddChild(pBtn52);

	IAPSRibbonGroupPtr pPanel6 = pFactory->CreateRibbonGroup();
	IAPSRibbonButtonPtr pBtn60 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn60->put_Enable(false);
	pBtn60->put_Label(_bstr_t("汇总"));
	m_CollectCmd = pBtn60->GetCmdID();		//记录命令ID
	pPanel6->AddChild(pBtn60);

	IAPSRibbonButtonPtr pBtn61 = pFactory->CreateRibbonButton();
	(IAPSRibbonElementPtr)pBtn61->put_Enable(false);
	pBtn61->put_Label(_bstr_t("更改单"));
	m_ChangeCmd = pBtn61->GetCmdID();		//记录命令ID
	pPanel6->AddChild(pBtn61);

	pTab->put_Label(_bstr_t(_T("Windchill集成")));
	pTab->AddGroup(pPanel1);
	pTab->AddGroup(pPanel2);
	pTab->AddGroup(pPanel3);
	pTab->AddGroup(pPanel4);
	pTab->AddGroup(pPanel5);
	pTab->AddGroup(pPanel6);
	pRibbonBar->AddRibbonTab(pTab);

	//pRibbonBar->Layout(); 统一在创建端刷新TAB

	m_PdfConfigDatas = new CArray<CAPSGridPropData*>;
	m_ConfigProcPDFName = new std::vector<std::map<CString, UINT>>;

	return S_OK;
}

STDMETHODIMP  CwindchillObject::raw_GetEnable(VARIANT_BOOL * pbEnable)
{
	return S_OK;
}

STDMETHODIMP CwindchillObject::raw_GetCookie(long * pdwCookie)
{
	return S_OK;
}

