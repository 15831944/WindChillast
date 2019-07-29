#include "StdAfx.h"

#include "PBomCreate.h"
#include "WindChillSetting.h"
#include "UtilityFunction.h"
#include "common.h"


extern vector<string> split(string str, string pattern);
extern bool WFromUTF8(const char* szText, std::wstring &wstrVal);
extern PartInfo lookforpartbyNumber(std::vector<PartInfo> partInfo, std::string cstr);


CPbomCreate::CPbomCreate(CString &PbomPath, CString partNumber)
{
	this->PbomPath = PbomPath;
	this->partNumber = partNumber;
}


CPbomCreate::CPbomCreate(CString partNumber)
{
	this->partNumber = partNumber;

}

BOOL CPbomCreate::CreatePbom()
{
	CWindChillSetting::PROCUDEID.clear();
	TiXmlDocument doc;
	TiXmlDocument Ebomdoc;

	//获取pbom中的内容，去掉第一层utf-8，然后解析
	CFile file;
	file.Open(this->PbomPath, CFile::modeReadWrite);
	auto dwFilelen = file.GetLength();
	char *buf;
	buf = new char[dwFilelen + 1];
	buf[dwFilelen] = 0;

	file.Read(buf, dwFilelen);
	file.Close();
	std::wstring a;

	WFromUTF8(buf, a);
	auto dwNum = WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, NULL, 0, NULL, FALSE);
	char * sabstr;
	sabstr = new char[dwNum + 1];
	WideCharToMultiByte(CP_OEMCP, 0, a.c_str(), -1, sabstr, dwNum, NULL, FALSE);

	//将<?xml version="1.0" encoding="UTF-8"?>去掉  否则影响解析
	CString s =sabstr;
	auto n=s.FindOneOf(">")+1;
	auto tmp =s.Right(s.GetLength()-n);

	Ebomdoc.Parse(tmp.GetBuffer());

	std::vector<PartInfo> partInfo;
	TiXmlElement *root = Ebomdoc.FirstChildElement();
	if (root == NULL || root->FirstChildElement() == NULL)
	{
		Ebomdoc.Clear();
		return FALSE;
	}

	auto elem = root->FirstChildElement()->FirstChildElement();
	while (elem != NULL)
	{
		PartInfo part(elem);

		partInfo.push_back(part);
		auto size = sizeof(partInfo);
		elem = elem->NextSiblingElement();
	}

	TraverPbom(doc, partInfo, this->partNumber);

	auto PbomPath =GetFilePath(this->PbomPath)+"files\\Pbom.xml";
	doc.SaveFile(PbomPath);

	if(buf)
	{
		delete [] buf;
		buf = NULL;
	}
	
	if(sabstr)
	{
		delete [] sabstr;
		sabstr = NULL;
	}
	
	return TRUE;
}


BOOL CPbomCreate::CreatePbom(CString content)
{
	TiXmlDocument doc;
	TiXmlDocument Ebomdoc;


	Ebomdoc.Parse(content.GetBuffer());

	std::vector<PartInfo> partInfo;
	TiXmlElement *root = Ebomdoc.FirstChildElement();
	if (root == NULL || root->FirstChildElement() == NULL)
	{
		Ebomdoc.Clear();
		return FALSE;
	}

	auto elem = root->FirstChildElement()->FirstChildElement();
	while (elem != NULL)
	{
		PartInfo part(elem);

		partInfo.push_back(part);
		auto size = sizeof(partInfo);
		elem = elem->NextSiblingElement();
	}

	TraverPbom(doc, partInfo, this->partNumber);

	doc.SaveFile(this->PbomPath);

	documentAnsiToutf8(this->PbomPath);
	return TRUE;
}


