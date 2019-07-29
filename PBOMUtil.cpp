#include "StdAfx.h"
#include "PBOMUtil.h"
#include "tinyxml.h"
#include "WindChillSetting.h"
#include <string>
#include "xml.h"
#include "UtilityFunction.h"
CPBOMUtil::CPBOMUtil(void)
{
	m_strRootNumber = _T("");
	m_strAst = _T("");
	m_strPbom = _T("");
	m_nAsmId = 300000002;
	m_nPrtId = 200000002;
}


CPBOMUtil::~CPBOMUtil(void)
{
}

BOOL FindRootNode(IN	MSXML2::IXMLDOMElementPtr xml)
{
	// 首先判断节点本身是否Root节点
	CString strNodeName = (char*)xml->nodeName;
	if (strNodeName == "Root")
	{
		return TRUE;
	}

	// 得到到子节点链表指针
	MSXML2::IXMLDOMNodeListPtr pChildList = xml->GetchildNodes();
	long nCount = pChildList->length;

	// 深度优先搜索子节点, 判断是否存在Root节点
	if (nCount > 0)
	{
		for (int i = 0; i < nCount; i++)
		{
			MSXML2::IXMLDOMElementPtr pChild = pChildList->item[i];
			strNodeName = (char*)pChild->nodeName;
			if (strNodeName == "Root")
			{
				return TRUE;
			}
			else
			{
				MSXML2::IXMLDOMNodeListPtr pGrandList = pChild->GetchildNodes();
				long nCount1 = pGrandList->length;
				if (nCount1 > 0)
					return FindRootNode(pChild);
				else
					continue;
			}
		}
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}


bool CPBOMUtil::CreatePbomExchange(CString strAstXml,CString strWindchillXml,CString topNumber)
{
	CWindChillSetting::PROCUDEID.clear();

	vector<CAstNodeInfo> nodeInfos;
	nodeInfos.clear();
	if (!GetAstInfo(strAstXml,nodeInfos)) return false;
	
	map<CString,CWindNode*> windNodes;
	windNodes.clear();
	vector<CWindRelation> windRelationVec;
	windRelationVec.clear();
	
	if (!GetWindchillInfo(strWindchillXml,windNodes,windRelationVec, topNumber)) return false;
	
	GetPosition(windRelationVec,windNodes,nodeInfos);
	
	CreatePbomXml(windRelationVec,windNodes,strAstXml);

	return true;
}


bool CPBOMUtil::CreatePbomIop(CString strAstXml, CString strWindchillXml, CString topNumber)
{
	CWindChillSetting::PROCUDEID.clear();

	vector<CAstNodeInfo> nodeInfos;
	nodeInfos.clear();
	if (!GetAstInfoIop(strAstXml, nodeInfos)) return false;

	map<CString, CWindNode*> windNodes;
	windNodes.clear();
	vector<CWindRelation> windRelationVec;
	windRelationVec.clear();

	if (!GetWindchillInfo(strWindchillXml, windNodes, windRelationVec, topNumber)) return false;

	GetPosition(windRelationVec, windNodes, nodeInfos);

	CreatePbomXml(windRelationVec, windNodes, strAstXml);
	return true;
}

MSXML2::IXMLDOMNodePtr FindNodePtrByNodeName(
	IN	MSXML2::IXMLDOMNodeListPtr	pRootChilds,
	IN	const CString&				strNodeName
)
{
	if (NULL == pRootChilds)
		return NULL;

	long nCount = pRootChilds->length;
	for (int i = 0; i < nCount; i++)
	{
		MSXML2::IXMLDOMNodePtr pDocNode = pRootChilds->item[i];
		if (MSXML2::NODE_ELEMENT == pDocNode->nodeType)
		{
			MSXML2::IXMLDOMElementPtr xmlnode = pDocNode;
			if (CString((char*)pDocNode->nodeName) == strNodeName ||
				FindRootNode(xmlnode))
			{
				return pDocNode;
			}
		}
	}
	return NULL;
}



//获取iop的位置信息
BOOL ReadPosition(MSXML2::IXMLDOMElementPtr	pPosNode,double	arPosion[12])
{
	CString str;

	CXML::ReadStrFromXML(pPosNode, _T("V1"), str);
	arPosion[0] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V2"), str);
	arPosion[1] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V3"), str);
	arPosion[2] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V4"), str);
	arPosion[3] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V5"), str);
	arPosion[4] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V6"), str);
	arPosion[5] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V7"), str);
	arPosion[6] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V8"), str);
	arPosion[7] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("V9"), str);
	arPosion[8] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("T1"), str);
	arPosion[9] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("T2"), str);
	arPosion[10] = _tstof(str);

	CXML::ReadStrFromXML(pPosNode, _T("T3"), str);
	arPosion[11] = _tstof(str);

	return TRUE;
}

