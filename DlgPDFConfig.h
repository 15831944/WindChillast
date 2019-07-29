#pragma once
#include "Resource.h"
#include "MyCtrl.h"
#include <map>
#include <vector>
#include <set>
#include "CAcroAXDocShim.h"

//#include "CAcroAXDocShim.h"
// CDlgProcRule 对话框
class CProcess;
class CActionBase;
class CAsmStep;
class CAPSGridPropData;
// CDlgPDFConfig 对话框

class CDlgPDFConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgPDFConfig)

public:
	CDlgPDFConfig(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPDFConfig();
	CProcess *m_pProcess;
// 对话框数据
	enum { IDD = IDD_WINDCHILL_PDFCONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedWINDCHILLProcruleBtnAdd();
	afx_msg void OnBnClickedWINDCHILLProcruleBtnDelete();
	afx_msg void OnBnClickedWINDCHILLProcruleBtnMoveup();
	afx_msg void OnBnClickedWINDCHILLProcruleBtnMovedown();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnItemChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSelChanged(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnItemDblClk(WPARAM wp, LPARAM lp);

	void setTemplateName(CString sName);
	void setTemplateType(int iType);
	void SetProcess(CProcess *pProcess);
	bool ParseConfigFile(CString strxml, CStringArray & PDFTempLateProcArray, CStringArray & PDFTempLateStepArray);
	void GetStepTemp(CStringArray &StrStepTemp);  //从默认配置里获取到步骤上的模板
	void SaveConfig();
	int getMode();
private:
	int m_iType;
	std::map<CString, int> m_pPageTypeMap;
	CAcroAXDocShim m_ctrl;
	BOOL m_bCtrlSucc;

	CString m_sTemplateName;   //导入XML模板的路径及名称
	CStringArray m_ProcPDFArray;   //xml配置文件中所有的Proc类型的pdf模板文件列表
	CStringArray m_StepPDFArray;   //xml配置文件中所有的Step类型的pdf模板文件列表
	CMyGridCtrl *m_ProcList;
	CAPSGridPropData* m_PdfConfigDatas;
	int m_iMode;	//0:取消  1:下一步  2:上一步
public:
	afx_msg void OnBnClickedLast();
};