void CPbomCreate::AddPbomToolNode(TiXmlElement *Node, PartInfo &partinfo, pair<string, string> Map, CString parent = "")
{
	std::string type = "";
	if (partinfo.childvalue.size()>0)
	{
		type = "asm";
		Node->SetAttribute("id", (_bstr_t)Map.first.c_str());
		Node->SetAttribute("instanceid", (_bstr_t)Map.first.c_str());
		Node->SetAttribute("type", (_bstr_t)(type.c_str()));
		Node->SetAttribute("name", (_bstr_t)partinfo.value["Number"].c_str());
	}
	else
	{
		type = "prt";
		Node->SetAttribute("id", Map.first.c_str());
		Node->SetAttribute("instanceid", (_bstr_t)Map.first.c_str());
		Node->SetAttribute("type", (_bstr_t)(type.c_str()));
		Node->SetAttribute("name", (_bstr_t)partinfo.value["Number"].c_str());
		Node->SetAttribute("filename", (_bstr_t)partinfo.value["filename"].c_str());
	}

	if (CString( partinfo.value["ISGONGYIJIAN"].c_str()).CompareNoCase(_T("是"))==0)
	{
		CWindChillSetting::GYJID.push_back(Map.first.c_str());
	}
	CWindChillSetting::PROCUDEID.push_back(Map.first.c_str());

	Node->SetAttribute("srcname", (_bstr_t)partinfo.value["srcname"].c_str());

	//Params
	TiXmlElement  * Params = new TiXmlElement("Params");
	Node->LinkEndChild(Params);

	
	for (auto i = 0; i < CWindChillSetting::m_strRelMatch.GetCount(); ++i)
	{
		CString strName = CWindChillSetting::m_strRelMatch.GetAt(i).m_strName;
		CString strValue = CWindChillSetting::m_strRelMatch.GetAt(i).m_strValue;

		auto flag = true;
		for (auto it = partinfo.value.begin(); it != partinfo.value.end(); ++it)
		{

			if (strValue == it->first.c_str())
			{
				flag = false;
				TiXmlElement  * Param = new TiXmlElement("Param");

				Param->SetAttribute("name", (_bstr_t)(strName));
		
				Param->SetAttribute("value", (_bstr_t)it->second.c_str());

				Params->LinkEndChild(Param);
				//nodemap.SetAt(strName, Node);
				break;
			}
			//若返回的xml里无配置文件中的信息，则创建空的信息
		}
		if (flag)
		{

			TiXmlElement  * Param = new TiXmlElement("Param");
			Param->SetAttribute("name", (_bstr_t)(strName));
			Param->SetAttribute("value", (_bstr_t)(""));
			Params->LinkEndChild(Param);
			//nodemap.SetAt(strName, Node);
		}

	}

	auto pos = split(Map.second, ",");

	if (pos.size()<12)
	{
		//Position
		TiXmlElement  *Position = new TiXmlElement("Position");
		Node->LinkEndChild(Position);
		//Node->appendChild(Position);

		TiXmlElement  *V1 = new TiXmlElement("V1");
		TiXmlText * text = new TiXmlText("1.000000");
		V1->LinkEndChild(text);
		Position->LinkEndChild(V1);

		TiXmlElement  *V2 = new TiXmlElement("V2");
		text = new TiXmlText("0.000000");
		V2->LinkEndChild(text);
		Position->LinkEndChild(V2);

		TiXmlElement  *V3 = new TiXmlElement("V3");
		text = new TiXmlText("0.000000");
		V3->LinkEndChild(text);
		Position->LinkEndChild(V3);

		TiXmlElement  *V4 = new TiXmlElement("V4");
		text = new TiXmlText("0.000000");
		V4->LinkEndChild(text);
		Position->LinkEndChild(V4);

		TiXmlElement  *V5 = new TiXmlElement("V5");
		text = new TiXmlText("1.000000");
		V5->LinkEndChild(text);
		Position->LinkEndChild(V5);

		TiXmlElement  *V6 = new TiXmlElement("V6");
		text = new TiXmlText("0.000000");
		V6->LinkEndChild(text);
		Position->LinkEndChild(V6);

		TiXmlElement  *V7 = new TiXmlElement("V7");
		text = new TiXmlText("0.000000");
		V7->LinkEndChild(text);
		Position->LinkEndChild(V7);

		TiXmlElement  *V8 = new TiXmlElement("V8");
		text = new TiXmlText("0.000000");
		V8->LinkEndChild(text);
		Position->LinkEndChild(V8);

		TiXmlElement  *V9 = new TiXmlElement("V9");
		text = new TiXmlText("1.000000");
		V9->LinkEndChild(text);
		Position->LinkEndChild(V9);

		TiXmlElement  *T1 = new TiXmlElement("T1");
		text = new TiXmlText("0.000000");
		T1->LinkEndChild(text);
		Position->LinkEndChild(T1);

		TiXmlElement  *T2 = new TiXmlElement("T2");
		text = new TiXmlText("0.000000");
		T2->LinkEndChild(text);
		Position->LinkEndChild(T2);

		TiXmlElement  *T3 = new TiXmlElement("T3");
		text = new TiXmlText("0.000000");
		T3->LinkEndChild(text);
		Position->LinkEndChild(T3);
	}
	else
	{
		//Position
		TiXmlElement  *Position = new TiXmlElement("Position");
		Node->LinkEndChild(Position);
		//Node->appendChild(Position);

		TiXmlElement  *V1 = new TiXmlElement("V1");
		TiXmlText * text = new TiXmlText(pos[0].c_str());
		V1->LinkEndChild(text);
		Position->LinkEndChild(V1);

		TiXmlElement  *V2 = new TiXmlElement("V2");
		text = new TiXmlText(pos[1].c_str());
		V2->LinkEndChild(text);
		Position->LinkEndChild(V2);

		TiXmlElement  *V3 = new TiXmlElement("V3");
		text = new TiXmlText(pos[2].c_str());
		V3->LinkEndChild(text);
		Position->LinkEndChild(V3);

		TiXmlElement  *V4 = new TiXmlElement("V4");
		text = new TiXmlText(pos[3].c_str());
		V4->LinkEndChild(text);
		Position->LinkEndChild(V4);

		TiXmlElement  *V5 = new TiXmlElement("V5");
		text = new TiXmlText(pos[4].c_str());
		V5->LinkEndChild(text);
		Position->LinkEndChild(V5);

		TiXmlElement  *V6 = new TiXmlElement("V6");
		text = new TiXmlText(pos[5].c_str());
		V6->LinkEndChild(text);
		Position->LinkEndChild(V6);

		TiXmlElement  *V7 = new TiXmlElement("V7");
		text = new TiXmlText(pos[6].c_str());
		V7->LinkEndChild(text);
		Position->LinkEndChild(V7);

		TiXmlElement  *V8 = new TiXmlElement("V8");
		text = new TiXmlText(pos[7].c_str());
		V8->LinkEndChild(text);
		Position->LinkEndChild(V8);

		TiXmlElement  *V9 = new TiXmlElement("V9");
		text = new TiXmlText(pos[8].c_str());
		V9->LinkEndChild(text);
		Position->LinkEndChild(V9);

		TiXmlElement  *T1 = new TiXmlElement("T1");
		text = new TiXmlText(pos[9].c_str());
		T1->LinkEndChild(text);
		Position->LinkEndChild(T1);

		TiXmlElement  *T2 = new TiXmlElement("T2");
		text = new TiXmlText(pos[10].c_str());
		T2->LinkEndChild(text);
		Position->LinkEndChild(T2);

		TiXmlElement  *T3 = new TiXmlElement("T3");
		text = new TiXmlText(pos[11].c_str());
		T3->LinkEndChild(text);
		Position->LinkEndChild(T3);
	}

}

