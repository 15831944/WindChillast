#pragma once

//#include "..\ResourceDef\APM_Res.h"
#include "Resource.h"
//#include <BCG\BCGCBProInc.h>
#include "MyCtrl.h"
//#import "msxml6.dll"

#include <map>
#include <vector>
#include <set>
//#include "APSProp.h"
#include "CAcroAXDocShim.h"

//#include "CAcroAXDocShim.h"
// CDlgProcRule �Ի���
class CProcess;
class CActionBase;
class CAsmStep;
class CAPSGridPropData;


class CDlgProcRule : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcRule)

public:
	CDlgProcRule(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcRule();
	CProcess *m_pProcess;
	//CAcroAXDocShim m_view;
	// �Ի�������
	enum { IDD = IDD_WINDCHILL_PROCRULE };

private:
	CString m_sPath;
	CString m_sName;
	CString m_strPath;
	int m_iType;
	std::map<CString, int> m_pPageTypeMap;
	CStringArray m_arrName;
	CAcroAXDocShim m_ctrl;
	BOOL m_bCtrlSucc;

	//���PDF·��������xmlģ���·��
	CString m_sOutputFilePath;
	CString m_sLoadFilePath;
	CString m_sTemplateName;   //����XMLģ���·��������
	bool m_bExportAction;
	CStringArray m_ProcPDFArray;   //xml�����ļ������е�Proc���͵�pdfģ���ļ��б�
	CStringArray m_StepPDFArray;   //xml�����ļ������е�Step���͵�pdfģ���ļ��б�
	CMyGridCtrl *m_ProcList;
	//stProcTempConfig m_ProcTempConfig;  //������ģ��ؼ��е�����
	CString m_ConfigXMLName;  //���������ѡ���xml
	//CStringArray *m_ConfigProcPDFName;  //�������ģ������
	std::vector<std::map<CString,UINT>> *m_ConfigProcPDFName;  //xxxx

	//CArray<CAPSGridPropData*> m_PdfConfigDatas;
	CAPSGridPropData* m_PdfConfigDatas;
public:
	void setFilePath(CString sPath);
	void setTemplateName(CString sName);
	void setTemplateType(int iType);
	void setFileName(CString sName);

	void getPath(CString &sOutputFilePath, CString &sLoadFilePath);
	void SetProcess(CProcess *pProcess);
	bool getExportAction();
	bool ParseConfigFile(CString strxml, CStringArray & PDFTempLateProcArray, CStringArray & PDFTempLateStepArray);
	bool IsStepInTempPlan(int iStepID, CAPSGridPropData * pPropData);
	void GetProcTempConfig(CArray<CAPSGridPropData*> *pPdfConfigDatas);
	void SetProcTempConfig(CArray<CAPSGridPropData*> *pPdfConfigDatas);
	void GetStepTemp(CStringArray &StrStepTemp);  //��Ĭ���������ȡ�������ϵ�ģ��
	void SaveConfig();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	virtual BOOL OnInitDialog();
	virtual int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelectTemlate();
	afx_msg void OnSave();
	afx_msg void OnClose();
	afx_msg void OnExport();
	afx_msg void OnBnClickedMoveUp();
	afx_msg void OnBnClickedMoveDown();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnDir();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnItemChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSelChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnItemDblClk(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};
