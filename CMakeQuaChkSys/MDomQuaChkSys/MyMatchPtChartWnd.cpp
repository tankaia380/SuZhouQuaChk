// MyMatchPtChartWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "MyMatchPtChartWnd.h"
#include "ChartAxisLabel.h"

// CMyMatchPtChartWnd

IMPLEMENT_DYNAMIC(CMyMatchPtChartWnd, CDockablePane)

CMyMatchPtChartWnd::CMyMatchPtChartWnd()
{

}

CMyMatchPtChartWnd::~CMyMatchPtChartWnd()
{
}


BEGIN_MESSAGE_MAP(CMyMatchPtChartWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CMyMatchPtChartWnd message handlers




void CMyMatchPtChartWnd::CreateChartItem()
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_DataChart.Create(this, rectDummy, IDC_DATA_CHART));
	m_DataChart.ShowWindow(true);
	m_pAxisL = m_DataChart.GetLeftAxis();
	m_pAxisL->SetMinMax(0, 0);
	m_pAxisL->SetAutomatic(true);
	m_pAxisL->GetLabel()->SetText("Overlap_Y");
	m_pAxisB = m_DataChart.GetBottomAxis();
	m_pAxisB->SetMinMax(0, 0);
	m_pAxisB->SetAutomatic(true);
	m_pAxisB->GetLabel()->SetText("Image ID");

	m_pLineSerie = m_DataChart.AddSerie(1);
	m_pLineSerie->SetSeriesOrdering(CChartSerie::soXOrdering);  //ÉèÖÃÎªÎÞÐò
}

void CMyMatchPtChartWnd::FillDataChart(double *pValue, int num, double dMax, double dMin)
{
	if (m_pLineSerie == NULL)
	{
		m_pLineSerie = m_DataChart.AddSerie(1);
		m_pLineSerie->SetSeriesOrdering(CChartSerie::soXOrdering);
	}
	m_pLineSerie->ClearSerie();
	m_pAxisL->SetMinMax(dMin - fabs(dMin*0.1), dMax + fabs(dMax*0.1));
	m_pAxisB->SetMinMax(-1, num*2);
	

	double *pX = new double[num]; for (int i = 0; i < num; i++) *(pX + i) = i;
	double *pY = pValue;
	m_pLineSerie->AddPoints(pX, pY, num);
	delete[]pX;
	m_DataChart.EnableRefresh(true);
}

void CMyMatchPtChartWnd::UpdateFonts()
{
	m_DataChart.SetFont(&afxGlobalData.fontRegular);
}
void CMyMatchPtChartWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}
	CRect rectClient, rectCombo;
	GetClientRect(rectClient);
	m_DataChart.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CMyMatchPtChartWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CreateChartItem();
	return 0;
}


void CMyMatchPtChartWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
	double pDat[10] = { 1,2,3,4,5,6,7,8,9,10 };
	FillDataChart(pDat, 10, 11, -1);
	// TODO: Add your message handler code here
}


BOOL CMyMatchPtChartWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
//	return TRUE;
	return CDockablePane::OnEraseBkgnd(pDC);
}
