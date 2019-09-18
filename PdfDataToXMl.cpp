#include "StdAfx.h"
#include "PdfDataToXMl.h"
#include "UtilityFunction.h"



PdfDataToXMl::PdfDataToXMl(std::map<CString, std::vector<PageData *> > temp)
{
	m_PageData = temp;
}


PdfDataToXMl::~PdfDataToXMl()
{

}





void PdfDataToXMl::SaveInfoData(TiXmlElement* eleItem, stCell item)
{
	eleItem->SetAttribute("ProcessID", item.m_nProcessID);
	eleItem->SetAttribute("Range", item.m_strRange.GetBuffer());
	eleItem->SetAttribute("Val", item.m_strVal.GetBuffer());
	eleItem->SetAttribute("Format", item.m_strFormat.GetBuffer());
	eleItem->SetAttribute("Angle", item.m_sAngle.GetBuffer());
	eleItem->SetAttribute("OverTurn", item.m_sOverTurn.GetBuffer());
	eleItem->SetAttribute("Width", item.m_sWidth.GetBuffer());
	eleItem->SetAttribute("OnlyShowModel", item.m_sOnlyShowModel.GetBuffer());
	eleItem->SetAttribute("Name", item.m_strName.GetBuffer());
	eleItem->SetAttribute("Alignment", item.m_sAlignment.GetBuffer());
	eleItem->SetAttribute("MergeType", item.m_strMergeType.GetBuffer());
	eleItem->SetAttribute("FontName", item.m_sFontName.GetBuffer());
	eleItem->SetAttribute("FontSize", item.m_sFontSize.GetBuffer());
}




void PdfDataToXMl::savePageInfo(TiXmlElement *ele, GridData *Griddata)
{
	for (auto i = 0; i < Griddata->m_arRowData.GetSize(); ++i)
	{
		auto item = Griddata->m_arRowData.GetAt(i);


		auto rowItem = new TiXmlElement("RowItem");
		for (auto j = 0; j < item->m_arData.GetSize(); ++j)
		{
			auto StCell = new TiXmlElement("StCell");
			auto Infodata = item->m_arData.GetAt(j);
			SaveInfoData(StCell, Infodata);
			rowItem->LinkEndChild(StCell);
		}
		ele->LinkEndChild(rowItem);
	}

	Griddata->m_arRanFt;
	Griddata->m_iInv;
}

void PdfDataToXMl::saveGrid(TiXmlElement *ele, GridData *Griddata)
{
	for (auto i = 0; i < Griddata->m_arRowData.GetSize(); ++i)
	{
		auto item = Griddata->m_arRowData.GetAt(i);
	
		auto rowItem = new TiXmlElement("RowItem");
		for (auto j = 0; j < item->m_arData.GetSize(); ++j)
		{
			auto StCell = new TiXmlElement("StCell");
			auto Infodata = item->m_arData.GetAt(j);
			SaveInfoData(StCell, Infodata);
			rowItem->LinkEndChild(StCell);
		}
		ele->LinkEndChild(rowItem);
	}

	for (auto j = 0; j < Griddata->m_arChildGrid.GetSize(); ++j)
	{
		auto item = Griddata->m_arChildGrid.GetAt(j);
		auto ChildGrid = new TiXmlElement("ChildGrid");
		saveGrid(ChildGrid, item);
		ele->LinkEndChild(ChildGrid);
	}


	for (auto j = 0; j < Griddata->m_arRanFt.GetSize(); ++j)
	{
		auto item = Griddata->m_arRanFt.GetAt(j);

		auto ranformat = new TiXmlElement("RanFormat");
		ranformat->SetAttribute("St", item.strSt.GetBuffer());
		ranformat->SetAttribute("Ed", item.strEd.GetBuffer());
		ranformat->SetAttribute("Range", item.strRange.GetBuffer());

		ele->LinkEndChild(ranformat);
	}
}

