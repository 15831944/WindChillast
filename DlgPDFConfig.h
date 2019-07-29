#pragma once
#include "Resource.h"
#include "MyCtrl.h"
#include <map>
#include <vector>
#include <set>
#include "CAcroAXDocShim.h"

//#include "CAcroAXDocShim.h"
// CDlgProcRule �Ի���
class CProcess;
class CActionBase;
class CAsmStep;
class CAPSGridPropData;
// CDlgPDFConfig �Ի���

class CDlgPDFConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgPDFConfig)

public:
	CDlgPDFConfig(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPDFConfig();
	CProcess *m_pProcess;
// �Ի�������
	enum { IDD = IDD_WINDCHILL_PDFCONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	void GetStepTemp(CStringArray &StrStepTemp);  //��Ĭ���������ȡ�������ϵ�ģ��
	void SaveConfig();
	int getMode();
private:
	int m_iType;
	std::map<CString, int> m_pPageTypeMap;
	CAcroAXDocShim m_ctrl;
	BOOL m_bCtrlSucc;

	CString m_sTemplateName;   //����XMLģ���·��������
	CStringArray m_ProcPDFArray;   //xml�����ļ������е�Proc���͵�pdfģ���ļ��б�
	CStringArray m_StepPDFArray;   //xml�����ļ������е�Step���͵�pdfģ���ļ��б�
	CMyGridCtrl *m_ProcList;
	CAPSGridPropData* m_PdfConfigDatas;
	int m_iMode;	//0:ȡ��  1:��һ��  2:��һ��
public:
	afx_msg void OnBnClickedLast();
};
