#pragma once
#include <vector>
#include <string>
#include "Partinfo.h"

enum QueryType
{
	QueryType_Unknown  = -1,
	QueryType_EBOM     = 0,//EBOM��ѯ
	QueryType_PBOM     = 1,//PBOM�༭
	QueryType_RULE     = 2,//���չ��
	QueryType_Material_PBOM = 3//���϶����в�ѯ PBOM�ļ�
};

enum ResourceType
{
	ResourceType_Unknown = -1,
	ResourceType_ADDMaterial = 1,//����
	ResourceType_EQUIPTOOL = 2,//�豸
};

class CAPSPropList;
class CAPSModel;

//��ȡ��ǰ�ĵ����ļ���
BOOL GetCurFileName(CAPSModel* pModel, bool bPrompt, CString& strRes);

BOOL GetCurFileName1(CAPSModel* pModel, bool bPrompt, CString& strRes);
//ת��UTF8
CString ConvertUTF8(CString szAnsi);  //ansi תutf-8

std::string UTF8toMulitype(const char *  utf8);   //utf-8 ת ansi

bool LookUpInArray(CStringArray &arr, CString val);
BOOL ConvertModel(CString strFileFullName, CString &strXml);

DWORD GetProcessIDbyName(char *FileName);

std::vector<std::string> split(std::string str, std::string pattern);

PartInfo lookforpartbyNumber(std::vector<PartInfo> partInfo, std::string cstr);

void documentAnsiToutf8(CString strXml);

//�������partName�к��зǷ��ַ�();����/\.*���ܽ��ļ�����
CString changePartName(CString s=_T(""));

