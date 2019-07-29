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
	BOOL CreatePbom(); //ͨ���ļ�����

	BOOL CreatePbom(CString content); //ͨ���ַ�������
protected:

	bool TravePbomrPart(std::vector<PartInfo> partInfo, PartInfo parttop, TiXmlElement *Node);

	void TraverPbom(TiXmlDocument &doc, std::vector<PartInfo> partInfo, CString str);

	void AddPbomToolNode(TiXmlElement *Node, PartInfo &partinfo, pair<string, string> Map, CString parent );

private:
	CString PbomPath;
	CString partNumber;
};