// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#define _ATL_APARTMENT_THREADED 
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"
#include "Resource.h"
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
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

#define PULIGN_IN   //����dev_include ���ļ�
#define DEV_TMP  //�򿪿���ģʽ

#define APS_MSGBOX_TITIE "KM 3DAST"

//#define COMPANY 
#define LOCALE

//�����Ƿ�ɾ��ftp�ļ�
#define DeleteFTPFILE
#pragma warning(disable: 4996)

