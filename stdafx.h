// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#define _ATL_APARTMENT_THREADED 
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"
#include "Resource.h"
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#include <afxdisp.h>
#include <afxdisp.h>

#include <iostream>
#ifdef WIN32
#include <tchar.h>
#endif

#include <vector>
#include <afxcontrolbars.h>

//#include <BCGCBProInc.h>

//#pragma comment(lib,"Output3DPdf1.lib")

#import "..\include\kmapstool.tlb" no_namespace, named_guids
#import "..\include\PSM.tlb" no_namespace, named_guids
#import "..\include\Common.tlb" no_namespace, named_guids

#import "msxml6.dll"
#import "..\include\msado15.dll" no_namespace rename("EOF", "adoEOF")\
	rename("LockTypeEnum","adoLockTypeEnum")\
	rename("DataTypeEnum","adoDataTypeEnum")\
	rename("FieldAttributeEnum","adoFieldAttributeEnum")\
	rename("EditModeEnum","adoEditModeEnum")\
	rename("RecordStatusEnum","adoRecordStatusEnum")\
	rename("ParameterDirectionEnum","adoParameterDirectionEnum")

#import "..\include\Common.tlb" no_namespace, named_guids



#define VOID_PTR		byte*

#define PULIGN_IN   //控制dev_include 中文件
#define DEV_TMP  //打开开发模式

#define APS_MSGBOX_TITIE "KM 3DAST"

//#define COMPANY 
#define LOCALE

//编译是否删除ftp文件
#define DeleteFTPFILE
#pragma warning(disable: 4996)

