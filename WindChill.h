// WindChill.h : WindChill DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "WindChill_i.h"


// CWindChillApp
// �йش���ʵ�ֵ���Ϣ������� WindChill.cpp
//

class CWindChillApp : public CWinApp
{
public:
	CWindChillApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
