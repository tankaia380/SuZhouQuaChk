// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MDomQuaChkSysView.cpp : CMDomQuaChkSysView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MDomQuaChkSys.h"
#endif
#include "MainFrm.h"
#include "MDomQuaChkSysDoc.h"
#include "MDomQuaChkSysView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern HGLRC g_pRC;
extern HGLRC g_pRCSharing;
// CMDomQuaChkSysView

IMPLEMENT_DYNCREATE(CMDomQuaChkSysView, CView)

BEGIN_MESSAGE_MAP(CMDomQuaChkSysView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMDomQuaChkSysView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(ID_MYMSG_INIT_IMGRENDER, &CMDomQuaChkSysView::OnInitImgRender)
	ON_MESSAGE(ID_MYMSG_SELECTEDRENDER, &CMDomQuaChkSysView::OnSelectedImgRender)
	ON_MESSAGE(ID_MYMSG_MODELPOINTRENDER, &CMDomQuaChkSysView::OnCreateModelPointRender)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_COMMAND(ID_BUTTON_REPORT_FILL, &CMDomQuaChkSysView::OnButtonReportFill)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_REPORT_FILL, &CMDomQuaChkSysView::OnUpdateButtonReportFill)
	ON_MESSAGE(ID_MYMSG_FILLREPORTVIEW_TIE, OnFillReportViewTie)
	ON_MESSAGE(ID_MYMSG_FILLREPORTVIEW_GCP, OnFillReportViewGcp)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIEW_MATCHPOINT, &CMDomQuaChkSysView::OnUpdateCheckViewMatchpoint)
	ON_COMMAND(ID_CHECK_VIEW_RESIDUALCHART, &CMDomQuaChkSysView::OnCheckViewResidualchart)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIEW_RESIDUALCHART, &CMDomQuaChkSysView::OnUpdateCheckViewResidualchart)
	ON_COMMAND(ID_CHECK_VIEW_MATCHPOINT, &CMDomQuaChkSysView::OnCheckViewMatchpoint)
	ON_COMMAND(ID_CHECK5_VIEW_SELECT_POLYGON, &CMDomQuaChkSysView::OnCheck5ViewSelectPolygon)
	ON_UPDATE_COMMAND_UI(ID_CHECK5_VIEW_SELECT_POLYGON, &CMDomQuaChkSysView::OnUpdateCheck5ViewSelectPolygon)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_COLOR1, &CMDomQuaChkSysView::OnUpdateButtonColor1)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_COLOR2, &CMDomQuaChkSysView::OnUpdateButtonColor2)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_COLOR3, &CMDomQuaChkSysView::OnUpdateButtonColor3)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_COLOR4, &CMDomQuaChkSysView::OnUpdateButtonColor4)
	ON_COMMAND(ID_BUTTON_COLOR1, &CMDomQuaChkSysView::OnButtonColor1)
	ON_COMMAND(ID_BUTTON_COLOR2, &CMDomQuaChkSysView::OnButtonColor2)
	ON_COMMAND(ID_BUTTON_COLOR3, &CMDomQuaChkSysView::OnButtonColor3)
	ON_COMMAND(ID_BUTTON_COLOR4, &CMDomQuaChkSysView::OnButtonColor4)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CMDomQuaChkSysView 构造/析构

CMDomQuaChkSysView::CMDomQuaChkSysView()
{
	// TODO: 在此处添加构造代码
	m_pDC = NULL;
	m_bViewDom = true;
	m_bViewRef = true;
	m_bViewMatchPoint = true;;
	m_bViewFill = false;
	m_bViewChart = false;
	m_bSaveView2Image = false;
	m_bViewSelectPolygon = true;
	m_bAddlViewImgLabel = false;
}

CMDomQuaChkSysView::~CMDomQuaChkSysView()
{
}

BOOL CMDomQuaChkSysView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMDomQuaChkSysView 绘制

Point2D CMDomQuaChkSysView::ptCln2Geo(CPoint& ptCln)
{
	Point2D ptGeo;
	ptGeo.x = m_ptClnCenter2Geo.x + 1.0*(ptCln.x - m_RectCln.CenterPoint().x) / m_fZoomRateCln2Geo;
	ptGeo.y = m_ptClnCenter2Geo.y - 1.0*(ptCln.y - m_RectCln.CenterPoint().y) / m_fZoomRateCln2Geo;
	return ptGeo;
}
CPoint CMDomQuaChkSysView::ptGeo2Cln(Point2D& ptGeo)
{
	CPoint ptCln;
	ptCln.x = (ptGeo.x - m_ptClnCenter2Geo.x) * m_fZoomRateCln2Geo + m_RectCln.CenterPoint().x;
	ptCln.y = (m_ptClnCenter2Geo.y - ptGeo.y) * m_fZoomRateCln2Geo + m_RectCln.CenterPoint().y;
	return ptCln;
}
Point2D CMDomQuaChkSysView::ClnCenter2Geo(CPoint ptCln, Point2D ptGeo)
{
	Point2D ptClnCenter2Geo;
	ptClnCenter2Geo.x = ptGeo.x - 1.0*(ptCln.x - m_RectCln.CenterPoint().x) / m_fZoomRateCln2Geo;
	ptClnCenter2Geo.y = ptGeo.y + 1.0*(ptCln.y - m_RectCln.CenterPoint().y) / m_fZoomRateCln2Geo;
	return ptClnCenter2Geo;
}
void CMDomQuaChkSysView::ResetRectCln2Geo()
{
	m_RectCln2Geo = CRectD(ptCln2Geo(m_RectCln.TopLeft()), ptCln2Geo(m_RectCln.BottomRight()));
}

