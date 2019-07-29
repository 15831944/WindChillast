#include "StdAfx.h"

#include "CreatePbom.h"

extern vector<string> split(string str, string pattern);

bool CreatePbom::PbomCreate(CString strWindchillXml,CString topNumber)
{
	map<CString,CWindNode*> windNodes;
	windNodes.clear();
	vector<CWindRelation> windRelationVec;
	windRelationVec.clear();

	if (!GetWindchillInfo(strWindchillXml,windNodes,windRelationVec, topNumber)) return false;

}

bool CreatePbom::GetWindchillInfo(CString strXml,map<CString,CWindNode*> &nodeinfos,vector<CWindRelation> &relationVec,CString topNumber)
{
	TiXmlDocument Ebomdoc;

	Ebomdoc.Parse(strXml.GetBuffer());
	
	TiXmlElement *root = Ebomdoc.FirstChildElement()->FirstChildElement();
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
	
		nodeinfos[pNodeData->m_strNumber] = pNodeData;
		elem = elem->NextSiblingElement();
	}

	return true;
}

bool CreatePbom::GetWindchillNode(TiXmlElement * pElement, CWindNode &oNodeData,vector<CWindRelation> &relationVec)
{

	if (pElement == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

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
		}
		else
		{
			if (strKey.CompareNoCase(_T("Number")) == 0)
			{
				oNodeData.m_strNumber = strValue;
				
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

bool CreatePbom::GetWindRelation(TiXmlElement * pElement, CWindNode& pNodeData,vector<CWindRelation> &relationVec)
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

	vector<string> Children;
	Children = split(strTmp, "|");   //每个children以 |分割

	for (auto child = Children.begin(); child != Children.end(); ++child)
	{
		vector<string> content;

		content = split(*child, ";;;");

		CWindRelation* pRelationTmp = new CWindRelation;
		pRelationTmp->m_strParentNumber = pNodeData.m_strNumber;

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
	}
	return true;
}
