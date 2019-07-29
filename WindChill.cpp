// WindChill.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "WindChill.h"
#include <initguid.h>
#include "WindChill_i.c"
#include "WindChillSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWindChillApp
class CWindChillModule :
	public ATL::CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_WindChillLib);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WindChill, "{71217AB9-ECCE-433C-89AA-2D1CAF8C717E}");
};

CWindChillModule _AtlModule;

BEGIN_MESSAGE_MAP(CWindChillApp, CWinApp)
END_MESSAGE_MAP()


// CWindChillApp 构造

CWindChillApp::CWindChillApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


CWindChillApp theApp;

BOOL CWindChillApp::InitInstance()
{
	COleObjectFactory::RegisterAll();
	CWinApp::InitInstance();

	//初始化配置
	CWindChillSetting* pSetting = CWindChillSetting::GetAppSettings();
	return TRUE;
}

// DllCanUnloadNow - Allows COM to unload DLL
#if !defined(_WIN32_WCE) && !defined(_AMD64_) && !defined(_IA64_)
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")
#else
#if defined(_X86_) || defined(_SHX_)
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer,PRIVATE")
#endif // (_X86_)||(_SHX_)
#endif // !_WIN32_WCE && !_AMD64_ && !_IA64_ 

STDAPI DllCanUnloadNow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_AtlModule.GetLockCount() > 0)
		return S_FALSE;
	return S_OK;
}

// DllGetClassObject - Returns class factory
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (S_OK == _AtlModule.GetClassObject(rclsid, riid, ppv))
		return S_OK;
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_AtlModule.UpdateRegistryAppId(TRUE);
	HRESULT hRes2 = _AtlModule.RegisterServer(TRUE);
	if (hRes2 != S_OK)
		return hRes2;
	if (!COleObjectFactory::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);
	return S_OK;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_AtlModule.UpdateRegistryAppId(FALSE);
	HRESULT hRes2 = _AtlModule.UnregisterServer(TRUE);
	if (hRes2 != S_OK)
		return hRes2;
	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);
	return S_OK;
}