void CMDomQuaChkSysView::Ready4SaveView2Img(CString strSavePath, bool bAddLabel)
{
	m_bSaveView2Image = true;
	m_strViewImgSavePath = strSavePath;
	m_bAddlViewImgLabel = bAddLabel;
	m_ptClnCenter2Geo = m_RectCqViewGeo.CenterPoint();
	m_fZoomRateCln2Geo = min(m_RectCln.Width()*1.0 / m_RectCqViewGeo.Width(),
		m_RectCln.Height()*1.0 / m_RectCqViewGeo.Height())*0.9;
	m_bViewMatchPoint = false;
	m_bViewSelectPolygon = false;
}

void CMDomQuaChkSysView::SaveView2Image(CRect RectSaveView, CString strSavePath, bool bAddLabel /* = false */)
{
	int l, t, w, h;
	l = max(RectSaveView.left - 10, 0);
	t = max(m_RectCln.Height() - RectSaveView.bottom - 10, 0);   //opengl窗口左下角为（0，0）
	w = min(RectSaveView.Width() + 20, m_RectCln.Width() - l);
	h = min(RectSaveView.Height() + 20, m_RectCln.Height() - t);

	BYTE* pMem = new BYTE[w*h * 3]; memset(pMem, 0, w*h * 3);
	BYTE* pData = new BYTE[w*h * 3]; memset(pData, 0, w*h * 3);
	glReadBuffer(GL_FRONT);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(l, t, w, h, GL_RGB, GL_UNSIGNED_BYTE, pMem); //l,t相对窗口左下角
#pragma omp parallel for
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			for (int b = 0; b < 3; b++)
			{
				*(pData + (h - i - 1)*w * 3 + j * 3 + b) = *(pMem + i*w * 3 + j * 3 + b);
			}
		}
	}
	CMImage4Check img;
	if (!bAddLabel)
	{
		img.SaveImg(strSavePath, w, h, 3, pData);
	}
	else
	{
		int hLabel = 15, wLabel = w*2/3 / 4;
		COLORREF color1 = FunGetMainFrameHand()->GetFillColor1();
		COLORREF color2 = FunGetMainFrameHand()->GetFillColor2();
		COLORREF color3 = FunGetMainFrameHand()->GetFillColor3();
		COLORREF color4 = FunGetMainFrameHand()->GetFillColor4();
		BYTE *pData2 = new BYTE[w*(h + hLabel) * 3]; memset(pData2, 255, w*(h + hLabel) * 3);
		for (int i = 0; i<h*w*3; i++)
		{
			*(pData2+i) = *(pData+i);
		}
		for (int i = h; i<h+hLabel; i++)
		{
			for (int j = 0; j<wLabel; j++)
			{
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 0) = GetRValue(color1);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 1) = GetGValue(color1);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 2) = GetBValue(color1);
			}
			for (int j = wLabel; j <2* wLabel; j++)
			{
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 0) = GetRValue(color2);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 1) = GetGValue(color2);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 2) = GetBValue(color2);
			}
			for (int j = 2*wLabel; j < 3*wLabel; j++)
			{
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 0) = GetRValue(color3);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 1) = GetGValue(color3);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 2) = GetBValue(color3);
			}
			for (int j = 3*wLabel; j < 4*wLabel; j++)
			{
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 0) = GetRValue(color4);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 1) = GetGValue(color4);
				*(pData2 + i*w * 3 + (j + w / 3) * 3 + 2) = GetBValue(color4);
			}
		}
		img.SaveImg(strSavePath, w, h+hLabel, 3, pData2);
		delete[]pData2;
	}

	if (pMem) delete[] pMem;
	if (pData) delete[] pData;
	pMem = NULL;
	pData = NULL;
}

void CMDomQuaChkSysView::RefreshFillColor(int nLevel)
{
	COLORREF color = m_pColorFillLevel[nLevel - 1];
	for (int i = 0; i < m_vecRenderFillPolygonTie.size(); i++)
	{
		if (m_vecLevelIdPolygonTie[i] != nLevel)continue;
		int nVertexNum = m_vecRenderFillPolygonTie[i].GetVertexNum();
		double *pC = new double[nVertexNum * 4];
		FunCreateVertexColor(nVertexNum, GetRValue(color)*1.0 / 255, GetGValue(color)*1.0 / 255, GetBValue(color)*1.0 / 255, pC);
		m_vecRenderFillPolygonTie[i].UpdateVectorVBO(nVertexNum * 2 /** sizeof(double)*/, nVertexNum * 4, pC);
		delete[]pC;
	}

	for (int i = 0; i < m_vecRenderFillPolygonGcp.size(); i++)
	{
		if (m_vecLevelIdPolygonGcp[i] != nLevel)continue;
		int nVertexNum = m_vecRenderFillPolygonGcp[i].GetVertexNum();
		double *pC = new double[nVertexNum * 4];
		FunCreateVertexColor(nVertexNum, GetRValue(color)*1.0 / 255, GetGValue(color)*1.0 / 255, GetBValue(color)*1.0 / 255, pC);
		m_vecRenderFillPolygonGcp[i].UpdateVectorVBO(nVertexNum * 2 /** sizeof(double)*/, nVertexNum * 4, pC);
		delete[]pC;
	}
}

