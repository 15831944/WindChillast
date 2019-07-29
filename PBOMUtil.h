#pragma once
#include <vector>
#include <map>
using namespace std;

class TiXmlElement;

class CPosition
{
public:
	CPosition();
	~CPosition();
public:
	double v1,v2,v3,v4,v5,v6,v7,v8,v9,t1,t2,t3;
};

class CWindRelation
{
public:
	CWindRelation();
	CWindRelation(const CWindRelation& val);
	~CWindRelation();

	CWindRelation & operator= (const CWindRelation & val);
	void Copy(const CWindRelation & val);
	void SetPosition(vector<CString> position);
	CString GetProp(CString strName);
public:
	CString m_strParentNumber;
	CString m_strNumber;
	int m_nCount;
	CString m_strName;
	CString m_strUnit;
	map<CString,CString> m_props;
	//map<CString,CString> m_position;
	CPosition m_position;
	
};


class CWindNode
{
public:
	CWindNode();
	CWindNode(const CWindNode& val);
	~CWindNode();
	CWindNode & operator= (const CWindNode & val);
	void AddProp(CString strKey, CString strValue);
	void Copy(const CWindNode& val);
	CString GetPropValue(CString strName);
public:
	map<CString,CString> m_props;
	CString m_strNumber;
	map<CString,CWindRelation*> m_relationMap;
	bool m_bHasChild;
	//CMap<CString,LPCTSTR,CWindRelation*,CWindRelation*> m_relationMap;
	//CArray<CWindRelation*,CWindRelation*> m_relationVec;
	CString m_strFileName;
	CString m_strSrcname;
};

class CAstNodeInfo
{
public:
	CAstNodeInfo();
	CAstNodeInfo(const CAstNodeInfo& val);
	~CAstNodeInfo();
	CAstNodeInfo& operator=(const CAstNodeInfo& val);
	CString GetId() const { return m_strId; }
	void SetId(CString val) { m_strId = val; }
	CString GetNumber() const { return m_strNumber; }
	void SetNumber(CString val) { m_strNumber = val; }
	CString GetParentNumber() const { return m_strParentNumber; }
	void SetParentNumber(CString val) { m_strParentNumber = val; }
	map<CString,CString> GetProps() const { return m_props; }
	void SetProps(map<CString,CString> val) { m_props = val; }
	vector<CString> GetPosition() const { return m_position; }
	//map<CString,CString> GetPosition() const { return m_position; }
	void SetPosition(vector<CString> val) { m_position = val; }
	//void SetPosition(map<CString,CString> val) { m_position = val; }

	void AddProp(CString strName,CString strValue);
	void AddPosition(CString strName, CString strvalue);
	void AddPosition(CString strValue);

	bool IsMatched() const{return m_bMatched;}
	void SetMatched() {m_bMatched = true;}
	
	CString GetPropValue(CString strName);
private:
	void Copy(const CAstNodeInfo& val);
	
private:
	CString m_strId;
	CString m_strNumber;
	CString m_strParentNumber;
	map<CString,CString> m_props;
	//map<CString,CString> m_position;
	vector<CString> m_position;
	bool m_bMatched;
	
};

class CPBOMUtil
{
public:
	CPBOMUtil(void);
	~CPBOMUtil(void);
public:
	bool CreatePbomExchange(CString strAstXml,CString strWindchillXml,CString topNumber);	//生成pbom.xml   exchange
	bool GetAstInfo(CString strXml, vector<CAstNodeInfo> &nodeinfos);  //exchange

	bool CreatePbomIop(CString strAstXml, CString strWindchillXml, CString topNumber);
	bool GetAstInfoIop(CString strXml, vector<CAstNodeInfo> &nodeinfos);

private:
	bool GetWindchillInfo(CString strXml,map<CString,CWindNode*> &nodeinfos,vector<CWindRelation> &relationVec,CString topNumber);	//获取windchil的xml信息
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

