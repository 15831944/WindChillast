// WindChillObject.h : CWindChillObject 的声明

#pragma once

#include "resource.h"       // 主符号
#include "WebServiceInterface.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;

class  CProcess;

// CWindChillObject

class ATL_NO_VTABLE CwindchillObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CwindchillObject, &CLSID_WindChillObject>,
//	public IWindChillObject,
	public ISupportErrorInfo,
	public IDispatchImpl<IWindChillObject, &IID_IWindChillObject, &LIBID_WindChillLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IKmAPSPlugin, &__uuidof(IKmAPSPlugin), &LIBID_KmApsToolLib, /* wMajor = */ 1, /*wMinor =*/ 0>,
	public IDispEventImpl < 1, CwindchillObject, &__uuidof(_IAPSApplicationEvents), &LIBID_KmApsToolLib, /* wMajor = */ 1, /*wMinor =*/ 0 >
{
public:
	CwindchillObject()
	{
		 m_pApplication=NULL;
		 m_pPSM=NULL;
		 m_pModel = NULL;

		 m_nLoginCmd =0;
		 m_nPBOMCmd=0;
		 m_nPBomEditCmdl=0;
		 m_nMaterialCmd=0;
		 m_nProcRuleCmd=0;
		 m_nCheckinPBomCmd=0;
		

		 m_CreatProcCmd = 0;
		 m_ProcQueryCmd = 0;
		 m_ProcCheckInCmd = 0;
		 m_ProcPdfEditCmd = 0;

		 m_ImportEquiesCmd=0;
		 m_UpdatePartCmd = 0;

		 m_pProcess = NULL;
		
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WindChillOBJECT)

DECLARE_NOT_AGGREGATABLE(CwindchillObject)

BEGIN_COM_MAP(CwindchillObject)
	//COM_INTERFACE_ENTRY(IDispatch)

	COM_INTERFACE_ENTRY(IWindChillObject)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IKmAPSPlugin)
	COM_INTERFACE_ENTRY2(IDispatch, IKmAPSPlugin)
END_COM_MAP()

BEGIN_SINK_MAP(CwindchillObject)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 1, OnAppInit)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 2, OnAppClose)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 4, OnDocumentOpening)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 5, OnDocumentOpened)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 6, OnDocumentSaving)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 7, OnDocumentSaved)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 10, OnPluginCommand)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 11, OnUpdatePluginCommandUI)
	SINK_ENTRY_EX(1, __uuidof(_IAPSApplicationEvents), 12, OnMainFrameCreated)
END_SINK_MAP()

STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// 实现IKmAPSPlugin
	STDMETHOD(raw_GetName)(BSTR* bstrName);
	STDMETHOD(raw_GetComment)(BSTR* bstrName);
	STDMETHOD(raw_Connect)(IDispatch* app);
	STDMETHOD(raw_Disconnect)();
	STDMETHOD(raw_GetEnable)(long bEnable);
	STDMETHOD(raw_GetEnable)(VARIANT_BOOL* bEnable);
	STDMETHOD(raw_GetCookie) (long * pdwCookie);
	STDMETHOD(raw_GetDownFilePath) (long* strkey, BSTR* bstrFile);
	STDMETHOD(raw_AddOrUpdateSolid) (UINT ID, BSTR bstrFile);

	STDMETHOD(raw_IsChangeGetDownFileMode)(void);


	// 实现_IAPSApplicationEvents
	STDMETHOD(OnAppInit)(IAPSApplication* pdispApp);
	STDMETHOD(OnAppClose)();
	STDMETHOD(OnDocumentOpening)(BSTR bstrPath);
	STDMETHOD(OnDocumentOpened)(IAPSDocument* doc);
	STDMETHOD(OnDocumentSaving)(IAPSDocument* doc);
	STDMETHOD(OnDocumentSaved)(IAPSDocument* doc, BSTR bstrPath);
	STDMETHOD(OnPluginCommand)(long nID);
	STDMETHOD(OnUpdatePluginCommandUI)(long nID, long* state);
	STDMETHOD(OnMainFrameCreated)();