void PdfDataToXMl::CreateXml(CString path)
{

	TiXmlDocument doc;

	for (auto it = m_PageData.begin(); it != m_PageData.end(); ++it)
	{
		auto vpagedata = it->second;

		auto Page = new TiXmlElement("Page");
		auto strPage = it->first;
		Page->SetAttribute("PageName", strPage);


		for (auto itempage = vpagedata.begin(); itempage != vpagedata.end(); ++itempage)
		{
			auto PageNum = new TiXmlElement("PageNum");
			auto pageinfo = (*itempage)->m_pInfo;
			auto Info = new TiXmlElement("Info");
		
			auto No = (*itempage)->m_No;
			auto StepId = (*itempage)->m_iStepID;
			auto strDefName = (*itempage)->m_strDefName;



			auto strSheetNameNo = (*itempage)->m_strSheetName;
			auto strSheetName = (*itempage)->m_strDefName;

			Info->SetAttribute("NO", No);
			Info->SetAttribute("StepId", StepId);
			Info->SetAttribute("DefName", strDefName.GetBuffer());
			Info->SetAttribute("SheetNameNo", strSheetNameNo.GetBuffer());
			Info->SetAttribute("strSheetName", strSheetName.GetBuffer());
			
			savePageInfo(Info, pageinfo);
			PageNum->LinkEndChild(Info);

			for (auto i = 0; i < (*itempage)->m_arGrid.GetSize(); ++i)
			{
				auto Grid = new TiXmlElement("Grid");
				auto data = (*itempage)->m_arGrid.GetAt(i);
				saveGrid(Grid, data);

				PageNum->LinkEndChild(Grid);
			}
			Page->LinkEndChild(PageNum);
		}

		doc.LinkEndChild(Page);
	}

	doc.SaveFile(path.GetBuffer());


}

XmlToPdfdata::XmlToPdfdata()
{

}

XmlToPdfdata::~XmlToPdfdata()
{

}

void XmlToPdfdata::GetInfoData(TiXmlElement* eleItem, stCell *item)
{
	item->m_nProcessID = atoi( eleItem->Attribute("ProcessID"));
	item->m_strRange=eleItem->Attribute("Range");
	item->m_strVal=eleItem->Attribute("Val");
	item->m_strFormat=eleItem->Attribute("Format");
	item->m_sAngle=eleItem->Attribute("Angle");
	item->m_sOverTurn=eleItem->Attribute("OverTurn");
	item->m_sWidth=eleItem->Attribute("Width");
	item->m_sOnlyShowModel=eleItem->Attribute("OnlyShowModel");
	item->m_strName=eleItem->Attribute("Name");
	item->m_sAlignment=eleItem->Attribute("Alignment");
	item->m_strMergeType=eleItem->Attribute("MergeType");
	item->m_sFontName=eleItem->Attribute("FontName");
	item->m_sFontSize=eleItem->Attribute("FontSize");
}

void XmlToPdfdata::GetPageData(TiXmlElement *ele, std::vector<PageData *> &Pagedata)
{
	for (auto it = ele; it; it = it->NextSiblingElement())
	{
		auto value = it->Value();

		if (CString(value).CompareNoCase("PageNum")==0)
		{
			PageData *data = new PageData;

			auto FirstELe = it->FirstChildElement();

			auto No = FirstELe->Attribute("NO");
			auto  StepId = FirstELe->Attribute("StepId");
			auto DefName = FirstELe->Attribute("DefName");
			auto SheetNameNo = FirstELe->Attribute("SheetNameNo");
			auto strSheetName = FirstELe->Attribute("strSheetName");

			data->m_No = atoi(No);
			data->m_iStepID = atoi(StepId);
			data->m_strDefName = DefName;
			data->m_strPageRotate = SheetNameNo;
			data->m_strSheetName = strSheetName;

			data->m_pInfo = new GridData;

			GetPage(FirstELe, data->m_pInfo,&data->m_arGrid);

			Pagedata.push_back(data);
		}
	}
}





