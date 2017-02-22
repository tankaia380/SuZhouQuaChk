#pragma once


// CMyMatchPointChartDlg dialog
#include "ChartCtrl.h"
#include "MyFunctions.h"
#include "afxwin.h"
class CMyMatchPointChartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMyMatchPointChartDlg)

public:
	CMyMatchPointChartDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMyMatchPointChartDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MATCHPOINT_CHART };
#endif
public:
	void FillChart(MstuMchModel &model);
protected:
	CEdit m_EditInfo;
	CChartCtrl m_DataChartX;
	CChartSerie *m_pLineSerieX;
	CChartAxis *m_pAxisLX;
	CChartAxis *m_pAxisBX;

	CChartCtrl m_DataChartY;
	CChartSerie *m_pLineSerieY;
	CChartAxis *m_pAxisLY;
	CChartAxis *m_pAxisBY;
protected:
	void CreateChart();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnFillChart(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CString m_strEditInfo;
	CEdit m_pEditInfo;
};