BOOL ReadAssembleInfo(MSXML2::IXMLDOMElementPtr	pAssembleNode,bool bTopLevel,bool bBaseAsm,vector<CAstNodeInfo>& nodeinfos,CString parent,std::map<CString,CString> m_mapDocLinkID)
{
	_variant_t						tName;
	_variant_t						tIdentifier;
	UINT							nID;

	if (NULL == pAssembleNode)
		return FALSE;

	//InterOPR15输出的顶层装配下没有Name、Identifier属性
	// 读入装配节点的 Name 属性
	
	tIdentifier = pAssembleNode->getAttribute((_bstr_t)"Identifier");
	// 遍历该装配节点的子节点
	MSXML2::IXMLDOMNodeListPtr pNodeList = pAssembleNode->childNodes;

	int nChildCount = pNodeList->length;
	for (int i = 0; i < nChildCount; i++)
	{
		// 在XML中的一个实体节点指针
		MSXML2::IXMLDOMElementPtr pSolid = (MSXML2::IXMLDOMElementPtr)pNodeList->item[i];
		CString strChildName = (char*)pSolid->nodeName;
		CAstNodeInfo temp;
		if (strChildName == "Position")
		{
			// 是位置节点， 保存位置矩阵
			double				arPosion[12];
			ReadPosition(pSolid, arPosion);

			for (auto it=begin(arPosion);it!=end(arPosion);++it)
			{
				char pos[256];
				sprintf_s(pos, "%.10f", *it);
				temp.AddPosition(pos);
			}

			MSXML2::IXMLDOMElementPtr pRefLink;
			pRefLink = CXML::FindChildXMLElement(pAssembleNode, _T("RefLink"));
			_variant_t tAttr = pRefLink->getAttribute(_T("Id"));
			CString strPartRefID((char*)(_bstr_t)tAttr);

			auto space =strPartRefID.FindOneOf(" ");
			if (space!=-1)
			{
				strPartRefID=strPartRefID.Left(space);
			}
			temp.SetNumber(strPartRefID);
			space =parent.FindOneOf(" ");

			if (space!=-1)
			{
				parent=parent.Left(space);
			}

			temp.SetNumber(strPartRefID);
			temp.SetParentNumber(parent);


			auto result=m_mapDocLinkID.find(strPartRefID);
			if (result!= m_mapDocLinkID.end())
			{
				temp.AddProp("srcname",result->second );

				auto filename =GetMainFileName(result->second)+".hsf";
				temp.AddProp("filename",filename );
			}

			

			nodeinfos.push_back(temp);
			
		} // if(strChildName == "Position")
		else if (strChildName == "Child")
		{
			// 如果，子节点是一个子装配，递归调用本成员函数进一步读取子装配的信息

			if (bTopLevel==true)
			{
				ReadAssembleInfo(pSolid, false, false, nodeinfos, parent,m_mapDocLinkID);
			}
			else
			{
				MSXML2::IXMLDOMElementPtr pRefLink;
				pRefLink = CXML::FindChildXMLElement(pAssembleNode, _T("RefLink"));
				_variant_t tAttr = pRefLink->getAttribute(_T("Id"));
				CString strPartRefID((char*)(_bstr_t)tAttr);
				ReadAssembleInfo(pSolid, false, false, nodeinfos, strPartRefID, m_mapDocLinkID);
			}
		} // else if(strChildName == "Child")
	}


	return TRUE;
}

void InitSatIDMap(MSXML2::IXMLDOMElementPtr m_pEBom, vector<CAstNodeInfo>& nodeinfos, std::map<CString,CString> &m_mapDocLinkID)
{
	
	MSXML2::IXMLDOMElementPtr pDocNode = CXML::FindChildXMLElement(m_pEBom, _T("Document"));
	if (pDocNode == NULL)
	{
		return;
	}

	MSXML2::IXMLDOMElementPtr pExpos = CXML::FindChildXMLElement(pDocNode, _T("Exposition"));
	if (pExpos == NULL)
	{
		return;
	}

	MSXML2::IXMLDOMElementPtr pStructure = CXML::FindChildXMLElement(pExpos, _T("Structure"));
	if (pStructure == NULL)
	{
		return;
	}

	MSXML2::IXMLDOMElementPtr pReference = CXML::FindChildXMLElement(pStructure, _T("Reference"));
	if (pReference == NULL)
	{
		return;
	}

	//	CMap<CString,LPCTSTR,CString,LPCTSTR> mapSolidId2RepId;
	//	CMap<CString,LPCTSTR,CString,LPCTSTR> mapDocId2FilePath;

	//读出所有的Reference节点映射
	//读出所有的Document映射
	while (pReference)
	{
		CAstNodeInfo temp;
		_variant_t varName = pReference->getAttribute((_bstr_t)"Identifier");
		CString strName(varName.bstrVal);

		MSXML2::IXMLDOMElementPtr pRep = CXML::FindChildXMLElement(pReference, _T("Rep"));
		if (pRep != NULL)
		{
			_variant_t tName = pRep->getAttribute((_bstr_t)"Identifier");
			CString str = (char*)(_bstr_t)tName;

			//m_mapSatID.SetAt((LPCTSTR)strName, str);
		}

		pReference = pReference->GetnextSibling();
	}

	while (pDocNode)
	{
		_variant_t varName = pDocNode->getAttribute((_bstr_t)"Identifier");
		CString strName(varName.bstrVal);

		auto space =strName.FindOneOf(" ");
		if (space!=-1)
		{
			strName=strName.Left(space);
		}

		_variant_t varType = pDocNode->getAttribute((_bstr_t)"Type");
		CString strType(varType.bstrVal);

		if ((strType.CompareNoCase(_T("SAT")) == 0) || (strType.CompareNoCase(_T("SAB")) == 0))
		{
			_variant_t varFilePath = pDocNode->getAttribute((_bstr_t)"FilePath");
			//m_mapShapeID.SetAt(strName, CString(varFilePath.bstrVal));

			//begin add by dm in 20190509 读取零件的文件路径
			MSXML2::IXMLDOMElementPtr pIss = CXML::FindChildXMLElement(pDocNode, _T("IssuedFrom"));
			if (pIss != NULL)
			{
				MSXML2::IXMLDOMElementPtr pDocLink = CXML::FindChildXMLElement(pIss, _T("DocLink"));
				if (pDocLink != NULL)
				{
					CString strFilePath = pDocLink->getAttribute((_bstr_t)"Id");
					//m_mapDocLinkID.SetAt(strName, strFilePath);

					m_mapDocLinkID[strName] = strFilePath;
				}
			}
			//end mod
		}
		pDocNode = pDocNode->GetnextSibling();
	}

}