void XmlToPdfdata::GetPage(TiXmlElement *ele, GridData *Griddata, CArray<GridData *, GridData *> *arGrid)
{
	for (auto it = ele; it; it = it->NextSiblingElement())
	{
		auto value = it->Value();

		if (CString(value).CompareNoCase("Info") == 0)
		{
			for (auto rowdata = it->FirstChildElement(); rowdata; rowdata = rowdata->NextSiblingElement())
			{
				auto row = new RowData;

				for (auto Cell = rowdata->FirstChildElement(); Cell; Cell = Cell->NextSiblingElement())
				{
					auto cell = new stCell;
					GetInfoData(Cell, cell);
					row->m_arData.Add(*cell);
				}
				Griddata->m_arRowData.Add(row);
			}
		}

		if (CString(value).CompareNoCase("Grid") == 0)
		{
			auto Grid = new GridData;

			GetGrid(it->FirstChildElement(), Grid);

			arGrid->Add(Grid);

		}
	}
}

void XmlToPdfdata::GetGrid(TiXmlElement *ele, GridData *Griddata)
{
	
	for (auto Item = ele; Item; Item = Item->NextSiblingElement())
	{
		auto tempstr = Item->Value();

		if (CString(tempstr).CompareNoCase("RowItem") == 0)
		{
			auto row = new RowData;
			for (auto Cell = Item->FirstChildElement(); Cell; Cell = Cell->NextSiblingElement())
			{
				auto cell = new stCell;
				GetInfoData(Cell, cell);
				row->m_arData.Add(*cell);
			}

			Griddata->m_arRowData.Add(row);

		}

		if (CString(tempstr).CompareNoCase("ChildGrid") == 0)
		{
			auto childGrid = new GridData;
			GetGrid(Item->FirstChildElement(), childGrid);
			Griddata->m_arChildGrid.Add(childGrid);
		}

		if (CString(tempstr).CompareNoCase("RanFormat") == 0)
		{
			auto arRant = new RanFormat;
			arRant->strEd = Item->Attribute("Ed");
			arRant->strRange= Item->Attribute("Range");
			arRant->strSt = Item->Attribute("St");
			Griddata->m_arRanFt.Add(*arRant);
		}
	}
	
}

void XmlToPdfdata::GetData(CString path)
{
	TiXmlDocument doc;

	auto falg = doc.LoadFile(path.GetBuffer());
	if (!doc.LoadFile(path.GetBuffer()))
	{
		path.ReleaseBuffer();
		return;
	}
	path.ReleaseBuffer();

	auto PageEle = doc.RootElement();

	for (auto it = PageEle; it != NULL; it = it->NextSiblingElement())
	{
		auto str = it->Attribute("PageName");
		auto Item = it->FirstChildElement();
		std::vector<PageData *> Pagedata;

		GetPageData(Item, Pagedata);

		m_PageData[str] = Pagedata;
	}

}

ChangeXmlToPageData::ChangeXmlToPageData(CString chagePath)
{
	m_path = chagePath;
	GetXmlInfo();
}

ChangeXmlToPageData::~ChangeXmlToPageData()
{

}

void ChangeXmlToPageData::GetXmlInfo()
{
	TiXmlDocument doc;

	if (!doc.LoadFile(m_path))
	{
		return;
	}

	auto rootEle = doc.FirstChildElement();

	for (auto InfoEle = rootEle; InfoEle; InfoEle = InfoEle->NextSiblingElement())
	{
		CString changeName = InfoEle->Attribute("PageFirst");
		auto chageinfo = new ChangeInfo;
		auto Info = InfoEle->FirstChildElement();
		for (auto Item = Info->FirstChildElement(); Item; Item = Item->NextSiblingElement())
		{
			CString temp = Item->Attribute("Range");
			if (temp.CompareNoCase("plant") == 0)
			{
				chageinfo->plant = Item->GetText();
			}
			if (temp.CompareNoCase("genggaidan") == 0)
			{
				chageinfo->genggaidan = Item->GetText();
			}
			if (temp.CompareNoCase("pronum") == 0)
			{
				chageinfo->pronum = Item->GetText();
			}
			if (temp.CompareNoCase("lzjname") == 0)
			{
				chageinfo->lzjname = Item->GetText();
			}

			if (temp.CompareNoCase("lzjnum") == 0)
			{
				chageinfo->lzjnum = Item->GetText();
			}

			if (temp.CompareNoCase("ffdw") == 0)
			{
				chageinfo->ffdw = Item->GetText();
			}

			if (temp.CompareNoCase("grouptotal") == 0)
			{
				chageinfo->grouptotal = Item->GetText();
			}

			if (temp.CompareNoCase("groupcurrent") == 0)
			{
				chageinfo->groupcurrent = Item->GetText();
			}

			if (temp.Find(',') == 0)
			{
				chageinfo->dwg = Item->GetText();
			}
		}

		m_XmlData[changeName] = chageinfo;
	}
}

