#pragma once
#include "tinyxml.h"
#include <map>
#include <vector>
#include "Output3DPdf.h"
#include "DlgChange.h"
#include "PDFData.h"




class PdfDataToXMl
{

public:
	PdfDataToXMl(std::map<CString, std::vector<PageData *> > temp);
	~PdfDataToXMl();
	void SaveInfoData(TiXmlElement* eleItem, stCell item);
	void savePageInfo(TiXmlElement *ele, GridData *Griddata);
	void saveGrid(TiXmlElement *ele, GridData *Griddata);

	void CreateXml(CString path);
private:
	std::map<CString, std::vector<PageData *> > m_PageData;
};



class XmlToPdfdata
{
public:

	XmlToPdfdata();
	~XmlToPdfdata();
	void GetData(CString path);
	std::map<CString, std::vector<PageData *> > GetPageData()
	{
		return m_PageData;
	}
	
private:
	
	void GetInfoData(TiXmlElement* eleItem, stCell* item);
	void GetPageData(TiXmlElement *ele, std::vector<PageData *> &Pagedata);
	void GetPage(TiXmlElement *ele, GridData *Griddata, CArray<GridData *, GridData *> *arGrid);

	void GetGrid(TiXmlElement *ele, GridData *Griddata);
	
private:
	std::map<CString, std::vector<PageData *> > m_PageData;
};



class ChangeXmlToPageData
{

public:
	ChangeXmlToPageData(CString chagePath);
	~ChangeXmlToPageData();
	

	void GetXmlInfo();
	void GenPageData();
	void GetCongfig();

	CArray<PageData *, PageData*>* GetChangInfo()
	{
		return &m_Pagedata;
	}
private:
	CArray<PageData *, PageData*> m_Pagedata;
	std::map<CString, ChangeInfo*> m_XmlData;
	CString m_path;
	COutPut3DPdf m_putpdf;
};