void CMDomQuaChkSysView::RefreshPointColor()
{
	MstuMchModel model = FunGetMainFrameHand()->GetMchModel();
	double *pC = new double[4 * model.nPtValid];
	for (int i = 0; i < model.nPtValid; i++)
	{
		COLORREF color;
		if (fabs(model.vecPtResxy[i].x) >= 3 || fabs(model.vecPtResxy[i].y) >= 3)
		{
			color = FunGetMainFrameHand()->GetFillColor4();
		}
		else if (fabs(model.vecPtResxy[i].x) >= 2 || fabs(model.vecPtResxy[i].y) >= 2)
		{
			color = FunGetMainFrameHand()->GetFillColor3();
		}
		else if (fabs(model.vecPtResxy[i].x) >= 1 && fabs(model.vecPtResxy[i].y) >= 1)
		{
			color = FunGetMainFrameHand()->GetFillColor2();
		}
		else //1个像元rgb   0 0.7 0
		{
			color = FunGetMainFrameHand()->GetFillColor1();
		}
		double r = GetRValue(color)*1.0 / 255;
		double g = GetGValue(color)*1.0 / 255;
		double b = GetBValue(color)*1.0 / 255;

		pC[i * 4 + 0] = GetRValue(color)*1.0 / 255;
		pC[i * 4 + 1] = GetGValue(color)*1.0 / 255;
		pC[i * 4 + 2] = GetBValue(color)*1.0 / 255;
		pC[i * 4 + 3] = 255;
	}
	m_RenderModelPoint.UpdateVectorVBO(m_RenderModelPoint.GetVertexNum() * 2 /** sizeof(double)*/, m_RenderModelPoint.GetVertexNum() * 4, pC);
	delete[]pC;

}

void CMDomQuaChkSysView::OnDraw(CDC* /*pDC*/)
{
	CMDomQuaChkSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: 在此处为本机数据添加绘制代码
}

// CMDomQuaChkSysView 打印

LRESULT CMDomQuaChkSysView::OnFillReportViewTie(WPARAM wParam, LPARAM lParam)
{
	vector<MstuTieChkModel> vecTieChkModel = *(vector<MstuTieChkModel>*)wParam;
	vector<vector<Point2D>> vvTieModelArea = *(vector<vector<Point2D>>*)lParam;
	for (int i = 0; i < vecTieChkModel.size() - 1; i++)
	{
		for (int j = i + 1; j < vecTieChkModel.size(); j++)
		{
			if (vecTieChkModel[i].dMeanDxy > vecTieChkModel[j].dMeanDxy)
			{
				MstuTieChkModel tmp1 = vecTieChkModel[i];
				vecTieChkModel[i] = vecTieChkModel[j];
				vecTieChkModel[j] = tmp1;
				vector<Point2D> tmp2 = vvTieModelArea[i];
				vvTieModelArea[i] = vvTieModelArea[j];
				vvTieModelArea[j] = tmp2;
			}
		}
	}
	bool bMakeCurrent = false;
	do { bMakeCurrent = wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRCSharing); } while (!bMakeCurrent);
	for (int i = 0; i < m_vecRenderFillPolygonTie.size(); i++)
	{
		m_vecRenderFillPolygonTie[i].DeleteVBO();
	}
	vector<CMyViewerRender>().swap(m_vecRenderFillPolygonTie);
	vector<int>().swap(m_vecLevelIdPolygonTie);
	for (int i = 0; i < vecTieChkModel.size(); i++)
	{
		if (vecTieChkModel[i].nPtNum == 0) continue;
		COLORREF color;
		if (vecTieChkModel[i].nPtNum == 0)
		{
			color = FunGetMainFrameHand()->GetFillColor4();
			m_vecLevelIdPolygonTie.push_back(4);
		}
		else if (vecTieChkModel[i].dMeanDxy < 1)
		{
			//127 255 0
			color = FunGetMainFrameHand()->GetFillColor1();
			m_vecLevelIdPolygonTie.push_back(1);
		}
		else if (vecTieChkModel[i].dMeanDxy < 2)
		{
			//	184 134 11
			color = FunGetMainFrameHand()->GetFillColor2();
			m_vecLevelIdPolygonTie.push_back(2);
		}
		else if (vecTieChkModel[i].dMeanDxy < 3)
		{
			//255 69 0
			color = FunGetMainFrameHand()->GetFillColor3();
			m_vecLevelIdPolygonTie.push_back(3);
		}
		else
		{
			//139 0 0
			color = FunGetMainFrameHand()->GetFillColor4();
			m_vecLevelIdPolygonTie.push_back(4);
		}
		double r = GetRValue(color)*1.0 / 255;
		double g = GetGValue(color)*1.0 / 255;
		double b = GetBValue(color)*1.0 / 255;
		double *pV = new double[vvTieModelArea[i].size() * 2];
		double *pC = new double[vvTieModelArea[i].size() * 4];
		FunVectorPt2VertexXY(&vvTieModelArea[i], 2, pV);
		FunCreateVertexColor(vvTieModelArea[i].size(), r, g, b, pC);
		CMyViewerRender render;
		render.CreateVectorVBO(vvTieModelArea[i].size(), vPane);
		render.FillVectorVBO(pV, pC, vvTieModelArea[i].size());
		m_vecRenderFillPolygonTie.push_back(render);
		delete[]pC;
		delete[]pV;
	}
	wglMakeCurrent(NULL, NULL);
	vector<MstuTieChkModel>().swap(vecTieChkModel);
	vector<vector<Point2D>>().swap(vvTieModelArea);
	return 1;
}

