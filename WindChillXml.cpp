#include "stdafx.h"

#include "WindChillXml.h"
#include "tinyxml.h"
#include "apsprop.h"
#include "WindChillSetting.h"

bool CWindChillXml::ParseResult(CString strResXml)
{
	if (strResXml.IsEmpty())
	{
		return false;
	}

	try
	{
		TiXmlDocument doc;
		doc.Parse(strResXml.GetBuffer());
		strResXml.ReleaseBuffer();

		TiXmlElement *root = doc.FirstChildElement();
		while (CString(root->Value()) != CString("MESSAGE"))
		{
			root = root->FirstChildElement();
		}

		if (root == NULL)
		{
			doc.Clear();
			return false;
		}

		auto Info = root->GetText();

		CString strRes = _T("Success");
		if (strRes.CompareNoCase(Info) == 0)
		{
			return true;
		}
	}
	catch (...)
	{
		ASSERT(false);
	}
	return false;
}



CString CWindChillXml::GetPath(CString strResXml)
{
	if (strResXml.IsEmpty())
	{
		return false;
	}

	try
	{
		TiXmlDocument doc;
		doc.Parse(strResXml.GetBuffer());
		strResXml.ReleaseBuffer();

		TiXmlElement *root = doc.FirstChildElement();
		while ( CString("filePath").CompareNoCase(CString(root->Value())))
		{
			root = root->FirstChildElement();
		}

		if (root == NULL)
		{
			doc.Clear();
			return "";
		}

		auto Info = root->GetText();

		if (Info)
		{
			return Info;
		}
	}
	catch (...)
	{
		ASSERT(false);
	}
	return "";
}

void CWindChillXml::ParesrChangexml(CString strXml)
{
	TiXmlDocument doc;
	
	doc.LoadFile(strXml.GetBuffer());

	strXml.ReleaseBuffer();
	TiXmlElement *CraftOrder = doc.FirstChildElement();

	if (CraftOrder)
	{
		auto indexEle = CraftOrder->FirstChildElement();
		while (indexEle)
		{
			CString text(indexEle->Value());
			if (text.CompareNoCase("Title")==0)
			{
				
				auto titleEle = indexEle->FirstChildElement();
				while (titleEle)
				{
					auto key =titleEle->Value();
					auto value = UTF8toMulitype(titleEle->GetText());

					CWindChillSetting::titleValue[key] = value.c_str();
					titleEle = titleEle->NextSiblingElement();
				}
			}

			if (text.CompareNoCase("Review") == 0)
			{
				auto ReviewAttr = indexEle->FirstAttribute();
				while (ReviewAttr)
				{
					auto key = ReviewAttr->Name();
					auto value = UTF8toMulitype(ReviewAttr->Value());

					auto pos =KmWindChillCommon::FindPropDataIndex(CWindChillSetting::m_strChangeinfoMatch, key);
					if(pos)
					{
						auto item =CWindChillSetting::m_strChangeinfoMatch.GetAt(pos);
						key = item.m_strName;
					}
					

					CWindChillSetting::ReviewValue[key] = value.c_str();
					ReviewAttr = ReviewAttr->Next();
				}
			}

			if (text.CompareNoCase("ChangeList") == 0)
			{
				auto changeList = indexEle->FirstChildElement();
				if (changeList)
				{
					auto change = changeList->FirstChildElement();

					if (change)
					{
						auto changeEle = change->FirstChildElement();

						while (changeEle)
						{
							auto key = changeEle->Value();
							//auto value = changeEle->GetText();
							auto value = UTF8toMulitype(changeEle->GetText());

							CWindChillSetting::changeValue[key] = value.c_str();
							changeEle = changeEle->NextSiblingElement();
						}
					}

				}
			}

			indexEle = indexEle->NextSiblingElement();
		}

	}

	
}

