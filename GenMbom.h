#pragma once
#include "stdafx.h"
#include <direct.h>
#include <afxinet.h>

#include "common.h"

#include "AsmInst.h"
#include "APSProp.h"
#include "Solid.h"
#include "Actionbase.h"
#include "AsmStep.h"
#include "CProduct.h"
#include "apsprocess.h"
#include "Equips.h"
#include "APSModel.h"

#include "tinyxml.h"
#include "WindChillObject.h"
#include "WindChillSetting.h"
#include "KmZipUtil.h"
#include "UtilityFunction.h"
#include "WindChillXml.h"
#include "DlgSavePdf.h"
#include "DlgProcEdit.h"


#include "APSWaitCursor.h"

#include "EquipActionIn.h"
#include "Equips.h"
#include"APSProp.h"




class CwindchillObject
{
public:
	CwindchillObject(CString path)
	{
		strNewSubPath=path;
	}

	void CwindchillObjectXml(CString strNewSubPath);
	std::map<CString, CString> GetStepinfo(CAsmStep * step);
	std::map<CString, CString> GetUserStepinfo(CAsmStep * step);

	std::vector<std::map<CString, CString>> GetIdinfo(CAsmStep * step);
	std::vector<std::map<CString, CString>>  GetSolidinfo(CAsmStep * step);

	std::map<CString, std::vector<std::map<CString, CString>>> GetGridData(CArray<CAPSGridPropData*>* actionGridDatas);

	std::map<CAsmStep *,std::map<CString, CString>> GetChildStepInfo(CAsmStep * step);
	std::vector<std::map<CString, CString>> GetChildStepEquies(CAsmStep * pStep);

	std::vector<CSolid *> GetStepSolid(CAsmStep * step);

private:
	 CString strNewSubPath;
	 CAPSModel *m_pModel;
};