LRESULT CMDomQuaChkSysView::OnFillReportViewGcp(WPARAM wParam, LPARAM lParam)
{
	vector<MstuGcpChkModel> vecGcpChkModel = *(vector<MstuGcpChkModel>*)wParam;
	vector<vector<Point2D>> vvGcpModelArea = *(vector<vector<Point2D>>*)lParam;
	for (int i = 0; i < vecGcpChkModel.size() - 1; i++)
	{
		for (int j = i + 1; j < vecGcpChkModel.size(); j++)
		{
			if (vecGcpChkModel[i].dMeanDxy > vecGcpChkModel[j].dMeanDxy)
			{
				MstuGcpChkModel tmp1 = vecGcpChkModel[i];
				vecGcpChkModel[i] = vecGcpChkModel[j];
				vecGcpChkModel[j] = tmp1;
				vector<Point2D> tmp2 = vvGcpModelArea[i];
				vvGcpModelArea[i] = vvGcpModelArea[j];
				vvGcpModelArea[j] = tmp2;
			}
		}
	}
	bool bMakeCurrent = false;
	do { bMakeCurrent = wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRCSharing); } while (!bMakeCurrent);
	for (int i = 0; i < m_vecRenderFillPolygonGcp.size(); i++)
	{
		m_vecRenderFillPolygonGcp[i].DeleteVBO();
	}
	vector<CMyViewerRender>().swap(m_vecRenderFillPolygonGcp);
	vector<int>().swap(m_vecLevelIdPolygonGcp);
	for (int i = 0; i < vecGcpChkModel.size(); i++)
	{
		if (vecGcpChkModel[i].nPtNum == 0) continue;
		COLORREF color;
		if (vecGcpChkModel[i].nPtNum == 0)
		{
			color = FunGetMainFrameHand()->GetFillColor4();
			m_vecLevelIdPolygonGcp.push_back(4);
		}
		else if (vecGcpChkModel[i].dMeanDxy < 1)
		{
			//127 255 0
			color = FunGetMainFrameHand()->GetFillColor1();
			m_vecLevelIdPolygonGcp.push_back(1);
		}
		else if (vecGcpChkModel[i].dMeanDxy < 2)
		{
			//	184 134 11
			color = FunGetMainFrameHand()->GetFillColor2();
			m_vecLevelIdPolygonGcp.push_back(2);
		}
		else if (vecGcpChkModel[i].dMeanDxy < 3)
		{
			//255 69 0
			color = FunGetMainFrameHand()->GetFillColor3();
			m_vecLevelIdPolygonGcp.push_back(3);
		}
		else
		{
			//139 0 0
			color = FunGetMainFrameHand()->GetFillColor4();
			m_vecLevelIdPolygonGcp.push_back(4);
		}
		double r = GetRValue(color)*1.0 / 255;
		double g = GetGValue(color)*1.0 / 255;
		double b = GetBValue(color)*1.0 / 255;
		double *pV = new double[vvGcpModelArea[i].size() * 2];
		double *pC = new double[vvGcpModelArea[i].size() * 4];
		FunVectorPt2VertexXY(&vvGcpModelArea[i], 2, pV);
		FunCreateVertexColor(vvGcpModelArea[i].size(), r, g, b, pC);
		CMyViewerRender render;
		render.CreateVectorVBO(vvGcpModelArea[i].size(), vPane);
		render.FillVectorVBO(pV, pC, vvGcpModelArea[i].size());
		m_vecRenderFillPolygonGcp.push_back(render);
		delete[]pC;
		delete[]pV;
	}
	wglMakeCurrent(NULL, NULL);
	vector<MstuGcpChkModel>().swap(vecGcpChkModel);
	vector<vector<Point2D>>().swap(vvGcpModelArea);
	return 1;
}

