// MyMatchPointChartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "MyMatchPointChartDlg.h"
#include "afxdialogex.h"
#include "ChartAxisLabel.h"
#include "MDomQuaChkSysDoc.h"
// CMyMatchPointChartDlg dialog

IMPLEMENT_DYNAMIC(CMyMatchPointChartDlg, CDialogEx)

CMyMatchPointChartDlg::CMyMatchPointChartDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MATCHPOINT_CHART, pParent)
	, m_strEditInfo(_T(""))
{

}

CMyMatchPointChartDlg::~CMyMatchPointChartDlg()
{
}

void CMyMatchPointChartDlg::FillChart(MstuMchModel& model)
{
	clock_t t1 = clock();
	m_DataChartX.RemoveAllSeries(); m_pLineSerieX = NULL;
	m_DataChartY.RemoveAllSeries(); m_pLineSerieY = NULL;

	if (m_pLineSerieX == NULL)
	{
		m_pLineSerieX = m_DataChartX.AddSerie(1);
		m_pLineSerieX->SetSeriesOrdering(CChartSerie::soXOrdering);
		
	}
	if (m_pLineSerieY == NULL)
	{
		m_pLineSerieY = m_DataChartY.AddSerie(1);
		m_pLineSerieY->SetSeriesOrdering(CChartSerie::soXOrdering);
		
	}
	if (model.nPtValid == 0) return;
	double dMaxResX, dMaxResY, dMinResX, dMinResY;
	double dMaxX, dMaxY, dMinX, dMinY;
	dMaxX = dMaxY = dMaxResX = dMaxResY = -1e10;
	dMinX = dMinY = dMinResX = dMinResY = +1e10;
	double *pBX, *pLX, *pBY, *pLY;
	pBX = new double[model.nPtValid];
	pLX = new double[model.nPtValid];
	pBY = new double[model.nPtValid];
	pLY = new double[model.nPtValid];
	for (int i = 0; i < model.nPtValid; i++)
	{
		dMaxY = max(dMaxY, model.vecImgPt1[i].y);
		dMaxX = max(dMaxX, model.vecImgPt1[i].x);
		dMinY = min(dMinY, model.vecImgPt1[i].y);
		dMinX = min(dMinX, model.vecImgPt1[i].x);
		dMaxResY = max(dMaxResY, model.vecPtResxy[i].y);
		dMaxResX = max(dMaxResX, model.vecPtResxy[i].x);
		dMinResY = min(dMinResY, model.vecPtResxy[i].y);
		dMinResX = min(dMinResX, model.vecPtResxy[i].x);
		*(pBX + i) = model.vecImgPt1[i].x;
		*(pLX + i) = model.vecPtResxy[i].x;
		*(pBY + i) = model.vecImgPt1[i].y;
		*(pLY + i) = model.vecPtResxy[i].y;
	}
	clock_t t2 = clock();
	m_pAxisLX->SetMinMax(dMinResX - fabs(dMinResX*0.2), dMaxResX + fabs(dMaxResX*0.2));
	m_pAxisBX->SetMinMax(dMinX - fabs(dMinX*0.2), dMaxX + fabs(dMaxX*0.2));
	m_pAxisLY->SetMinMax(dMinResY - fabs(dMinResY*0.2), dMaxResY + fabs(dMaxResY*0.2));
	m_pAxisBY->SetMinMax(dMinY - fabs(dMinY*0.2), dMaxY + fabs(dMaxY*0.2));
	clock_t t3 = clock();
	if (model.nPtValid < 5000) {

		m_pLineSerieX->SetPoints(pBX, pLX, model.nPtValid);
		m_pLineSerieY->SetPoints(pBY, pLY, model.nPtValid);
	}
	else
	{
		double *pBX2 = new double[5000];
		double *pLX2 = new double[5000];
		double *pBY2 = new double[5000];
		double *pLY2 = new double[5000];
		srand((unsigned)time(NULL));
		for (int k = 0; k<5000; k++)
		{		
			int idx = (rand() % model.nPtValid);
			*(pBX2 + k) = *(pBX + idx);
			*(pLX2 + k) = *(pLX + idx);
			*(pBY2 + k) = *(pBY + idx);
			*(pLY2 + k) = *(pLY + idx);

		}
		m_pLineSerieX->SetPoints(pBX2, pLX2, 5000);
		m_pLineSerieY->SetPoints(pBY2, pLY2, 5000);
		delete[]pBX2;
		delete[]pLX2;
		delete[]pBY2;
		delete[]pLY2;
	}
	clock_t t4 = clock();
	m_DataChartX.EnableRefresh(true);
	m_DataChartY.EnableRefresh(true);
	clock_t t5 = clock();
//	CString str; str.Format("data-%d  set-%d  add-%d  fresh-%d", t2 - t1, t3 - t2, t4 - t3, t5 - t4);
//	MessageBox(str);
	m_strEditInfo.Format("point:%d time(ms): load-%d, set-%d, add-%d, refresh-%d", model.nPtValid, t2 - t1, t3 - t2, t4 - t3, t5 - t4);
	UpdateData(FALSE);
	delete[]pBX;
	delete[]pLX;
	delete[]pBY;
	delete[]pLY;
}