CMap<CString, LPCTSTR, MSXML2::IXMLDOMNodePtr, MSXML2::IXMLDOMNodePtr> CAstXml::m_BomNodemap;
int CAstXml::m_Maxinstanceid = -1;
#if 1
//遍历strAstXml，查找属性名称为PartNumber的Node节点，获取到属性值。查找3rdXml中partToName节点的值等于该属性值的节点partRel，
//将partRel下的属性写入到strAstXml的相应Node节点下。具体写入哪些属性，在WINDCHILLConfig.ini中配置
bool CAstXml::UpdateAttribute(CString strAstXml, std::vector<PartInfo> partinfo, CString strpartFirstName)
{
	m_BomNodemap.RemoveAll();
	m_Maxinstanceid = -1;

	//获取输出xml的所有零部件的属性
	/*WINDCHILLPropMap propmap;
	propmap.RemoveAll();*/

	//CWINDCHILLSetting::m_toolpropmap.RemoveAll();
	//if (!CWINDCHILLXml::ParseCheckOutPBOMXml(str3rdXml, propmap, CWINDCHILLSetting::m_toolpropmap))
	//	return false;

	MSXML2::IXMLDOMDocumentPtr	pXmlDoc(__uuidof(MSXML2::DOMDocument60));
	pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
	if (!pXmlDoc)
	{
		ASSERT(FALSE);
		return false;
	}

	if (pXmlDoc->load((LPCTSTR)(strAstXml)) == VARIANT_FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	// 读取XML文档根节点
	MSXML2::IXMLDOMNodeListPtr pGroupList = pXmlDoc->GetchildNodes();
	if (pGroupList->Getlength() != 2)
		return false;

	MSXML2::IXMLDOMNodePtr pGroup = pGroupList->Getitem(1);
	MSXML2::IXMLDOMNodeListPtr pNodeList = pGroup->GetchildNodes();
	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pNode = pNodeList->Getitem(i);
		CString strName = pNode->GetnodeName();
		if (strName.CompareNoCase("Node") != 0)
			continue;

		TraverseNodeParams(pXmlDoc, pNode, partinfo, TRUE, strpartFirstName);
	}

	//添加tool节点
	//AddToolNode(pXmlDoc, partinfo);  //将未添加的节点，添加进去

	pXmlDoc->save(_variant_t(strAstXml));
	return true;
}