LRESULT CMDomQuaChkSysView::OnInitImgRender(WPARAM wParam, LPARAM lParam)
{
	DeleteAreaRender();
	CMDomQuaChkSysDoc* pDoc = GetDocument();
	double l = 1e10, t = -1e10, r = -1e10, b = 1e10;
	int nDomNum = pDoc->m_DomChkPrj.GetDomNum();
	int nRefNum = pDoc->m_DomChkPrj.GetRefNum();
	bool bMakeCurrent = false;
	do { bMakeCurrent = wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRCSharing); } while (!bMakeCurrent);
	double *pV = new double[4 * 2];
	double *pC = new double[4 * 4];
	for (int i = 0; i < nDomNum; i++)
	{
		vector<Point2D> pTfwPt4 = pDoc->m_DomChkPrj.GetDomInfo(i).GetGeoInfo().vecValidVertex;
		for (int j = 0; j < 4; j++)
		{
			l = min(l, pTfwPt4[j].x);
			t = max(t, pTfwPt4[j].y);
			r = max(r, pTfwPt4[j].x);
			b = min(b, pTfwPt4[j].y);
		}
		FunVectorPt2VertexXY(&pTfwPt4, 2, pV);
		FunCreateVertexColor(pTfwPt4.size(), 0, 0, 1, pC);
		CMyViewerRender render;

		render.CreateVectorVBO(4, vLine, true);
		render.FillVectorVBO(pV, pC, 4);
		m_vecRenderDomArea.push_back(render);
		vector<Point2D>().swap(pTfwPt4);
	}
	for (int i = 0; i < nRefNum; i++)
	{
		vector<Point2D> pTfwPt4 = pDoc->m_DomChkPrj.GetRefInfo(i).GetGeoInfo().vecValidVertex;
		for (int j = 0; j < 4; j++)
		{
			l = min(l, pTfwPt4[j].x);
			t = max(t, pTfwPt4[j].y);
			r = max(r, pTfwPt4[j].x);
			b = min(b, pTfwPt4[j].y);
		}
		//	pTfwPt4.push_back(pTfwPt4[0]);
		FunVectorPt2VertexXY(&pTfwPt4, 2, pV);//255 239 213
		FunCreateVertexColor(pTfwPt4.size(), 255 / 255, 239.0 / 255, 213.0 / 255, pC);
		CMyViewerRender render;
		render.CreateVectorVBO(4, vPane, true);
		render.FillVectorVBO(pV, pC, 4);
		m_vecRenderRefArea.push_back(render);
		vector<Point2D>().swap(pTfwPt4);
	}
	wglMakeCurrent(NULL, NULL);
	if (pC)delete[]pC;
	if (pC)delete[]pV;
	m_RectCqViewGeo = CRectD(l, t, r, b);
	m_ptClnCenter2Geo = m_RectCqViewGeo.CenterPoint();
	m_fZoomRateCln2Geo = min(m_RectCln.Width()*1.0 / m_RectCqViewGeo.Width(),
		m_RectCln.Height()*1.0 / m_RectCqViewGeo.Height())*0.9;
	m_fZoomRateCln2GeoMin = m_fZoomRateCln2Geo / 20;
	m_fZoomRateCln2GeoMax = m_fZoomRateCln2Geo * 20;
	Invalidate();
	return 1;
}

LRESULT CMDomQuaChkSysView::OnSelectedImgRender(WPARAM wParam, LPARAM lParam)
{
	DeleteSelectRender();
	vector<int> vDomIdx = *(vector<int>*)wParam;
	if (vDomIdx.size() == 0) return -1;
	MenmModelType eModelType = *(MenmModelType*)lParam;
	double *pV = new double[4 * 2];
	double *pC = new double[4 * 4];
	bool bMakeCurrent = false;
	do { bMakeCurrent = wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRCSharing); } while (!bMakeCurrent);
	CMDomQuaChkSysDoc* pDoc = GetDocument();
	for (int i = 0; i < vDomIdx.size(); i++)
	{
		if (vDomIdx[i] < 0) continue;
		vector<Point2D> pTfwPt4 = pDoc->m_DomChkPrj.GetDomInfo(vDomIdx[i]).GetGeoInfo().vecValidVertex;
		FunVectorPt2VertexXY(&pTfwPt4, 2, pV);
		//240 255 240
		FunCreateVertexColor(pTfwPt4.size(), 0.5, 0, 0, pC);
		CMyViewerRender render;
		render.CreateVectorVBO(4, vLine, true);
		render.FillVectorVBO(pV, pC, 4);
		m_vecRenderSelected.push_back(render);
		vector<Point2D>().swap(pTfwPt4);
		if (eModelType == MODEL_CTL) break;
	}
	wglMakeCurrent(NULL, NULL);
	if (pC)delete[]pC;
	if (pC)delete[]pV;
	Invalidate();
	return 1;
}

LRESULT CMDomQuaChkSysView::OnCreateModelPointRender(WPARAM wParam, LPARAM lParam)
{
	MstuMchModel model = *(MstuMchModel*)wParam;
	m_RenderModelPoint.DeleteVBO();
	bool bMakeCurrent = false;
	do { bMakeCurrent = wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRCSharing); } while (!bMakeCurrent);
	m_RenderModelPoint.CreateVectorVBO(model.nPtValid, vPoint);
	double *pV = new double[2 * model.nPtValid];
	double *pC = new double[4 * model.nPtValid];
	FunVectorPt2VertexXY(&model.vecObjPtXY, 2, pV);
	for (int i = 0; i < model.nPtValid; i++)
	{
		COLORREF color;
		if (fabs(model.vecPtResxy[i].x) >= 3 || fabs(model.vecPtResxy[i].y) >= 3)
		{
			color = FunGetMainFrameHand()->GetFillColor4();
		}
		else if (fabs(model.vecPtResxy[i].x) >= 2 || fabs(model.vecPtResxy[i].y) >= 2)
		{
			color = FunGetMainFrameHand()->GetFillColor3();
		}
		else if (fabs(model.vecPtResxy[i].x) >= 1 && fabs(model.vecPtResxy[i].y) >= 1)
		{
			color = FunGetMainFrameHand()->GetFillColor2();
		}
		else //1个像元rgb   0 0.7 0
		{
			color = FunGetMainFrameHand()->GetFillColor1();
		}
		double r = GetRValue(color)*1.0 / 255;
		double g = GetGValue(color)*1.0 / 255;
		double b = GetBValue(color)*1.0 / 255;

		pC[i * 4 + 0] = GetRValue(color)*1.0 / 255;
		pC[i * 4 + 1] = GetGValue(color)*1.0 / 255;
		pC[i * 4 + 2] = GetBValue(color)*1.0 / 255;
		pC[i * 4 + 3] = 255;
	}
	m_RenderModelPoint.FillVectorVBO(pV, pC, model.nPtValid);
	wglMakeCurrent(NULL, NULL);
	model.Clear();
	delete[]pV;
	delete[]pC;
	Invalidate();
	return 1;
}