public:
	void UserPropIsEdit();

	BOOL UpdatePart();

	void CheckinPBom(CString strlocal);
	void CheckinProc(CString kmapxpath, CString pdfpath);
	CString GetRelativeDir();
	CString GetUid();

	void GenProcXml(CString strNewSubPath);
	void TraverseAsmObj(MSXML2::IXMLDOMNodePtr pProcedure, CString strNewSubPath);
	void AttachAsmObjAttr(MSXML2::IXMLDOMNodePtr pProcedure, CString strID, CString strNewSubPath);
	BOOL TraverseNodeParams(MSXML2::IXMLDOMNodePtr pProcedure, MSXML2::IXMLDOMNodePtr pNode, CString strID);//组Proc.xml时用

	void GenMbomXml(CString strNewSubPath);
	int OutputPDF(CString sPathFile, int iType, CString &sOutputFilePath);
	std::map<CString, CString> GetStepinfo(CAsmStep * step);
	std::map<CString, CString> GetUserStepinfo(CAsmStep * step);
	
	std::vector<std::map<CString, CString>> GetIdinfo(CAsmStep * step);
	std::vector<std::map<CString, CString>>  GetSolidinfo(CAsmStep * step);
	
	std::map<CString, std::vector<std::map<CString, CString>>>   GetGridData(CArray<CAPSGridPropData*>* actionGridDatas);
	
	std::map<CAsmStep *,std::map<CString, CString>> GetChildStepInfo(CAsmStep * step);

	std::vector<std::map<CString, CString>> GetChildStepEquies(CAsmStep * pStep);

	std::vector<CSolid *> GetStepSolid(CAsmStep * step);

	BOOL OpenPdfEditPage(CString sPathFile, CAPSModel* pModel, int iType);
private:
	IAPSApplication* m_pApplication;
	IPSManager * m_pPSM;
	
	UINT m_nLoginCmd;   
	UINT m_nPBOMCmd;
	UINT m_nPBomEditCmdl;
	UINT m_nMaterialCmd;
	UINT m_nProcRuleCmd;
	UINT m_nCheckinPBomCmd;
	UINT m_CreatProcCmd;
	UINT m_ProcQueryCmd;
	UINT m_ProcCheckInCmd;
	UINT m_ProcPdfEditCmd;
	UINT m_ImportEquiesCmd;
	UINT m_UpdatePartCmd;

	UINT m_CollectCmd;
	UINT m_ChangeCmd;

	CAPSModel *m_pModel;

	CFTPInterface m_FTPInterface;
	CWebServiceInterface m_WebserverInterface;
	

	//pdf输出
	CProcess * m_pProcess;
	int LoadEditableConfig();
	//BOOL OpenPdfEditPage(CString sPathFile, CAPSModel* pModel, int iType);

	CArray<CAPSGridPropData*> *m_PdfConfigDatas;
	CString GetModuleDir();
	//stProcTempConfig m_ProcTempConfig;  //工序类模板控件中的配置
	CString m_ConfigXMLName;  //输出方案所选择的xml
	std::vector<std::map<CString, UINT>> *m_ConfigProcPDFName;  //工序类的模板名称
	BOOL GetArrayFromString(LPCTSTR lpszString, LPCTSTR lpszSeparator, CStringArray &saResult);

	//主属性的信息
	std::map<CString, CString> m_mapArris;
	int InitExchange();
	int m_eSDKStatus;
	int m_bSDKLoaded;
	int m_bPdfLoaded;
	std::map<int, CString> m_mapConfig;
	BOOL m_bLoadedPRCFlag;
	int        m_iPageNums;//总页面
	CString    m_sOutPath;   //输出PDF全路径
	CString    m_sCfgPath;   //配置文件全路径

	bool ReadCfg(CString sCfgPath, int &nType);
	BOOL Convert(BOOL bConvert, bool bExportAction);
	void GetXMLConfig(std::map<int, CString> &mapConfig);
	void ClearXMLConfig();
	//	BOOL OutPut();
	void SetParam(CAPSModel* pModel, CProcess* pProcess, CString sCfgPath, CString sFilePath);
};

OBJECT_ENTRY_AUTO(__uuidof(WindChillObject), CwindchillObject)
