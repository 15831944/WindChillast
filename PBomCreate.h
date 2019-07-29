#pragma once


#include "afx.h"
#include "tinyxml.h"
#include "Partinfo.h"
#include <map>
#include <string>
#include <vector>
#include "comutil.h"

using namespace std;

class CPbomCreate
{
public:
	CPbomCreate(CString &PbomPath, CString partNumber);

	CPbomCreate(CString);
	BOOL CreatePbom(); //通过文件创建

	BOOL CreatePbom(CString content); //通过字符串创建
protected:

	bool TravePbomrPart(std::vector<PartInfo> partInfo, PartInfo parttop, TiXmlElement *Node);

	void TraverPbom(TiXmlDocument &doc, std::vector<PartInfo> partInfo, CString str);

	void AddPbomToolNode(TiXmlElement *Node, PartInfo &partinfo, pair<string, string> Map, CString parent );

private:
	CString PbomPath;
	CString partNumber;
};