void CMDomQuaChkSysView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMDomQuaChkSysView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMDomQuaChkSysView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMDomQuaChkSysView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CMDomQuaChkSysView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMDomQuaChkSysView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CMDomQuaChkSysView 诊断

#ifdef _DEBUG
void CMDomQuaChkSysView::AssertValid() const
{
	CView::AssertValid();
}

void CMDomQuaChkSysView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMDomQuaChkSysDoc* CMDomQuaChkSysView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMDomQuaChkSysDoc)));
	return (CMDomQuaChkSysDoc*)m_pDocument;
}
#endif //_DEBUG

// CMDomQuaChkSysView 消息处理程序

int CMDomQuaChkSysView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pDC = new CClientDC(this);//初始化OPENGL设置定时器
	SetupPixelFormat(m_pDC, m_hPalette);
	InitOpenGL(g_pRCSharing, m_pDC, m_hPalette);
	InitOpenGL(g_pRC, m_pDC, m_hPalette);
	wglShareLists(g_pRCSharing, g_pRC);
	wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRC);
	glewInit();

	bool g_bSupportVBO = isGLExtensionSupported("GL_ARB_vertex_buffer_object");
	if (g_bSupportVBO)
	{
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	}
	m_BkgrdRender.CreateVectorVBO(4, vPane);
	return 0;
}

BOOL CMDomQuaChkSysView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CMDomQuaChkSysDoc* pDoc = GetDocument();
	if (pDoc == NULL) return CView::OnEraseBkgnd(pDC);
	if (!pDoc->m_bDataReady)return CView::OnEraseBkgnd(pDC);
	bool bMakeCurrent = false;
	do { bMakeCurrent = wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRC); } while (!bMakeCurrent);
	PrepareDraw(0, 0, 0, 1);
	glViewport(0, 0, m_RectCln.Width(), m_RectCln.Height());
	glLoadIdentity();    // 重置当前指定的矩阵为单位矩阵
	ResetRectCln2Geo();
	glOrtho(m_RectCln2Geo.left, m_RectCln2Geo.right, m_RectCln2Geo.bottom, m_RectCln2Geo.top, -1, 1);
	//	glOrtho(m_RectCln.left, m_RectCln.right, m_RectCln.bottom, m_RectCln.top, -1, 1);
	float A[4] = { 0.5,0.5,0.5,0.5 };
	float R[4] = { 155.0 / 255,230.0 / 255,230.0 / 255,155.0 / 255 };
	SetBackColor(m_BkgrdRender, m_RectCln2Geo, R, R, R, A);
	for (int i = 0; i < m_vecRenderRefArea.size(); i++)
	{
		m_vecRenderRefArea[i].Display(3);
	}
	if (m_bViewFill)
	{
		if (pDoc->m_eModelType == MODEL_TIE)
		{
			for (int i = 0; i < m_vecRenderFillPolygonTie.size(); i++)
			{
				m_vecRenderFillPolygonTie[i].Display(1);
			}
		}
		else if (pDoc->m_eModelType == MODEL_CTL)
		{
			for (int i = 0; i < m_vecRenderFillPolygonGcp.size(); i++)
			{
				m_vecRenderFillPolygonGcp[i].Display(1);
			}
		}
	}
	if (m_bViewMatchPoint) m_RenderModelPoint.Display(4);
	for (int i = 0; i < m_vecRenderDomArea.size(); i++)
	{
		m_vecRenderDomArea[i].Display(2);
	}
	if (m_bViewSelectPolygon)
	{
		for (int i = 0; i < m_vecRenderSelected.size(); i++)
		{
			m_vecRenderSelected[i].Display(3);
		}
	}

	if (pDoc->m_bIsIdentify&&m_vecIdentifyPoint.IsFilled())   //矢量图中点击位置显示
	{
		m_vecIdentifyPoint.Display(8);
	}

	EndDraw(m_pDC);
	if (m_bSaveView2Image)
	{
		CRect rect = CRect(ptGeo2Cln(m_RectCqViewGeo.LeftTop()), ptGeo2Cln(m_RectCqViewGeo.RightBottom()));
		if (rect.Width() < 1.4*rect.Height())
		{
			rect.left = rect.CenterPoint().x - rect.Height()*0.7;
			rect.right = rect.CenterPoint().x + rect.Height()*0.7;
		}
		SaveView2Image(rect, m_strViewImgSavePath, m_bAddlViewImgLabel);
		m_bSaveView2Image = false;
	}
	return true;
}

void CMDomQuaChkSysView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	GetClientRect(m_RectCln);
	// TODO: Add your message handler code here
}