void CAstXml::AddToolNode(MSXML2::IXMLDOMDocumentPtr pXmlDoc, std::vector<PartInfo> &partinfo)
{
	for (auto it = partinfo.begin(); it != partinfo.end();++it)
	{
		CString strPropValue = it->value["Number"].c_str();
		MSXML2::IXMLDOMNodePtr pNode = NULL;
		if (m_BomNodemap.Lookup(strPropValue, pNode))
		{
			CString strKey = strPropValue;
			MSXML2::IXMLDOMElementPtr  Node;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"Node", &Node);
			CString strValue;
			strValue.Format("%d", ++m_Maxinstanceid);
			Node->setAttribute("id", (_bstr_t)strValue);
			Node->setAttribute("instanceid", (_bstr_t)strValue);
			Node->setAttribute("type", (_bstr_t)"prt");
			Node->setAttribute("name", (_bstr_t)strKey);
			strValue.Format("%s.sab", strKey);
			Node->setAttribute("sabfilename", (_bstr_t)strValue);
			strValue.Format("%s.hsf", strKey);
			Node->setAttribute("filename", (_bstr_t)strValue);
			pNode->appendChild(Node);

			//Params
			MSXML2::IXMLDOMElementPtr  Params;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"Params", &Params);
			Node->appendChild(Params);

			CMap<CString, LPCTSTR, MSXML2::IXMLDOMNodePtr, MSXML2::IXMLDOMNodePtr> nodemap;
			nodemap.RemoveAll();
			MSXML2::IXMLDOMElementPtr  Param;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"Param", &Param);
			Param->setAttribute("name", (_bstr_t)"代号");
			Param->setAttribute("value", (_bstr_t)strKey);
			Params->appendChild(Param);
			nodemap.SetAt(strKey, Param);
			UpdateParam(pXmlDoc, Params, partinfo, nodemap, strKey);

			//Position
			MSXML2::IXMLDOMElementPtr  Position;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"Position", &Position);
			Node->appendChild(Position);

			MSXML2::IXMLDOMElementPtr  V1;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V1", &V1);
			V1->Puttext((_bstr_t)"1.000000");
			Position->appendChild(V1);
			MSXML2::IXMLDOMElementPtr  V2;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V2", &V2);
			V2->Puttext((_bstr_t)"0.000000");
			Position->appendChild(V2);
			MSXML2::IXMLDOMElementPtr  V3;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V3", &V3);
			V3->Puttext((_bstr_t)"0.000000");
			Position->appendChild(V3);
			MSXML2::IXMLDOMElementPtr  V4;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V4", &V4);
			V4->Puttext((_bstr_t)"0.000000");
			Position->appendChild(V4);
			MSXML2::IXMLDOMElementPtr  V5;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V5", &V5);
			V5->Puttext((_bstr_t)"1.000000");
			Position->appendChild(V5);
			MSXML2::IXMLDOMElementPtr  V6;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V6", &V6);
			V6->Puttext((_bstr_t)"0.000000");
			Position->appendChild(V6);
			MSXML2::IXMLDOMElementPtr  V7;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V7", &V7);
			V7->Puttext((_bstr_t)"0.000000");
			Position->appendChild(V7);
			MSXML2::IXMLDOMElementPtr  V8;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V8", &V8);
			V8->Puttext((_bstr_t)"0.000000");
			Position->appendChild(V8);
			MSXML2::IXMLDOMElementPtr  V9;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"V9", &V9);
			V9->Puttext((_bstr_t)"1.000000");
			Position->appendChild(V9);
			MSXML2::IXMLDOMElementPtr  T1;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"T1", &T1);
			T1->Puttext((_bstr_t)"0.000000");
			Position->appendChild(T1);
			MSXML2::IXMLDOMElementPtr  T2;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"T2", &T2);
			T2->Puttext((_bstr_t)"0.000000");
			Position->appendChild(T2);
			MSXML2::IXMLDOMElementPtr  T3;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"T3", &T3);
			T3->Puttext((_bstr_t)"0.000000");
			Position->appendChild(T3);
		}
	}

}

void CAstXml::TraverseAstXml(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<ASTXMLInfo> &ASTInfo,CString ParentNumber,BOOL bIsRoot)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
	auto count = pNodeList->Getlength();

	CString strNameVal = "";
	ASTXMLInfo temp;
	temp.parentNumber=ParentNumber;
	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pSubNode = pNodeList->Getitem(i);
		CString strName = pSubNode->GetnodeName();


		if (strName.CompareNoCase("Params") == 0)
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList1 = pSubNode->GetchildNodes();

			for (int j = 0; j < pNodeList1->Getlength(); j++)
			{
				MSXML2::IXMLDOMNodePtr pParam = pNodeList1->Getitem(j);
				MSXML2::IXMLDOMElementPtr pItem = pParam;

				CString strAttrName = (LPCTSTR)(_bstr_t)pItem->getAttribute(_T("name"));
				CString strAttrValue = (LPCTSTR)(_bstr_t)pItem->getAttribute(_T("value"));
				strAttrName=strAttrName.Trim();

				if ( strAttrName.CompareNoCase(_T("代号")) == 0)   //配置
					//if (strAttrName=="名称" || strAttrName=="代号" )
				{
					strNameVal = strAttrValue;
					temp.Number = strNameVal;
					break;
				}
			}


			auto Node = pSubNode->GetparentNode();
			auto s = Node->Getattributes();

			for (auto k = 0; k < s->Getlength(); ++k)
			{
				auto item = s->Getitem(k);
				CString name = item->GetnodeName();
				CString value = item->GetnodeValue();
				if (name.CompareNoCase(_T("instanceid")) == 0)
				{
					temp.id = atoi(value.GetBuffer());
					value.ReleaseBuffer();
				}
				else if (name.CompareNoCase(_T("name")) == 0)
					value = strNameVal;

				temp.Nodeinfo[name] = value;
			}
		}
		else if (strName.CompareNoCase("Position") == 0)
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList1 = pSubNode->GetchildNodes();
			for (auto m = 0; m < pNodeList1->Getlength();++m)
			{
				MSXML2::IXMLDOMNodePtr postion = pNodeList1->Getitem(m);
				CString strName1 = postion->GetnodeName();
				CString strvalue1 = postion->Gettext();
				temp.Postion.push_back(strvalue1);
			}
		}
		else if (strName.CompareNoCase("Node") == 0)
		{
			MSXML2::IXMLDOMElementPtr pItem = pSubNode;
			CString strNameVal1 = pItem->getAttribute(_T("name"));
			TraverseAstXml(pXmlDoc, pSubNode,ASTInfo,temp.Number);
		}
		/*if (temp.Number!=""&&temp.Nodeinfo.size()>0 && temp.Postion.size() > 0)
		{
		auto flag = true;

		for (auto i = 0; i < ASTInfo.size();++i)
		{
		if (ASTInfo[i].Number ==temp.Number)
		{
		flag = false ;
		}
		}

		if (flag)
		{*/

		/*		}
		}	*/
	}
	ASTInfo.push_back(temp);
	return;
}