bool CPBOMUtil::GetAstInfoIop(CString strXml, vector<CAstNodeInfo>& nodeinfos)
{
	std::map<CString ,CString> m_mapDocLinkID;

	MSXML2::IXMLDOMElementPtr m_pEBom;
	MSXML2::IXMLDOMElementPtr	m_pStructure;
	MSXML2::IXMLDOMElementPtr m_pAttrRoot;	//属性文件根节点

	CString			tmpStr = strXml;
	CString			extName = tmpStr.Mid(tmpStr.ReverseFind('.'));
	CString			strXMLFile;
	BOOL			bBaseAsm = FALSE;
	bool			bRet = TRUE;
	VARIANT_BOOL	vbool = 0;
	CFileFind		finder;

	MSXML2::IXMLDOMDocumentPtr	xml(__uuidof(MSXML2::DOMDocument60));
	MSXML2::IXMLDOMDocumentPtr	xmlAttr(__uuidof(MSXML2::DOMDocument60));

	MSXML2::IXMLDOMElementPtr	pRootChild;
	MSXML2::IXMLDOMElementPtr	pRoot;
	MSXML2::IXMLDOMNodePtr		pExpos;
	MSXML2::IXMLDOMNodeListPtr	pRootChilds;
	MSXML2::IXMLDOMNodePtr		pDocNode;

	extName.MakeLower();

	if (extName == ".xml")
	{
		strXMLFile = tmpStr;
	}
	else
	{
		bRet = FALSE;
		goto ExitHere;
	}

	// 读取装配零件数据---------------------------------------------------------

	// 创建XML文档对象
	if (xml == NULL)
	{
		bRet = FALSE;
		goto ExitHere;
	}

	// 从文件中载入，XML文档数据
	vbool = xml->load((_variant_t)strXMLFile);

	if (vbool == VARIANT_FALSE)
	{
		bRet = FALSE;
		goto ExitHere;
	}

	if (finder.FindFile(strXMLFile.Mid(0, strXMLFile.ReverseFind('.')) + _T(".attr")))
	{
		vbool = xmlAttr->load(_variant_t(strXMLFile.Mid(0, strXMLFile.ReverseFind('.')) + _T(".attr")));
		
		m_pAttrRoot = CXML::ReadRootElement(xmlAttr, _T("APS_Model_Attr"));
	}
	finder.Close();

	// 读取XML文档根节点
	m_pEBom = CXML::ReadRootElement(xml, _T("E-BOM"));
	if (m_pEBom == NULL)
	{
		bRet = FALSE;
		goto ExitHere;
	}

	// 找到有Root子节点的Document节点
	pRootChilds = m_pEBom->GetchildNodes();
	pDocNode = FindNodePtrByNodeName(pRootChilds, _T("Root"));//找到Root节点位置

															  // 可以一级级的读数据了
															  // Exposition 节点
	pExpos = CXML::FindChildXMLElement(pDocNode, _T("Exposition"));
	if (pExpos == NULL)
	{
		bRet = FALSE;
		goto ExitHere;
	}

	// Structure 节点
	m_pStructure = CXML::FindChildXMLElement(pExpos, _T("Structure"));
	if (m_pStructure == NULL)
	{
		bRet = FALSE;
		goto ExitHere;
	}

	// Root 节点
	pRoot = CXML::FindChildXMLElement(m_pStructure, _T("Root"));
	if (pRoot == NULL)
	{
		bRet = FALSE;
		goto ExitHere;
	}

	pRootChild = pRoot;


	
	try
	{
		InitSatIDMap(m_pEBom,nodeinfos, m_mapDocLinkID);
		
		CString str = tmpStr.Mid(tmpStr.ReverseFind(_T('\\')) + 1);
		if (str.ReverseFind('.') > 0)
		{
			str = str.Left(str.ReverseFind(_T('.')));
		}
		//m_pAsm->SetSolidName(str);

		_variant_t tAttr = pRoot->getAttribute(_T("Identifier"));

		CString parent(tAttr);
		bRet = ReadAssembleInfo(pRootChild, TRUE, bBaseAsm,nodeinfos, parent, m_mapDocLinkID);

	}
	catch (...)
	{
		bRet = FALSE;

		ASSERT(FALSE);
		TRACE("Error When Reading Assemble Info.\n");
	}

ExitHere:
	return bRet;
}


extern bool WFromUTF8(const char* szText, std::wstring &wstrVal);

bool CPBOMUtil::GetWindchillInfo(CString strXml,map<CString,CWindNode*> &nodeinfos,vector<CWindRelation> &relationVec,CString topNumber)
{
	TiXmlDocument Ebomdoc;

	/*CFile file;
	file.Open(strXml, CFile::modeReadWrite);
	DWORD dwFilelen;
	dwFilelen = file.GetLength();
	char *buf;
	buf = new char[dwFilelen + 1];
	buf[dwFilelen] = 0;

	file.Read(buf, dwFilelen);
	std::wstring a;

	WFromUTF8(buf, a);
	auto dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum +1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);*/

	//AfxMessageBox(sabstr);
	//CString s =sabstr;
	Ebomdoc.Parse(strXml.GetBuffer());
	
	//file.Close();
	/*if (!Ebomdoc.LoadFile(strXml.GetBuffer()))
	{
		return false;
	}
	strXml.ReleaseBuffer();*/

	TiXmlElement *root = Ebomdoc.FirstChildElement();
	CString strNodeName = root->Value();
	if (root == NULL || root->FirstChildElement()==NULL)
	{
		Ebomdoc.Clear();
		return false;
	}

	auto elem = root->FirstChildElement()->FirstChildElement();
	bool bGetRoot = false;
	while (elem != NULL)
	{
		strNodeName = elem->Value();
		CWindNode *pNodeData = new CWindNode;
		GetWindchillNode(elem,*pNodeData,relationVec);
		if (!bGetRoot)
		{
			m_strRootNumber = topNumber;
			bGetRoot = true;
		}
		//nodeinfos.push_back(nodeData);
		nodeinfos[pNodeData->m_strNumber] = pNodeData;
		elem = elem->NextSiblingElement();
	}

	//delete []buf;
	//delete []sabstr;

	return true;
}