bool CPbomCreate::TravePbomrPart(std::vector<PartInfo> partInfo, PartInfo parttop, TiXmlElement *Node)
{
	if (parttop.childvalue.size()>0)
	{
		std::vector<PartInfo> HasChild;
		for (auto child = parttop.childvalue.begin(); child != parttop.childvalue.end(); ++child)
		{
			for (auto it = child->begin(); it != child->end(); ++it)
			{
				if (it->first == "Number")
				{
					auto part = lookforpartbyNumber(partInfo, it->second);
					HasChild.push_back(part);
				}
			}
		}

		for (auto it = HasChild.begin(); it != HasChild.end(); ++it)
		{
			auto number = 0;
			for (auto childitem = parttop.childvalue.begin(); childitem != parttop.childvalue.end(); ++childitem)
			{
				if ((*childitem)["Number"] == it->value["Number"])
				{
					number = atoi((*childitem)["Count"].c_str());
					break;
				}
			}

			for (int i = 0; i < number; ++i)
			{
				//auto idPostion = parttop.position[it->value["Number"]].begin()+i;

				//位置信息和ID在parttop下的 position
				auto value = parttop.position[it->value["Number"]];
				int key = 0;

				auto item = value.begin();

				for (; item != value.end(); ++item)
				{
					if (key == i)
					{
						break;
					}
					key++;
				}

				pair<string, string> Map;
				if (item != value.end())
				{
					Map = *item;
				}

				TiXmlElement *child = new TiXmlElement("Node");
				if (Map.first != "")
				{
					AddPbomToolNode(child, *it, Map, parttop.value["Number"].c_str());
					Node->LinkEndChild(child);
				}
				TravePbomrPart(partInfo, *it, child);
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

void CPbomCreate::TraverPbom(TiXmlDocument &doc, std::vector<PartInfo> partInfo, CString str)
{
	TiXmlDeclaration *xdec = new TiXmlDeclaration("1.0", "UTF-8", "yes");
	doc.LinkEndChild(xdec);

	auto Ebom = new TiXmlElement("KMPP_EBOM");
	doc.LinkEndChild(Ebom);

	auto Node = new TiXmlElement("Node");

	auto  parttop = partInfo.begin();
	for (auto partitem = partInfo.begin(); partitem != partInfo.end(); ++partitem)
	{
		if ( CString( partitem->value["Number"].c_str()).CompareNoCase(str) == 0)
		{
			parttop = partitem;
			pair<string, string> Map("300000001", "1.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000");

			AddPbomToolNode(Node, *parttop, Map);
			Ebom->LinkEndChild(Node);
		}
	}
	TravePbomrPart(partInfo, *parttop, Node);
}
