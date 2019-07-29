#pragma once

#include "afx.h"
#include "tinyxml.h"
#include "PBOMUtil.h"
#include <map>
#include <string>
#include <vector>
#include "comutil.h"

using namespace std;

class CPosition;

class CWindRelation;

class CWindNode;


class CreatePbom
{
public:
	CreatePbom(void)
	{


	}
	~CreatePbom(void)
	{

	}
public:
	bool PbomCreate(CString strWindchillXml,CString topNumber);	//生成pbom.xml   exchange

private:
	bool GetWindchillInfo(CString strWindchillXml,map<CString,CWindNode*> &nodeinfos,vector<CWindRelation> &relationVec,CString topNumber);	//获取windchil的xml信息
	//获取ast轻量化的xml的信息
	void TraverseAstXml(MSXML2::IXMLDOMDocumentPtr pXmlDoc, MSXML2::IXMLDOMNodePtr pNode, std::vector<CAstNodeInfo> &ASTInfo,CString strParentNum,BOOL bIsRoot=FALSE);
	bool GetWindchillNode(TiXmlElement * elem, CWindNode &oNodeData,vector<CWindRelation> &relationVec);
	bool GetWindRelation(TiXmlElement * pElement, CWindNode& pNodeData,vector<CWindRelation> &relationVec);
	void GetPosition(vector<CWindRelation> &windRelationVec,map<CString,CWindNode*> &windnodes,vector<CAstNodeInfo> &nodeInfos);
	bool isMatchAst(CString strParent, CString strNum, vector<CAstNodeInfo> & nodeInfos, int &index);
	CString CreatePbomXml(const vector<CWindRelation> &windRelationVec,  map<CString,CWindNode*> &windNodes,CString astxml);	//生成Pbom.xml
	TiXmlElement* CreateElement( CWindNode* pNode,const CPosition &pos);

	void Createchild(CWindNode* pNode,TiXmlElement* ele, const vector<CWindRelation> &windRelationVec,map<CString,CWindNode*> &windNodes);
protected:
	CString m_strAst;
	CString m_strWindchill;
	CString m_strPbom;
	CString m_strRootNumber;
	int		m_nAsmId;
	int		m_nPrtId;
};