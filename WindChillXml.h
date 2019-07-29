#pragma once
#include <map>
#include <vector>
#include "Common.h"
#include "Partinfo.h"


struct ASTXMLInfo
{
	int id;
	CString parentNumber;
	CString Number;
	std::map<CString, CString> Nodeinfo;
	std::vector<CString> Postion;	

	bool flag;
	ASTXMLInfo()
	{
		this->id =0;
		this->parentNumber ="";
		this->Number ="";
		this->flag =false;
	}
};


class CWindChillXml
{
public:
	static bool ParseResult(CString strOutXml); //判断返回是否成功
	static CString GetPath(CString strOutXml); //解析返回路径

	static void ParesrChangexml(CString strXml);
};

class CWindChillPbom
{

public:

	//CString judgeTop(std::vector<PartInfo>partInfo );
	//PartInfo lookforpartbyNumber(std::vector<PartInfo> partInfo,string cstr); 
	//void AddToolNode(TiXmlElement *Node, PartInfo &partinfo,CString parent="");
	//void TraverPart(TiXmlDocument &doc, std::vector<PartInfo> partInfo, CString str);

};

class CAstXml
{
public:
	static bool UpdateAttribute(CString strAstXml,std::vector<PartInfo> partinfo, CString strpartFirstName);	//更新3DAST的xml属性
	static void TraverseNodeParams(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<PartInfo> partinfo, BOOL bIsRoot = FALSE, CString strpartFirstName = "");
	static void AppendParam(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<PartInfo> partinfo, CString &strNameVal, BOOL bIsRoot, CString strpartFirstName);
	static void UpdateParam(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<PartInfo> partinfo, CMap<CString, LPCTSTR, MSXML2::IXMLDOMNodePtr, MSXML2::IXMLDOMNodePtr> &nodemap, CString strNameVal);

	static void AddToolNode(MSXML2::IXMLDOMDocumentPtr pXmlDoc, std::vector<PartInfo> &partinfo);

	static CMap<CString, LPCTSTR, MSXML2::IXMLDOMNodePtr, MSXML2::IXMLDOMNodePtr> m_BomNodemap;
	static int m_Maxinstanceid;

	static void TraverseAstXml(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<ASTXMLInfo> &ASTInfo,CString ParentNumber,BOOL bIsRoot = FALSE );
	
	static void TraverseAstXmlNodeParam(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<CString> &AddorUpdateFile, std::vector<MSXML2::IXMLDOMNodeListPtr> & Node,BOOL bIsRoot = FALSE);
};

extern bool traver(std::vector<PartInfo> partInfo, PartInfo parttop,TiXmlElement *);
extern void TraverPart(std::vector<PartInfo> partInfo);
extern PartInfo lookforpartbyNumber(std::vector<PartInfo> partInfo, std::string cstr);


