#include "stdafx.h"

#include <regex>
#include "Partinfo.h"
#include"common.h"
using namespace std;


//vector<string> regsplit(string str, string pattern)
//{
//	std::regex re(pattern.c_str());
//	
//	std::sregex_token_iterator p(str.begin(), str.end(),re,-1);
//	std::sregex_token_iterator end;
//	vector<string> vec;
//	while (p != end)
//	{
//		vec.emplace_back(*p++);
//	}
//	return vec;
//}
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

void PartInfo::Getvalue()
{
	if (Part == NULL)
	{
		return;
	}
	
	auto elem = Part->FirstChildElement();
	//auto t_1 = GetTickCount();
	while (elem != NULL)
	{
		auto key = elem->Value();
		auto word = elem->GetText();
		if (string(key) == string("CHINDREN"))
		{
			if (word)
			{
				auto ChildContent = word;
				vector<string> Children;
				Children = split(ChildContent, "|");   //每个children以 |分割
				
				for (auto child = Children.begin(); child != Children.end(); ++child)
				{
					vector<string> content;
					content = split(*child, ";;;");
					std::map <std::string, std::string > temp;
					childvalue.push_back(temp);

					
					string childNumber;
					for(auto it = content.begin();it!=content.end();++it)
					{
						auto Map = split(*it, "=");
					
						if(Map[0]!="Position"&& Map.size()>=2)
						{
							if ("Number"== Map[0])
							{
								childNumber=Map[1];
							}
							pair<string ,string> p(Map[0], Map[1]);
							childvalue[child - Children.begin()].insert(p);
						}
						else
						{
							if ( Map.size() >= 2&&Map[1] != "")
							{
								auto childcontent =split(Map[1],";");
							
								std::map <std::string,std::string>temp1;
								for (auto childit =childcontent.begin();childit!=childcontent.end();++childit)
								{
									auto childinfo =split(*childit,",");
									
									string pos;
									string instanceid;
									for (auto info =childinfo.begin();info!=childinfo.end();++info)
									{
										auto childMap = split(*info, ":");
										if (childMap[0]=="instanceid")
										{
											instanceid = childMap[1];
										}
										else
										{
											pos+=childMap[1]+",";
										}
									}
									pos[pos.size()-1]='\0';
									temp1[instanceid]=pos;
								}
								position[childNumber]=temp1;
							}
						}
					}
				}
			}
		}
		else
		{
			if (word != NULL)
			{
				pair<string ,string> p(key,word);
				value.insert(p);			
			}
		}
		elem = elem->NextSiblingElement();
	}
}

