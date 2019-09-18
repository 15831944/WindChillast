#pragma once
#include "KmPublicDef.h"
#include <afxmaskededit.h>
#include "CommonCtrlExport.h"

// CKMINTPropertyGridProperty ÃüÁîÄ¿±ê

class KM_EXT_COMMONCTRL CKMINTPropertyGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CKMINTPropertyGridProperty)

	friend class CMFCPropertyGridCtrl;
public:
	//CKMINTPropertyGridProperty();

	CKMINTPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE);

	// Simple property
	CKMINTPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);

	virtual ~CKMINTPropertyGridProperty();

	virtual BOOL OnEndEdit();
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL TextToVar(const CString& strText);
	virtual BOOL OnUpdateValue();
	virtual const COleVariant& GetValue() const;

	virtual void SetValue(const COleVariant& varValue);
public:
	void SetValueRange(int min, int max);
protected:
	int m_minValue;
	int m_maxValue;

public:
	static COleVariant m_StoreValue;
};