void CAstXml::TraverseAstXmlNodeParam(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode,std:: vector<CString> &AddorUpdateFile,std::vector<MSXML2::IXMLDOMNodeListPtr> & Node, BOOL bIsRoot)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
	auto count = pNodeList->Getlength();

	CString strNameVal = "";
	ASTXMLInfo temp;
	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pSubNode = pNodeList->Getitem(i);
		CString strName = pSubNode->GetnodeName();

		if (strName.CompareNoCase("Node") == 0)
		{
			MSXML2::IXMLDOMElementPtr pItem = pSubNode;
			CString strNameVal1 = pItem->getAttribute(_T("name"));
			TraverseAstXmlNodeParam(pXmlDoc, pSubNode, AddorUpdateFile,Node);
		}

		if (strName.CompareNoCase("Params") == 0)
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList1 = pSubNode->GetchildNodes();

			for (int j = 0; j < pNodeList1->Getlength(); j++)
			{
				MSXML2::IXMLDOMNodePtr pParam = pNodeList1->Getitem(j);
				MSXML2::IXMLDOMElementPtr pItem = pParam;

				CString strAttrName = pItem->getAttribute(_T("name"));
				CString strAttrValue = pItem->getAttribute(_T("value"));
				if (strAttrName.CompareNoCase("#solid_add_file")==0)
				{
					AddorUpdateFile.push_back(strAttrValue);
					Node.push_back(pNodeList1);
				}
			}
		}
	}
	return;
}



std::map<CString, MSXML2::IXMLDOMNodePtr> AstPostion;
void CAstXml::TraverseNodeParams(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<PartInfo> partinfo, BOOL bIsRoot/*=FALSE*/, CString strpartFirstName/*=""*/)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();
	auto count = pNodeList->Getlength();

	MSXML2::IXMLDOMElementPtr pItem = pNode;
	CString strNameVal1= pItem->getAttribute(_T("name"));


	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pSubNode = pNodeList->Getitem(i);
		CString strName = pSubNode->GetnodeName();
		if (strName.CompareNoCase("Node") == 0)
		{
			TraverseNodeParams(pXmlDoc, pSubNode, partinfo);
		}
		if (strName.CompareNoCase("Params") == 0)
		{
			MSXML2::IXMLDOMElementPtr pItem1 = pNode;
			CString strNameVal = pItem1->getAttribute(_T("name"));
			AppendParam(pXmlDoc, pSubNode, partinfo, strNameVal, bIsRoot, strpartFirstName);
			strNameVal1 = strNameVal;
		}
		if (strName.CompareNoCase("Position") == 0)
		{
			MSXML2::IXMLDOMElementPtr pItem1 = pNode;
			AstPostion[strNameVal1] = pSubNode;
		}
	}
	return;
}

