#pragma once


// CMyMatchPtChartWnd
#include "ChartCtrl.h"
class CMyMatchPtChartWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CMyMatchPtChartWnd)

public:
	CMyMatchPtChartWnd();
	virtual ~CMyMatchPtChartWnd();

protected:
	DECLARE_MESSAGE_MAP()
	void CreateChartItem();
	void FillDataChart(double *pValue, int num, double dMax, double dMin);
	void UpdateFonts();
	void AdjustLayout();

	CChartCtrl m_DataChart;
	CChartSerie *m_pLineSerie;
	CChartAxis *m_pAxisL;
	CChartAxis *m_pAxisB;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