BOOL CMDomQuaChkSysView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (GetDocument() == NULL) return CView::OnMouseWheel(nFlags, zDelta, pt);
	if (!GetDocument()->m_bDataReady) return CView::OnMouseWheel(nFlags, zDelta, pt);
	ScreenToClient(&pt);
	Point2D ptGeo = ptCln2Geo(pt);
	m_fZoomRateCln2Geo *= pow(1.15, 1.0*zDelta / 120);
	// 	m_fZoomRateCln2Geo = min(m_fZoomRateCln2Geo, m_fZoomRateCln2GeoMax);
	// 	m_fZoomRateCln2Geo = max(m_fZoomRateCln2Geo, m_fZoomRateCln2GeoMin);
	m_ptClnCenter2Geo = ClnCenter2Geo(pt, ptGeo);
	//ResetRectCln2Geo();
	Invalidate();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMDomQuaChkSysView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMDomQuaChkSysDoc*pDoc = GetDocument();
	if (pDoc == NULL) return;
	if (!pDoc->m_bDataReady) return;
	if ((nFlags&MK_MBUTTON) == MK_MBUTTON)
	{
		m_ptClnCenter2Geo = ClnCenter2Geo(point, m_ptMBtnDnGeo);
		Invalidate();
	}
	Point2D ptGeo = ptCln2Geo(point);
	char strPos[100];
	sprintf(strPos, "Pos: x = %.6lf, y = %.6lf, z = 0.000", ptGeo.x, ptGeo.y);
	pDoc->m_pMain->SendMessage(ID_MYMSG_UPDATESTATUSPOS, (WPARAM)strPos, 0);
	CView::OnMouseMove(nFlags, point);
}

void CMDomQuaChkSysView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMDomQuaChkSysDoc*pDoc = GetDocument();
	if (pDoc == NULL) return;
	if (!pDoc->m_bDataReady) return;
	SetCapture();
	m_ptMBtnDnGeo = ptCln2Geo(point);
	CView::OnMButtonDown(nFlags, point);
}

void CMDomQuaChkSysView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
}

void CMDomQuaChkSysView::OnButtonReportFill()
{
	// TODO: Add your command handler code here
	m_bViewFill = !m_bViewFill;
	// 	bool bViewMatchPt = m_bViewMatchPoint;
	// 	bool bViewSelect = m_bViewSelectPolygon;
	if (m_bViewFill)
	{
		if (GetDocument()->m_eModelType == MODEL_TIE)
		{
			Ready4SaveView2Img(GetDocument()->m_strTieViewImgPath, true);
		}
		else if (GetDocument()->m_eModelType == MODEL_CTL)
		{
			Ready4SaveView2Img(GetDocument()->m_strGcpViewImgPath, true);
		}
		else
		{
			return;
		}
	}
	// 	m_bViewMatchPoint = bViewMatchPt;
	// 	m_bViewSelectPolygon = bViewSelect;
	Invalidate();
}

void CMDomQuaChkSysView::OnUpdateButtonReportFill(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(FunGetMainFrameHand()->IsModelListFilled());
	pCmdUI->SetCheck(m_bViewFill);
}

void CMDomQuaChkSysView::OnCheckViewMatchpoint()
{
	// TODO: Add your command handler code here
	m_bViewMatchPoint = !m_bViewMatchPoint;
	Invalidate();
}

void CMDomQuaChkSysView::OnUpdateCheckViewMatchpoint(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_RenderModelPoint.IsFilled()) pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_bViewMatchPoint);

}

void CMDomQuaChkSysView::OnCheck5ViewSelectPolygon()
{
	// TODO: Add your command handler code here

	m_bViewSelectPolygon = !m_bViewSelectPolygon;
	Invalidate();
}

void CMDomQuaChkSysView::OnUpdateCheck5ViewSelectPolygon(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(FunGetMainFrameHand()->IsModelListFilled());
	pCmdUI->SetCheck(m_bViewSelectPolygon);
}

void CMDomQuaChkSysView::OnCheckViewResidualchart()
{
	// TODO: Add your command handler code here
	m_bViewChart = !m_bViewChart;
	GetDocument()->m_pChartDlg->ShowWindow(m_bViewChart);

}

void CMDomQuaChkSysView::OnUpdateCheckViewResidualchart(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_RenderModelPoint.IsFilled()) pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_bViewChart);
}

void CMDomQuaChkSysView::OnUpdateButtonColor1(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(FunGetMainFrameHand()->IsModelListFilled());
}

void CMDomQuaChkSysView::OnUpdateButtonColor2(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(FunGetMainFrameHand()->IsModelListFilled());
}

void CMDomQuaChkSysView::OnUpdateButtonColor3(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(FunGetMainFrameHand()->IsModelListFilled());
}

void CMDomQuaChkSysView::OnUpdateButtonColor4(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(FunGetMainFrameHand()->IsModelListFilled());
}

void CMDomQuaChkSysView::OnButtonColor1()
{
	// TODO: Add your command handler code here
	if (FunGetMainFrameHand()->GetFillColor1() != m_pColorFillLevel[0]);
	m_pColorFillLevel[0] = FunGetMainFrameHand()->GetFillColor1();
	RefreshFillColor(1);
	RefreshPointColor();
	if (m_bViewFill)
	{
		if (GetDocument()->m_eModelType == MODEL_TIE)
		{
			Ready4SaveView2Img(GetDocument()->m_strTieViewImgPath, true);
		}
		else
		{
			Ready4SaveView2Img(GetDocument()->m_strGcpViewImgPath, true);
		}
		
	}
	Invalidate();
}

