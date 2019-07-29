#pragma once
#include <vector>
#include <string>
#include "Partinfo.h"

enum QueryType
{
	QueryType_Unknown  = -1,
	QueryType_EBOM     = 0,//EBOM查询
	QueryType_PBOM     = 1,//PBOM编辑
	QueryType_RULE     = 2,//工艺规程
	QueryType_Material_PBOM = 3//材料定额中查询 PBOM文件
};

enum ResourceType
{
	ResourceType_Unknown = -1,
	ResourceType_ADDMaterial = 1,//辅材
	ResourceType_EQUIPTOOL = 2,//设备
};

class CAPSPropList;
class CAPSModel;

//获取当前文档的文件名
BOOL GetCurFileName(CAPSModel* pModel, bool bPrompt, CString& strRes);

BOOL GetCurFileName1(CAPSModel* pModel, bool bPrompt, CString& strRes);
//转换UTF8
CString ConvertUTF8(CString szAnsi);  //ansi 转utf-8

std::string UTF8toMulitype(const char *  utf8);   //utf-8 转 ansi

bool LookUpInArray(CStringArray &arr, CString val);
BOOL ConvertModel(CString strFileFullName, CString &strXml);

DWORD GetProcessIDbyName(char *FileName);

std::vector<std::string> split(std::string str, std::string pattern);

PartInfo lookforpartbyNumber(std::vector<PartInfo> partInfo, std::string cstr);

void documentAnsiToutf8(CString strXml);

//若保存的partName中含有非法字符();例如/\.*则不能将文件保存
CString changePartName(CString s=_T(""));