void ChangeXmlToPageData::GenPageData()
{
	GetCongfig();

	auto config =m_putpdf.GetPdfConfig();

	ConfigPage *page = NULL;
	CString strValue("厂艺表065工艺规程更改单.pdf");
	for (auto j = 0; j < config->m_arConfigGroup.GetSize(); ++j)
	{
		ConfigGroup* GroupInfo = config->m_arConfigGroup.GetAt(j);
		auto PageInfo = GroupInfo->m_pFirstPage;
		if (strValue.CompareNoCase(PageInfo->m_strName) == 0)
		{
			page= PageInfo;
			break;
		}
	}

	for (auto it = m_XmlData.begin(); it!=m_XmlData.end(); ++it)
	{
		auto Pagedata =new PageData;
		Pagedata->m_No = 1;
		Pagedata->m_strDefName = "厂艺表065工艺规程更改单.PDF";
		Pagedata->m_strSheetName = "厂艺表065工艺规程更改单.PDF";

	
		Pagedata->m_pInfo = new GridData;

		auto ChangeGridData = Pagedata->m_pInfo;
		//初始化GridData
		auto ranFor = new RanFormat;
		ranFor->strEd = "26";
		ranFor->strRange = "26";
		ranFor->strSt = "26";
		ChangeGridData->m_arRanFt.Add(*ranFor);

		delete ranFor;
		ranFor = NULL;

		auto rowdata = new RowData;

		for (auto i = 0; i <page->m_InfoHeard.GetSize(); ++i)
		{
			auto item = page->m_InfoHeard.GetAt(i);
			auto data = new stCell;

			data->m_strRange = item.m_strRange;
			data->m_sFontName = item.m_sFontName;
			data->m_strName = item.m_strName;
			data->m_strFormat = item.m_strFormat;

			auto value =it->second;
			
			CString temp = item.m_strRange;
			if (temp.CompareNoCase("plant") == 0)
			{
				data->m_strVal = value->plant;
			}
			if (temp.CompareNoCase("genggaidan") == 0)
			{
				data->m_strVal = value->genggaidan;
			}
			if (temp.CompareNoCase("pronum") == 0)
			{
				data->m_strVal = value->pronum;
			}
			if (temp.CompareNoCase("lzjname") == 0)
			{
				data->m_strVal = value->lzjname;
			}

			if (temp.CompareNoCase("lzjnum") == 0)
			{
				data->m_strVal = value->lzjnum;
			}

			if (temp.CompareNoCase("ffdw") == 0)
			{
				data->m_strVal = value->ffdw;
			}

			if (temp.CompareNoCase("grouptotal") == 0)
			{
				data->m_strVal = value->grouptotal;
			}

			if (temp.CompareNoCase("groupcurrent") == 0)
			{
				data->m_strVal = value->groupcurrent;
			}

			if (temp.Find(',') != -1)
			{
				data->m_strVal = value->dwg;
			}

			rowdata->m_arData.Add(*data);

			delete data;
			data = NULL;

		}
		ChangeGridData->m_arRowData.Add(rowdata);

		m_Pagedata.Add(Pagedata);
	}
		
}

void ChangeXmlToPageData::GetCongfig()
{
	//先清空配置
	m_putpdf.ClearXMLConfig();

	CString sCfgPath = GetModuleDir() + _T("\\Resources\\Chs\\PDFTemplate\\工艺规程模板.xml");
	m_putpdf.SetParam(NULL, NULL, sCfgPath, "");
	int nSign = 0;
	//解析xml格式
	BOOL bReadXml = m_putpdf.ReadCfg(nSign);
}