void CMDomQuaChkSysView::OnButtonColor2()
{
	// TODO: Add your command handler code here
	if (FunGetMainFrameHand()->GetFillColor2() != m_pColorFillLevel[1]);
	m_pColorFillLevel[1] = FunGetMainFrameHand()->GetFillColor2();
	RefreshFillColor(2);
	RefreshPointColor();
	if (m_bViewFill)
	{
		if (GetDocument()->m_eModelType == MODEL_TIE)
		{
			Ready4SaveView2Img(GetDocument()->m_strTieViewImgPath, true);
		}
		else
		{
			Ready4SaveView2Img(GetDocument()->m_strGcpViewImgPath, true);
		}
		
	}
	Invalidate();
}

void CMDomQuaChkSysView::OnButtonColor3()
{
	// TODO: Add your command handler code here
	if (FunGetMainFrameHand()->GetFillColor3() != m_pColorFillLevel[2]);
	m_pColorFillLevel[2] = FunGetMainFrameHand()->GetFillColor3();
	RefreshFillColor(3);
	RefreshPointColor();
	if (m_bViewFill)
	{
		if (GetDocument()->m_eModelType == MODEL_TIE)
		{
			Ready4SaveView2Img(GetDocument()->m_strTieViewImgPath, true);
		}
		else
		{
			Ready4SaveView2Img(GetDocument()->m_strGcpViewImgPath, true);
		}
		
	}
	Invalidate();
}

void CMDomQuaChkSysView::OnButtonColor4()
{
	// TODO: Add your command handler code here
	if (FunGetMainFrameHand()->GetFillColor4() != m_pColorFillLevel[3]);
	m_pColorFillLevel[3] = FunGetMainFrameHand()->GetFillColor4();
	RefreshFillColor(4);
	RefreshPointColor();
	if (m_bViewFill)
	{
		if (GetDocument()->m_eModelType == MODEL_TIE)
		{
			Ready4SaveView2Img(GetDocument()->m_strTieViewImgPath, true);
		}
		else
		{
			Ready4SaveView2Img(GetDocument()->m_strGcpViewImgPath, true);
		}
	
	}
	Invalidate();
}

bool CMDomQuaChkSysView::IsPointInPolygon(Point2D point, vector<Point2D> poly)
{
	int polySides = poly.size();
	int i, j = polySides - 1;
	bool  oddNodes = false;

	for (i = 0; i < polySides; i++) 
	{
		if ((poly[i].y < point.y && poly[j].y >= point.y
			|| poly[j].y < point.y && poly[i].y >= point.y)
			&& (poly[i].x <= point.x || poly[j].x <= point.x)) 
		{
			oddNodes ^= (poly[i].x + (point.y - poly[i].y) / (poly[j].y - poly[i].y)*(poly[j].x - poly[i].x) < point.x);
		}
		j = i;
	}
	return oddNodes;
}

void CMDomQuaChkSysView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CMDomQuaChkSysDoc *pDoc = (CMDomQuaChkSysDoc *)GetDocument();
	if (pDoc->m_bIsIdentify)
	{
		Point2D ptLBtnDnGeo = ptCln2Geo(point);

		if (pDoc->m_eModelType == MODEL_TIE)
		{
			m_vecSelectModelFromView.clear();
			vector<vector<Point2D>> vvTieModelArea = pDoc->m_DomChkPrj.GetTieModelArea();
			for (int ciri = 0; ciri < vvTieModelArea.size(); ++ciri)
			{
				if (IsPointInPolygon(ptLBtnDnGeo, vvTieModelArea[ciri]))
				{
					m_vecSelectModelFromView.push_back(ciri);
				}
			}
		}
		else if (pDoc->m_eModelType == MODEL_CTL)
		{
			m_vecSelectModelFromView.clear();
			vector<vector<Point2D>> vvGcpModelArea = pDoc->m_DomChkPrj.GetGcpModelArea();
			for (int ciri = 0; ciri < vvGcpModelArea.size(); ++ciri)
			{
				if (IsPointInPolygon(ptLBtnDnGeo, vvGcpModelArea[ciri]))
				{
					m_vecSelectModelFromView.push_back(ciri);
				}
			}
		}
		if (m_vecSelectModelFromView.size() == 0)
		{
			m_vecIdentifyPoint.DeleteVBO();
			Invalidate(true);
			return;
		}
		else
		{
			vector<Point2D> tempPointVec;
			tempPointVec.push_back(ptLBtnDnGeo);
			m_vecIdentifyPoint.DeleteVBO();
			m_vecIdentifyPoint.CreateVectorVBO(1 * tempPointVec.size(), vPoint);
			double *pV = new double[2 * tempPointVec.size()];
			double *pC = new double[4 * tempPointVec.size()];
			FunVectorPt2VertexXY(&tempPointVec, 2, pV);

			FunCreateVertexColor(tempPointVec.size(), 1.0, 0.0, 0.0, pC);
			m_vecIdentifyPoint.FillVectorVBO(pV, pC, tempPointVec.size());
			delete[]pC;
			delete[]pV;
			tempPointVec.clear();
			Invalidate(true);
			FunGetMainFrameHand()->SendMessage(ID_MYMSG_FILL_MODELLIST, (WPARAM)&pDoc->m_eModelType, (LPARAM)pDoc);
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CMDomQuaChkSysView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_vecIdentifyPoint.DeleteVBO();
	Invalidate(true);
	CView::OnLButtonUp(nFlags, point);
}