void CMyMatchPointChartDlg::CreateChart()
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_DataChartX.Create(this, rectDummy, IDC_DATA_CHART));
	m_DataChartX.ShowWindow(true);
	m_pAxisLX = m_DataChartX.GetLeftAxis();
	m_pAxisLX->SetMinMax(0, 0);
	m_pAxisLX->SetAutomatic(true);
	m_pAxisLX->GetLabel()->SetText("residual_err(pix)");
	m_pAxisBX = m_DataChartX.GetBottomAxis();
	m_pAxisBX->SetMinMax(0, 0);
	m_pAxisBX->SetAutomatic(true);
	m_pAxisBX->GetLabel()->SetText("direction_x");
	m_pLineSerieX = m_DataChartX.AddSerie(1);
	m_pLineSerieX->SetSeriesOrdering(CChartSerie::soXOrdering);  //设置为无序

	if (!m_DataChartY.Create(this, rectDummy, IDC_DATA_CHART));
	m_DataChartY.ShowWindow(true);
	m_pAxisLY = m_DataChartY.GetLeftAxis();
	m_pAxisLY->SetMinMax(0, 0);
	m_pAxisLY->SetAutomatic(true);
	m_pAxisLY->GetLabel()->SetText("residual_err(pix)");
	m_pAxisBY = m_DataChartY.GetBottomAxis();
	m_pAxisBY->SetMinMax(0, 0);
	m_pAxisBY->SetAutomatic(true);
	m_pAxisBY->GetLabel()->SetText("direction_y");
	m_pLineSerieY = m_DataChartY.AddSerie(1);
	m_pLineSerieY->SetSeriesOrdering(CChartSerie::soXOrdering);  //设置为无序
}

void CMyMatchPointChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INFO, m_strEditInfo);
	DDX_Control(pDX, IDC_EDIT_INFO, m_pEditInfo);
}


BEGIN_MESSAGE_MAP(CMyMatchPointChartDlg, CDialogEx)
	//	ON_WM_NCCREATE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(ID_MYMSG_FILLCHART,OnFillChart)
END_MESSAGE_MAP()


// CMyMatchPointChartDlg message handlers


LRESULT CMyMatchPointChartDlg::OnFillChart(WPARAM wParam, LPARAM lParam)
{
	MstuMchModel model = *(MstuMchModel*)wParam;
	FillChart(model);
	CString strLabel;
	if (model.eType == MODEL_TIE)
	{
		strLabel = FunGetFileName(FunGetDocHand()->m_DomChkPrj.GetDomInfo(model.nImgIdx1).GetImgPath()) + "=="
			+ FunGetFileName(FunGetDocHand()->m_DomChkPrj.GetDomInfo(model.nImgIdx2).GetImgPath());
	}
	else
	{
		strLabel = FunGetFileName(FunGetDocHand()->m_DomChkPrj.GetDomInfo(model.nImgIdx1).GetImgPath());
	}
	SetWindowText(strLabel);
	return 1;
}

int CMyMatchPointChartDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CreateChart();
	return 0;
}


void CMyMatchPointChartDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	CWnd *pTempEdit = GetDlgItem(IDC_EDIT_INFO);
	if(pTempEdit) pTempEdit->SetWindowPos(NULL, 0, cy - 30, cx, 30, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_pEditInfo.SetWindowPos(NULL, 0, cy - 30, cx, 30, SWP_NOACTIVATE | SWP_NOZORDER);
	m_DataChartX.SetWindowPos(NULL, 0, 0, cx / 2 - 5, cy-35, SWP_NOACTIVATE | SWP_NOZORDER);

	m_DataChartY.SetWindowPos(NULL, cx / 2 + 5, 0, cx / 2 - 5, cy-35, SWP_NOACTIVATE | SWP_NOZORDER);
	// TODO: Add your message handler code here
}



BOOL CMyMatchPointChartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	int width = min(1000, cx);
	int height = min(400, width * 400 / 1000);
	MoveWindow(CRect(0, 0, width, height));
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
