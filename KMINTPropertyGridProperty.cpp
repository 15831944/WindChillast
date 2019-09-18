// KMINTPropertyGridProperty.cpp : 实现文件
//

#include "stdafx.h"
#include "KMINTPropertyGridProperty.h"


// CKMINTPropertyGridProperty
IMPLEMENT_DYNAMIC(CKMINTPropertyGridProperty, CMFCPropertyGridProperty)

CKMINTPropertyGridProperty::~CKMINTPropertyGridProperty()
{
	
}

CKMINTPropertyGridProperty::CKMINTPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData/* = 0*/, BOOL bIsValueList/* = FALSE*/)
: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{
	
}

// Simple property
CKMINTPropertyGridProperty::CKMINTPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr/* = NULL*/, DWORD_PTR dwData/* = 0*/,
							 LPCTSTR lpszEditMask/* = NULL*/, LPCTSTR lpszEditTemplate/* = NULL*/, LPCTSTR lpszValidChars/* = NULL*/)
							 : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
	m_strValidChars = _T("0123456789");

	m_minValue = 0;
	m_maxValue = 65535;
}

void CKMINTPropertyGridProperty::SetValueRange(int min, int max)
{
	ASSERT(min < max);
	if(min < max)
	{
		m_minValue = min;
		m_maxValue = max;
	}
}

BOOL CKMINTPropertyGridProperty::TextToVar(const CString& strText)
{
	/*int nValue = StrToInt(strText);
	if(nValue < m_minValue || nValue > m_maxValue)
	{
		return FALSE;
	}*/

	return CMFCPropertyGridProperty::TextToVar(strText);
}	

BOOL CKMINTPropertyGridProperty::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CString strText;
	m_pWndInPlace->GetWindowText(strText);

	int nValue = StrToInt(strText);
	/*
	if(nValue < m_minValue || nValue > m_maxValue)
	{
		return FALSE;
	}
	*/
	if(nValue < m_minValue) nValue = m_minValue;
	if(nValue > m_maxValue) nValue = m_maxValue;

	strText.Format(_T("%d"), nValue);

	::SetWindowText(m_pWndInPlace->GetSafeHwnd(), strText);

	return CMFCPropertyGridProperty::OnUpdateValue();
}

COleVariant CKMINTPropertyGridProperty::m_StoreValue = COleVariant(_T(""));

const COleVariant& CKMINTPropertyGridProperty::GetValue() const
{ 
	/*
	COleVariant varNew(m_varValue);
	varNew.ChangeType(VT_BSTR);

	CString str = CString(varNew.bstrVal);

	int nValue = StrToInt(str);
	if(nValue < m_minValue)
	{
		nValue = m_minValue;
	}
	else if(nValue > m_maxValue)
	{	
		nValue = m_maxValue;
	}

	str.Format(_T("%d"), nValue);

	m_StoreValue.Attach(COleVariant(str));

	

	return m_StoreValue;
	*/
	return CMFCPropertyGridProperty::GetValue();
}

BOOL CKMINTPropertyGridProperty::OnEndEdit()
{
	return CMFCPropertyGridProperty::OnEndEdit();
}

void CKMINTPropertyGridProperty::SetValue(const COleVariant& varValue)
{
	//控制值在合适的范围内
	COleVariant varNew(varValue);
	varNew.ChangeType(VT_BSTR);

	CString str = CString(varNew.bstrVal);

	int nValue = StrToInt(str);
	if(nValue < m_minValue)
	{
		nValue = m_minValue;
	}
	else if(nValue > m_maxValue)
	{	
		nValue = m_maxValue;
	}

	str.Format(_T("%d"), nValue);
	//m_StoreValue.Attach(COleVariant(str));
	COleVariant StoreValue(str);

	return CMFCPropertyGridProperty::SetValue(StoreValue);
}

BOOL CKMINTPropertyGridProperty::OnEdit(LPPOINT lptClick)
{
	return CMFCPropertyGridProperty::OnEdit(lptClick);
}
// CKMINTPropertyGridProperty 成员函数
