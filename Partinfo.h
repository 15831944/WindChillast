#pragma once
#include "tinyxml.h"
#include <map>
#include <vector>
#include <string>
#include <algorithm>


class PartInfo
{
public:

	PartInfo(TiXmlElement *Part = NULL)
	{
		this->Part = Part;
		//GetType();
		value.clear();
		childvalue.clear();
		position.clear();
		Getvalue();
	}

	std::map<std::string, std::string > value;
	std::vector<std::map <std::string,std::string>> childvalue;

	std::map< std::string , std::map <std::string,std::string> > position;
	//position�ؼ���Ϊ<���ڵ�,<id,λ����Ϣ>>


	TiXmlElement *Part;
	void Getvalue();
	void Clear();
};