bool CPBOMUtil::GetAstInfo(CString strXml,vector<CAstNodeInfo> &nodeinfos)
{
	MSXML2::IXMLDOMDocumentPtr	pXmlDoc(__uuidof(MSXML2::DOMDocument60));
	/*pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
	if (!pXmlDoc)
	{
		ASSERT(FALSE);
		return false;
	}
*/
	if (pXmlDoc->load((LPCTSTR)(strXml)) == VARIANT_FALSE)
	{
		ASSERT(false);
		return false;
	}

	MSXML2::IXMLDOMNodeListPtr pGroupList = pXmlDoc->GetchildNodes();
	if (pGroupList->Getlength() != 2)
		return false;

	MSXML2::IXMLDOMNodePtr pGroup = pGroupList->Getitem(1);
	MSXML2::IXMLDOMNodeListPtr pNodeList = pGroup->GetchildNodes();
	//std::vector<CAstNodeInfo> ASTInfo;
	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
		CString strName = pNode->GetnodeName();
		if (strName.CompareNoCase(_T("Node")) != 0)
			continue;

		TraverseAstXml(pXmlDoc, pNode, nodeinfos,_T(""), TRUE);
	}

	return true;
}

void CPBOMUtil::TraverseAstXml(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<CAstNodeInfo> &ASTInfo,CString strParentNum,BOOL bIsRoot)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
	
	CString strNameVal = "";
	CAstNodeInfo temp;
	temp.SetParentNumber(strParentNum);
	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pSubNode = pNodeList->Getitem(i);
		CString strName = pSubNode->GetnodeName();
		if (strName.CompareNoCase(_T("Params")) == 0)
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList1 = pSubNode->GetchildNodes();

			for (int j = 0; j < pNodeList1->Getlength(); j++)
			{
				MSXML2::IXMLDOMNodePtr pParam = pNodeList1->Getitem(j);
				MSXML2::IXMLDOMElementPtr pItem = pParam;

				CString strAttrName = pItem->getAttribute(_T("name"));
				CString strAttrValue = pItem->getAttribute(_T("value"));
				strAttrName = strAttrName.Trim();

				if (/*strAttrName.CompareNoCase(_T("名称")) == 0 || */strAttrName.CompareNoCase(_T("代号")) == 0)
				{
					strNameVal = strAttrValue;
					temp.SetNumber(strNameVal);
					break;
				}
			}

			auto Node = pSubNode->GetparentNode();
			auto s = Node->Getattributes();

			for (auto j = 0; j < s->Getlength(); ++j)
			{
				auto item = s->Getitem(j);
				CString name = item->GetnodeName();
				CString value = item->GetnodeValue();
				if (name.CompareNoCase(_T("instanceid"))==0)
				{
					temp.SetId(value);
					continue;
				}
				temp.AddProp(name,value);
			}
		}
		else if (strName.CompareNoCase(_T("Position")) == 0)
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList1 = pSubNode->GetchildNodes();
			for (auto j = 0; j < pNodeList1->Getlength();++j)
			{
				MSXML2::IXMLDOMNodePtr postion = pNodeList1->Getitem(j);
				CString strName1 = postion->GetnodeName();
				CString strvalue1 = postion->Gettext();
				temp.AddPosition(strvalue1);
			}
		}
		else if (strName.CompareNoCase(_T("Node")) == 0)
		{
			MSXML2::IXMLDOMElementPtr pItem = pSubNode;
			CString strNameVal1 = pItem->getAttribute(_T("name"));
			TraverseAstXml(pXmlDoc, pSubNode,ASTInfo,temp.GetNumber());
		}
	}
	if (strParentNum.IsEmpty())
	{
		auto s = pNode->Getattributes();

		for (auto j = 0; j < s->Getlength(); ++j)
		{
			auto item = s->Getitem(j);
			CString name = item->GetnodeName();
			CString value = item->GetnodeValue();
			if (name.CompareNoCase(_T("instanceid")) == 0)
			{
				temp.SetId(value);
				continue;
			}
			temp.AddProp(name, value);
		}
	}

	ASTInfo.push_back(temp);

	return;
}