void CAstXml::AppendParam(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<PartInfo> partinfo, CString &strNameVal, BOOL bIsRoot, CString strpartFirstName)
{
	//先遍历属性节点，得到已有的属性节点，与属性列表进行匹配，避免重复添加属性
	CMap<CString, LPCTSTR, MSXML2::IXMLDOMNodePtr, MSXML2::IXMLDOMNodePtr> nodemap;
	nodemap.RemoveAll();
	CString strPartNumberVal = _T("");
	MSXML2::IXMLDOMNodeListPtr pNodeList = pNode->GetchildNodes();

	for (int i = 0; i < pNodeList->Getlength(); i++)
	{
		MSXML2::IXMLDOMNodePtr pParam = pNodeList->Getitem(i);
		MSXML2::IXMLDOMElementPtr pItem = pParam;

		CString strAttrName = pItem->getAttribute(_T("name"));
		CString strAttrValue = pItem->getAttribute(_T("value"));

		if (strAttrName.CompareNoCase("PartNumber") == 0 || strAttrName.CompareNoCase("名称") == 0 || strAttrName.CompareNoCase("#prop_solid_code") == 0)
		{
			strNameVal = strAttrValue;
			//continue;
		}
		nodemap.SetAt(strAttrName, pParam);
	}
	if (strNameVal.IsEmpty())
	{
		return;
	}

	MSXML2::IXMLDOMNodePtr pParentNode = pNode->GetparentNode();
	MSXML2::IXMLDOMElementPtr pItem = pParentNode;

	CString strType = pItem->getAttribute(_T("type"));
	if (strType.CompareNoCase("asm") == 0)
	{
		m_BomNodemap.SetAt(strNameVal, pParentNode);

	}
	else if (strType.CompareNoCase("prt") == 0)
	{
		CString strID = pItem->getAttribute(_T("instanceid"));
		int iID = atoi(strID);
		if (m_Maxinstanceid < iID)
			m_Maxinstanceid = iID;
	}
	UpdateParam(pXmlDoc, pNode, partinfo, nodemap, strNameVal);
}

void CAstXml::UpdateParam(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<PartInfo> partinfo, CMap<CString, LPCTSTR, MSXML2::IXMLDOMNodePtr, MSXML2::IXMLDOMNodePtr> &nodemap, CString strNameVal)
{
	auto part = lookforpartbyNumber(partinfo, strNameVal.GetBuffer());
	for (auto i = 0; i < CWindChillSetting::m_strRelMatch.GetCount(); ++i)
	{
		CString strName = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;
		CString strValue = CWindChillSetting::m_strRelMatch.GetAt(i).m_strValue;
		auto flag = true;
		for (auto it = part.value.begin(); it != part.value.end(); ++it)
		{
			if (strValue == it->first.c_str())
			{
				flag = false;
				MSXML2::IXMLDOMElementPtr  Node;
				pXmlDoc->raw_createElement((_bstr_t)(char*)"Param", &Node);

				if (it->first == "Number")
				{
					Node->setAttribute("name", (_bstr_t)"#prop_name");
				}
				else
				{
					Node->setAttribute("name", (_bstr_t)(strName));
				}

				Node->setAttribute("value", (_bstr_t)(it->second.c_str()));
				pNode->appendChild(Node);

				nodemap.SetAt(strName, Node);
				break;
			}
		}

		if (flag)
		{
			MSXML2::IXMLDOMElementPtr  Node;
			pXmlDoc->raw_createElement((_bstr_t)(char*)"Param", &Node);
			Node->setAttribute("name", (_bstr_t)(strName));
			Node->setAttribute("value", (_bstr_t)(""));
			pNode->appendChild(Node);
			nodemap.SetAt(strName, Node);
		}
	}
}
#endif