bool CPBOMUtil::GetWindchillNode(TiXmlElement * pElement, CWindNode & oNodeData,vector<CWindRelation> &relationVec)
{
	if (pElement == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//CString strNumber = _T("");
	TiXmlElement* pChildren = NULL;
	auto elem = pElement->FirstChildElement();
	auto t_1 = GetTickCount();
	
	while (elem != NULL)
	{
		CString strKey = elem->Value();
		CString strValue = elem->GetText();
		if (strKey.CompareNoCase(_T("chindren")) == 0)
		{
			pChildren = elem;
			//GetWindRelation(elem,oNodeData,relationVec);
		}
		else
		{
			if (strKey.CompareNoCase(_T("Number")) == 0)
			{
				oNodeData.m_strNumber = strValue;
				//strNumber = strValue;
			}
			oNodeData.AddProp(strKey,strValue);
		}
		elem = elem->NextSiblingElement();

	}
	//把其他属性获取后再取children节点信息，避免children节点在number时取不到number信息
	if (pChildren)
	{
		GetWindRelation(pChildren,oNodeData,relationVec);
	}
	return true;
}
extern vector<string> split(string str, string pattern);
//
//vector<string> split(string str, string pattern)
//{
//	string::size_type pos;
//	vector<string> result;
//	str += pattern;
//	auto size = str.size();
//	for (int i = 0; i < size; ++i)
//	{
//		pos = str.find(pattern, i);
//		if (pos < size)
//		{
//			auto s = str.substr(i, pos - i);
//			result.emplace_back(s);
//			i = pos + pattern.size() - 1;
//		}
//	}
//	return result;
//}

bool CPBOMUtil::GetWindRelation(TiXmlElement * pElement,  CWindNode& pNodeData,vector<CWindRelation> &relationVec)
{
	ASSERT(pElement);
	if (pElement==NULL)
	{
		return false;
	}
	CString strValue = pElement->GetText();
	if (strValue.IsEmpty())
	{
		pNodeData.m_bHasChild = false;
		return true;
	}
	pNodeData.m_bHasChild = true;
	string strTmp = strValue;

	/*vector<CString> strChilds;
	strChilds.clear();

	CString strChildContend = _T("");
	int nPos = 0;
	while((nPos = strTmp.Find('|',nPos))>=0)
	{
		strChildContend = strTmp.Left(nPos);
		strTmp.Mid(nPos+1);
		strChilds.push_back(strChildContend);
	}
	if (!strTmp.IsEmpty())
	{
		strChilds.push_back(strTmp);
	}*/
	vector<string> Children;
	Children = split(strTmp, "|");   //每个children以 |分割
	//auto t_1 = GetTickCount();
	for (auto child = Children.begin(); child != Children.end(); ++child)
	{
		vector<string> content;
		content = split(*child, ";;;");

		CWindRelation* pRelationTmp = new CWindRelation;
		pRelationTmp->m_strParentNumber = pNodeData.m_strNumber;

	/*	for (auto i = 0; i < content.size(); ++i)
		{
			switch (i)
			{
			case 0:
				pRelationTmp->m_strNumber = content[i].c_str();
				break;
			case 1:
				pRelationTmp->m_strName = content[i].c_str();
				break;
			case 2:
				pRelationTmp->m_nCount = atof(content[i].c_str());
				break;
			case 3:
				break;

			default:
				break;
			}
		}*/
		for (auto it=content.begin();it!=content.end();++it)
		{
			auto Map = split(*it, "=");
			
			if( CString(Map[0].c_str()) != "Position")
			{
				if (CString(Map[0].c_str())  == "Number" )
				{
					pRelationTmp->m_strNumber = Map[1].c_str();
				}
				else if ("Name"== CString(Map[0].c_str()) )
				{
					pRelationTmp->m_strName =  Map[1].c_str();
				}
				else if ("Count"== CString(Map[0].c_str()) )
				{
					pRelationTmp->m_nCount = atoi( Map[1].c_str());
				}
			}
		}
		
		pNodeData.m_relationMap[pRelationTmp->m_strNumber] = pRelationTmp;
		relationVec.push_back(*pRelationTmp);
		//pNodeData.m_relationVec.Add(pRelationTmp);
		/*delete pRelationTmp;
		pRelationTmp = NULL;*/
	}
	return true;
}

void CPBOMUtil::GetPosition(vector<CWindRelation> &windRelationVec,map<CString,CWindNode*> &windnodes, vector<CAstNodeInfo> &nodeInfos)
{
	vector<CWindRelation> matchVec;
	auto count_relation = windRelationVec.size();
	vector<CWindRelation>::iterator it = windRelationVec.begin();
	for (;it!=windRelationVec.end();it++)
	{
		int nCount = it->m_nCount;
		CString strParent = it->m_strParentNumber;
		CString strNum = it->m_strNumber;
		CString strFileName = _T("");
		CString strSrcname = _T("");
		for (int i = 0; i < nCount; i++)
		{
			int index= -1;
			if (isMatchAst(strParent,strNum,nodeInfos,index))
			{
				it->m_nCount--;
				CWindRelation relation = *it;
				relation.m_nCount = 1;
				relation.SetPosition(nodeInfos[index].GetPosition());
				strFileName = nodeInfos[index].GetPropValue(_T("filename"));
				strSrcname = nodeInfos[index].GetPropValue(_T("srcname"));

				matchVec.push_back(relation);
			}
		}
		if (it->m_nCount > 0)
		{
			matchVec.push_back(*it);
		}
		windnodes[strNum]->m_strFileName = strFileName;
		windnodes[strNum]->m_strSrcname = strSrcname;
	}
	windRelationVec.clear();
	windRelationVec = matchVec;
}

bool CPBOMUtil::isMatchAst(CString strParent, CString strNum, vector<CAstNodeInfo> & nodeInfos, int &index)
{
	for (size_t i = 0; i < nodeInfos.size(); i++)
	{
		CAstNodeInfo &tmp = nodeInfos[i];
		if (tmp.IsMatched())
		{
			continue;
		}
		if (tmp.GetParentNumber() == strParent && tmp.GetNumber() == strNum)
		{
			tmp.SetMatched();
			index = i;
			return true;
		}
	}
	return false;
}
//如果该节点有孩子，则从关系列表里查找所有父亲为该节点的节点，将这些节点挂到该节点下
void CPBOMUtil::Createchild(CWindNode* pNode,TiXmlElement* ele,const vector<CWindRelation> &windRelationVec,map<CString,CWindNode*> &windNodes)
{
	auto ParentNode =pNode;
	auto ParentElem =ele;

	if (ParentNode->m_bHasChild)
	{
		for (auto it1=windRelationVec.begin(); it1 != windRelationVec.end(); it1++)
		{
			CString strParentNumber = it1->m_strParentNumber;
			if (strParentNumber.CompareNoCase(ParentNode->m_strNumber)==0)
			{
				CString strchildNum = it1->m_strNumber;
				CWindNode* pchildNode = windNodes[strchildNum];
				if (pchildNode==NULL)
				{
					continue;
				}
				TiXmlElement* pNodeEle = CreateElement(pchildNode,it1->m_position);

				if (pNodeEle == NULL)
				{
					continue;
				}

				if (ParentElem)
				{
					ParentElem->LinkEndChild(pNodeEle);
				}

				Createchild(pchildNode,pNodeEle,windRelationVec,windNodes);
			}
		}
	}
	else
		return ;
}



CString CPBOMUtil::CreatePbomXml(const vector<CWindRelation> &windRelationVec, map<CString,CWindNode*> &windNodes,CString astxml)
{
	
	map<CString, TiXmlElement*> parentElement_map;
	parentElement_map.clear();

	map<TiXmlElement *, TiXmlElement*> flag;   //记录父子节点是否被创建；若创建key有值，否则没有值
	TiXmlDocument doc;
	TiXmlDeclaration* dec = new TiXmlDeclaration("1.0", "utf-8", "yes");
	doc.LinkEndChild(dec);

	auto Ebom = new TiXmlElement("KMPP_EBOM");
	doc.LinkEndChild(Ebom);

	//生成根node
	CWindNode* pRootNode = windNodes[m_strRootNumber];
	if (pRootNode == NULL)
	{
		return _T("");
	}

	CPosition position;
	TiXmlElement* pRoot = CreateElement(pRootNode, position);
	Ebom->LinkEndChild(pRoot);
	parentElement_map[m_strRootNumber] = pRoot;

	vector<CWindRelation>::const_iterator it = windRelationVec.begin();
	for (; it != windRelationVec.end(); it++)
	{
		/*
		CString strParent = it->m_strParentNumber;
		TiXmlElement* pParentElement = parentElement_map[strParent];

		if (pParentElement == NULL)
		{
			CPosition tmpPos;//需要找
			//创建父节点位置信息为默认
			pParentElement = CreateElement(windNodes[strParent], tmpPos);
			if (pParentElement)
			{
				parentElement_map[strParent] = pParentElement;
			}
		}

		CString strNum = it->m_strNumber;
		CWindNode* pNode = windNodes[strNum];
		if (pNode == NULL)
		{
			continue;
		}

		//根据数量进行创建
		int count = 0;
		while (it->m_nCount > count)
		{
			TiXmlElement* pNodeElem = CreateElement(pNode, it->m_position);

			if (pNodeElem == NULL)
			{
				continue;
			}

			//创建的逻辑  为自上而下进行创建 创建的信息有误

			if (pParentElement)
			{
				std::vector<CString>sort;
				auto pchild = pNodeElem;
				auto childstr = strNum;
				auto Parent = strParent;
				sort.push_back(childstr);
				while (Parent != m_strRootNumber)
				{
					vector<CWindRelation>::const_iterator it1 = windRelationVec.begin();

					for (it1; it1 != windRelationVec.end(); it1++)
					{
						if (it1->m_strNumber == Parent)
						{
							break;
						}
					}
					Parent = it1->m_strParentNumber;
					childstr = strParent;
					sort.push_back(childstr);
				}

				sort.push_back(m_strRootNumber);

				for (auto it2 = sort.rbegin(); it2 != sort.rend() - 1; ++it2)
				{
					auto pos = parentElement_map.find(*it2);
					if (pos != parentElement_map.end())
					{
						auto pParent = parentElement_map[*it2];
						auto child = *(it2 + 1);
						pos = parentElement_map.find(child);
						if (pos != parentElement_map.end())
						{
							auto pChild = parentElement_map[child];
							if (pChild)
							{
								auto posParetn = flag.find(pParent);
								auto posChild = flag.find(pChild);
								if (posParetn == posChild || posChild == flag.end())
								{
									pParent->LinkEndChild(pChild);
									flag[pParent] = pChild;
								}
								else
								{
									auto position = pChild->FirstChildElement()->NextSiblingElement();
									auto posinfo =it->m_position;
									int i = 0;
									auto v1 = position->FirstChildElement();

									while (i<12)
									{
										auto num = (double *)(&posinfo)+i;
										auto text = v1->FirstChild();
										text->SetValue(to_string(static_cast<long double>(*num)).c_str());
										v1 = v1->NextSiblingElement();
										++i;
									}
								}
							}
						}
						else
						{
							pParent->LinkEndChild(pNodeElem);
							flag[pParent] = pNodeElem;
						}
					}
				}
			}
			count++;
		}
		*/

		CString strParent = it->m_strParentNumber;
		TiXmlElement* pParentElement = parentElement_map[strParent];

		if (pParentElement == NULL)
		{
			CPosition tmpPos;//需要找

			pParentElement = CreateElement(windNodes[strParent],tmpPos);
			if (pParentElement)
			{
				parentElement_map[strParent] = pParentElement;
			}
		}


		CString strNum = it->m_strNumber;
		CWindNode* pNode = windNodes[strNum];
		if (pNode==NULL)
		{
			continue;
		}

		TiXmlElement* pNodeElem = CreateElement(pNode,it->m_position);

		if (pNodeElem == NULL)
		{
			continue;
		}

		if (pParentElement)
		{
			pParentElement->LinkEndChild(pNodeElem);
		}

		//Createchild(pNode,pNodeElem,windRelationVec,windNodes);
		auto ParentNode =pNode;
		auto ParentElem =pNodeElem;

		if (ParentNode->m_bHasChild)
		{
			for (auto it1=windRelationVec.begin(); it1 != windRelationVec.end(); it1++)
			{
				CString strParentNumber = it1->m_strParentNumber;
				if (strParentNumber.CompareNoCase(ParentNode->m_strNumber)==0)
				{
					CString strchildNum = it1->m_strNumber;
					CWindNode* pchildNode = windNodes[strchildNum];
					if (pchildNode==NULL)
					{
						continue;
					}
					TiXmlElement* pNodeEle = CreateElement(pchildNode,it1->m_position);

					if (pNodeEle == NULL)
					{
						continue;
					}

					if (ParentElem)
					{
						ParentElem->LinkEndChild(pNodeEle);
					}

					Createchild(pchildNode,pNodeEle,windRelationVec,windNodes);
				}
			}
		}
	}


	doc.SaveFile(astxml);
	return astxml.GetBuffer();
}

TiXmlElement* CPBOMUtil::CreateElement( CWindNode* pNode,const CPosition &pos)
{
	ASSERT(pNode);
	if (pNode==NULL)
	{
		return NULL;
	}

	TiXmlElement* pElement = new TiXmlElement(_T("Node"));
	int id = 0;
	CString strType = _T("prt");
	CString strFileName = pNode->m_strFileName;

	id = pNode->m_bHasChild?m_nAsmId++:m_nPrtId++;
	strType = pNode->m_bHasChild?_T("asm"):strType;
	CString strId = _T("");
	strId.Format(_T("%d"),id);
	pElement->SetAttribute(_T("instanceid"),id);
	pElement->SetAttribute(_T("name"),pNode->GetPropValue(_T("Name")));
	pElement->SetAttribute(_T("type"),strType);
	pElement->SetAttribute(_T("srcname"), pNode->m_strSrcname);

	CWindChillSetting::PROCUDEID.push_back(to_string((long long )id));
	if (strType.CompareNoCase("prt")==0)
	{
		if (strFileName.IsEmpty())
		{
			pElement->SetAttribute(_T("filename"), pNode->GetPropValue(_T("Number"))+".hsf");
		}
		else
			pElement->SetAttribute(_T("filename"), strFileName);
	}

	TiXmlElement* pParams = new TiXmlElement(_T("Params"));
	CWindChillSetting *pSetting = CWindChillSetting::GetAppSettings();
	
	int prop_count = pSetting->m_strRelMatch.GetCount();
	for (int i = 0; i < prop_count; i++)
	{
		CString strWindPropName = pSetting->m_strRelMatch.GetAt(i).m_strValue;
		CString strAstPropName = pSetting->m_strRelMatch.GetAt(i).m_strName;
		if (strWindPropName.IsEmpty())
		{
			continue;
		}
		if (strWindPropName.IsEmpty())
		{
			continue;
		}


		CString strValue = pNode->GetPropValue(strWindPropName);
		TiXmlElement* pParam = new TiXmlElement(_T("Param"));
		pParam->SetAttribute(_T("name"),strAstPropName);
		pParam->SetAttribute(_T("value"),strValue);
		pParams->LinkEndChild(pParam);
	}
	pElement->LinkEndChild(pParams);

	CString strPos = _T("");
	TiXmlElement* pPosition = new TiXmlElement(_T("Position"));
	TiXmlElement* pPos1 = new TiXmlElement(_T("V1"));
	strPos.Format(_T("%.6f"),pos.v1);
	TiXmlText *pText1 = new TiXmlText(strPos);
	pPos1->LinkEndChild(pText1);
	pPosition->LinkEndChild(pPos1);

	TiXmlElement* pPos2 = new TiXmlElement(_T("V2"));
	strPos.Format(_T("%.6f"),pos.v2);
	TiXmlText *pText2 = new TiXmlText(strPos);
	pPos2->LinkEndChild(pText2);
	pPosition->LinkEndChild(pPos2);

	TiXmlElement* pPos3 = new TiXmlElement(_T("V3"));
	strPos.Format(_T("%.6f"),pos.v3);
	TiXmlText *pText3 = new TiXmlText(strPos);
	pPos3->LinkEndChild(pText3);
	pPosition->LinkEndChild(pPos3);

	TiXmlElement* pPos4 = new TiXmlElement(_T("V4"));
	strPos.Format(_T("%.6f"),pos.v4);
	TiXmlText *pText4 = new TiXmlText(strPos);
	pPos4->LinkEndChild(pText4);
	pPosition->LinkEndChild(pPos4);

	TiXmlElement* pPos5 = new TiXmlElement(_T("V5"));
	strPos.Format(_T("%.6f"),pos.v5);
	TiXmlText *pText5 = new TiXmlText(strPos);
	pPos5->LinkEndChild(pText5);
	pPosition->LinkEndChild(pPos5);

	TiXmlElement* pPos6 = new TiXmlElement(_T("V6"));
	strPos.Format(_T("%.6f"),pos.v6);
	TiXmlText *pText6 = new TiXmlText(strPos);
	pPos6->LinkEndChild(pText6);
	pPosition->LinkEndChild(pPos6);

	TiXmlElement* pPos7 = new TiXmlElement(_T("V7"));
	strPos.Format(_T("%.6f"),pos.v7);
	TiXmlText *pText7 = new TiXmlText(strPos);
	pPos7->LinkEndChild(pText7);
	pPosition->LinkEndChild(pPos7);

	TiXmlElement* pPos8 = new TiXmlElement(_T("V8"));
	strPos.Format(_T("%.6f"),pos.v8);
	TiXmlText *pText8 = new TiXmlText(strPos);
	pPos8->LinkEndChild(pText8);
	pPosition->LinkEndChild(pPos8);

	TiXmlElement* pPos9 = new TiXmlElement(_T("V9"));
	strPos.Format(_T("%.6f"),pos.v9);
	TiXmlText *pText9 = new TiXmlText(strPos);
	pPos9->LinkEndChild(pText9);
	pPosition->LinkEndChild(pPos9);

	TiXmlElement* pPos10 = new TiXmlElement(_T("T1"));
	strPos.Format(_T("%.6f"),pos.t1);
	TiXmlText *pText10 = new TiXmlText(strPos);
	pPos10->LinkEndChild(pText10);
	pPosition->LinkEndChild(pPos10);

	TiXmlElement* pPos11 = new TiXmlElement(_T("T2"));
	strPos.Format(_T("%.6f"),pos.t2);
	TiXmlText *pText11 = new TiXmlText(strPos);
	pPos11->LinkEndChild(pText11);
	pPosition->LinkEndChild(pPos11);

	TiXmlElement* pPos12 = new TiXmlElement(_T("T3"));
	strPos.Format(_T("%.6f"),pos.t3);
	TiXmlText *pText12 = new TiXmlText(strPos);
	pPos12->LinkEndChild(pText12);
	pPosition->LinkEndChild(pPos12);
	pElement->LinkEndChild(pPosition);

	return pElement;
}

CAstNodeInfo::CAstNodeInfo()
{	
	m_strId = _T("0");
	m_strNumber = _T("");
	m_strParentNumber = _T("");
	m_props.clear();
	m_position.clear();
	m_bMatched = false;
}

CAstNodeInfo::CAstNodeInfo(const CAstNodeInfo& val)
{
	Copy(val);
}

CAstNodeInfo::~CAstNodeInfo()
{
	m_props.clear();
	m_position.clear();
}

void CAstNodeInfo::AddProp(CString strName,CString strValue)
{
	m_props[strName] = strValue;
}

CAstNodeInfo& CAstNodeInfo::operator=(const CAstNodeInfo& val)
{
	Copy(val);
	return *this;
}

void CAstNodeInfo::Copy(const CAstNodeInfo& val)
{
	if (this != &val)
	{
		this->m_strId = val.m_strId;
		this->m_strNumber = val.m_strNumber;
		this->m_strParentNumber = val.m_strParentNumber;
		this->m_position.clear();
		this->m_position = val.m_position;
		this->m_props.clear();
		this->m_props = val.m_props;
		this->m_bMatched = val.m_bMatched;
	}
}

void CAstNodeInfo::AddPosition(CString strName, CString strvalue)
{
	//m_position[strName] = strvalue;
}

void CAstNodeInfo::AddPosition(CString strValue)
{
	m_position.push_back(strValue);
}

CString CAstNodeInfo::GetPropValue(CString strName)
{
	map<CString,CString>::iterator it = m_props.find(strName);
	if (it!=m_props.end())
	{
		return it->second;
	}
	return _T("");
}

CWindNode::CWindNode()
{
	m_props.clear();
	m_relationMap.clear();
	//m_relationMap.RemoveAll();
	//m_relationVec.RemoveAll();
	m_strNumber = _T("");
	m_strFileName = _T("");
	m_bHasChild = false;
}

CWindNode::CWindNode(const CWindNode& val)
{
	Copy(val);
}

CWindNode::~CWindNode()
{

}

CWindNode & CWindNode::operator=(const CWindNode & val)
{
	Copy(val);
	return *this;
}

void CWindNode::AddProp(CString strKey, CString strValue)
{
	m_props[strKey] = strValue;
}

void CWindNode::Copy(const CWindNode& val)
{
	if (this!=&val)
	{
		m_strNumber = val.m_strNumber;	
		m_props.clear();
		m_props = val.m_props;
		m_relationMap.clear();
		m_relationMap = val.m_relationMap;
		m_bHasChild = val.m_bHasChild;
		m_strFileName = val.m_strFileName;
	}
}

CString CWindNode::GetPropValue(CString strName)
{
	map<CString,CString>::iterator it = m_props.find(strName);
	if (it!=m_props.end())
	{
		return it->second;
	}
	return _T("");
}

CWindRelation::CWindRelation()
{
	m_strNumber = _T("");
	m_strParentNumber = _T("");
	m_strName = _T("");
	m_strUnit = _T("");
	m_props.clear();
	m_nCount = 0;
}

CWindRelation::CWindRelation(const CWindRelation& val)
{
	Copy(val);
}

CWindRelation::~CWindRelation()
{
	m_props.clear();
}

void CWindRelation::Copy(const CWindRelation & val)
{
	if (this != &val)
	{
		this->m_props.clear();
		this->m_props = val.m_props;
		this->m_strNumber = val.m_strNumber;
		this->m_strParentNumber = val.m_strParentNumber;
		this->m_strUnit = val.m_strUnit;
		this->m_nCount = val.m_nCount;
		m_position = val.m_position;
	}
}

void CWindRelation::SetPosition(vector<CString> position)
{
	if (position.size() != 12)
	{
		return;
	}
	int i = 0;
	m_position.v1 = atof(position[i++]);
	m_position.v2 = atof(position[i++]);
	m_position.v3 = atof(position[i++]);
	m_position.v4 = atof(position[i++]);
	m_position.v5 = atof(position[i++]);
	m_position.v6 = atof(position[i++]);
	m_position.v7 = atof(position[i++]);
	m_position.v8 = atof(position[i++]);
	m_position.v9 = atof(position[i++]);
	m_position.t1 = atof(position[i++]);
	m_position.t2 = atof(position[i++]);
	m_position.t3 = atof(position[i++]);
}

CString CWindRelation::GetProp(CString strName)
{
	map<CString,CString>::iterator it = m_props.find(strName);
	if (it!=m_props.end())
	{
		return it->second;
	}
	return _T("");
}

CWindRelation & CWindRelation::operator=(const CWindRelation & val)
{
	Copy(val);
	return *this;
}

CPosition::CPosition()
{
	v1 = 1.0000000;
	v2 = 0.0000000;
	v3 = 0.0000000;
	v4 = 0.0000000;
	v5 = 1.0000000;
	v6 = 0.0000000;
	v7 = 0.0000000;
	v8 = 0.0000000;
	v9 = 1.0000000;
	t1 = 0.0000000;
	t2 = 0.0000000;
	t3 = 0.0000000;
}

CPosition::~CPosition()
{

}
