// MyImgViewWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "MyImgViewWnd.h"
#include "MyFunctions.h"
#include "MainFrm.h"
#include "MDomQuaChkSysDoc.h"

// CMyImgViewWnd

IMPLEMENT_DYNAMIC(CMyImgViewWnd, CMyDockablePane)
extern HGLRC g_pRC;
extern HGLRC g_pRCSharing;
CMyImgViewWnd::CMyImgViewWnd()
{
	m_pDC = NULL;
	m_bIsLinkerageClicked = false;
}

CMyImgViewWnd::~CMyImgViewWnd()
{
}

BEGIN_MESSAGE_MAP(CMyImgViewWnd, CMyDockablePane)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()

	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(ID_MYMSG_IMGRENDER,&OnRenderImage)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMyImgViewWnd message handlers

Point2D CMyImgViewWnd::ptCln2Geo(CPoint& ptCln)
{
	Point2D ptGeo;
	ptGeo.x = m_ptClnCenter2Geo.x + 1.0*(ptCln.x - m_RectCln.CenterPoint().x) / m_fZoomRateCln2Geo;
	ptGeo.y = m_ptClnCenter2Geo.y - 1.0*(ptCln.y - m_RectCln.CenterPoint().y) / m_fZoomRateCln2Geo;
	return ptGeo;
}
CPoint CMyImgViewWnd::ptGeo2Cln(Point2D& ptGeo)
{
	CPoint ptCln;
	ptCln.x = (ptGeo.x - m_ptClnCenter2Geo.x) * m_fZoomRateCln2Geo + m_RectCln.CenterPoint().x;
	ptCln.y = (m_ptClnCenter2Geo.y - ptGeo.y) * m_fZoomRateCln2Geo + m_RectCln.CenterPoint().y;
	return ptCln;
}
Point2D CMyImgViewWnd::ClnCenter2Geo(CPoint ptCln, Point2D ptGeo)
{
	Point2D ptClnCenter2Geo;
	ptClnCenter2Geo.x = ptGeo.x - 1.0*(ptCln.x - m_RectCln.CenterPoint().x) / m_fZoomRateCln2Geo;
	ptClnCenter2Geo.y = ptGeo.y + 1.0*(ptCln.y - m_RectCln.CenterPoint().y) / m_fZoomRateCln2Geo;
	return ptClnCenter2Geo;
}
void CMyImgViewWnd::ResetRectCln2Geo()
{
	m_RectCln2Geo = CRectD(ptCln2Geo(m_RectCln.TopLeft()), ptCln2Geo(m_RectCln.BottomRight()));
}
void CMyImgViewWnd::InitImgViewInfo(CString strImgPath)
{
	m_Img2Render.CloseImg();
	if (!m_Img2Render.OpenImage(strImgPath))
	{
		AfxMessageBox(strImgPath + " ===错误！");
	}
	m_RectImgGeo = m_Img2Render.GetGeoInfo().dGeoRect;
	m_ptClnCenter2Geo = m_RectImgGeo.CenterPoint();
	m_fZoomRateCln2Geo = min(m_RectCln.Width()*1.0 / m_RectImgGeo.Width(),
		m_RectCln.Height()*1.0 / m_RectImgGeo.Height())*0.9;
	m_fZoomRateCln2GeoMin = m_fZoomRateCln2Geo / 10;
	m_fZoomRateCln2GeoMax = min(m_RectCln.Width(),m_RectCln.Height())*m_Img2Render.GetCols()*1.0/m_RectImgGeo.Width()/50;
	SetWindowText(FunGetFileName(strImgPath));
}

void CMyImgViewWnd::RenderImage()
{
	CRectD RectShowGeo, RectShowImg; CRect RectShowCln;
	if (!FunIntersectCRectD(m_RectCln2Geo,m_RectImgGeo,  RectShowGeo)) return;
	RectShowCln = CRect(ptGeo2Cln(RectShowGeo.LeftTop()), ptGeo2Cln(RectShowGeo.RightBottom()));
	RectShowImg = CRectD(m_Img2Render.GeoPt2Img(RectShowGeo.LeftTop()), m_Img2Render.GeoPt2Img(RectShowGeo.RightBottom()));
	int nShowSizX = min(m_RectCln.Width(), RectShowCln.Width());
	int nShowSizY = min(m_RectCln.Height(), RectShowCln.Height());
	float nReadStaX, nReadStaY;
	nReadStaX = max(0, RectShowImg.left);
	nReadStaY = max(0, RectShowImg.top);
	BYTE* pMem = new BYTE[nShowSizX*nShowSizY* 4]; memset(pMem, 0, nShowSizX*nShowSizY* 4);
	int pBandOrder[3] = { 0,1,2 };
	double dZoomRateCln2Img = m_fZoomRateCln2Geo*m_RectImgGeo.Width() / m_Img2Render.GetCols();
	m_Img2Render.ReadData4Texture(nReadStaX, nReadStaY, nShowSizX, nShowSizY, dZoomRateCln2Img, pBandOrder, pMem);
	m_ImgTexRender.DeleteTexture();
	m_ImgTexRender.CreateTexture(nShowSizX,nShowSizY);
	m_ImgTexRender.FillTexture(pMem);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);
	m_ImgTexRender.DisplayTexture(RectShowCln);
	delete[]pMem;
}

void CMyImgViewWnd::SelectPoint(Point2D pt)
{
	m_ptSelected = pt;
    m_ptClnCenter2Geo = m_ptSelected;
	m_fZoomRateCln2Geo = 5 * m_Img2Render.GetCols() / m_Img2Render.GetGeoInfo().dGeoRect.Width();   //modified by tk (代码重复)
}

void CMyImgViewWnd::RenderSelectPoint()
{
	if (m_ptSelected == Point2D()) return;
	Point2D pt = m_ptSelected;
		m_PolygonSelectPt.DeleteVBO();
	m_PolygonSelectPt.CreateVectorVBO(4, vLine);
	CPoint ptCln = ptGeo2Cln(pt);
	double *pV = new double[8], *pC = new double[16];
	pV[0] = ptCln.x - 5; pV[1] = ptCln.y - 5;
	pV[2] = ptCln.x - 5; pV[3] = ptCln.y + 5;
	pV[4] = ptCln.x + 5; pV[5] = ptCln.y + 5;
	pV[6] = ptCln.x + 5; pV[7] = ptCln.y - 5;
	FunCreateVertexColor(4, 0, 0.8, 0, pC);
	m_PolygonSelectPt.FillVectorVBO(pV, pC, 4);
	delete[]pV;
	delete[]pC;
//	m_ptClnCenter2Geo = m_ptSelected;
}

LRESULT CMyImgViewWnd::OnRenderImage(WPARAM wParam, LPARAM lParam)
{
	CString strImgPath = *(CString*)wParam;	
	InitImgViewInfo(strImgPath);
	m_strImgPath = strImgPath;
	if(lParam!=NULL)
	{
		m_vecPtGeo = *(vector<Point2D>*)lParam;
	}
	return 1;
}

int CMyImgViewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMyDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pDC = new CClientDC(this);//初始化OPENGL设置定时器
	SetupPixelFormat(m_pDC, m_hPalette);
	InitOpenGL(g_pRCSharing, m_pDC, m_hPalette);
	InitOpenGL(g_pRC, m_pDC, m_hPalette);
	wglShareLists(g_pRCSharing, g_pRC);
	wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRC);
	return 0;
}

BOOL CMyImgViewWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	bool bMakeCurrent = false;
	do { bMakeCurrent = wglMakeCurrent(m_pDC->GetSafeHdc(), g_pRC); } while (!bMakeCurrent);
	PrepareDraw(1, 1, 1, 0);
	glViewport(0, 0, m_RectCln.Width(), m_RectCln.Height());
	glLoadIdentity();

	ResetRectCln2Geo();
//	glOrtho(m_RectCln2Geo.left, m_RectCln2Geo.right, m_RectCln2Geo.bottom, m_RectCln2Geo.top, -1, 1);
	glOrtho(m_RectCln.left, m_RectCln.right, m_RectCln.bottom, m_RectCln.top, -1, 1);
	if(m_Img2Render.GetBands()>0)
		RenderImage(); 
// 	glLoadIdentity();
// 	glOrtho(m_RectCln2Geo.left, m_RectCln2Geo.right, m_RectCln2Geo.bottom, m_RectCln2Geo.top, -1, 1);
	m_PointRender.DeleteVBO();
	m_PointRender.CreateVectorVBO(m_vecPtGeo.size(), vPoint);
	vector<Point2D> vecPtCln(m_vecPtGeo.size());
	for (int i = 0; i < m_vecPtGeo.size(); i++)
	{
		CPoint ptCln = ptGeo2Cln(m_vecPtGeo[i]);   
		vecPtCln[i] = Point2D(ptCln.x, ptCln.y);
	}
	double *pV = new double[vecPtCln.size() * 2];
	double *pC = new double[vecPtCln.size() * 4];
	FunVectorPt2VertexXY(&vecPtCln, 2, pV);
	FunCreateVertexColor(vecPtCln.size(), 1, 1, 0, pC);
	m_PointRender.FillVectorVBO(pV, pC, vecPtCln.size());
	delete[]pV;
	delete[]pC;
	vector<Point2D>().swap(vecPtCln);
	m_PointRender.Display(2);
	RenderSelectPoint();
	m_PolygonSelectPt.Display(2);
	EndDraw(m_pDC);
	return TRUE;
	return CMyDockablePane::OnEraseBkgnd(pDC);
}

void CMyImgViewWnd::OnSize(UINT nType, int cx, int cy)
{
	CMyDockablePane::OnSize(nType, cx, cy);
	GetClientRect(m_RectCln);
	//m_RectCln.bottom -= 30;
	// TODO: Add your message handler code here
}

void CMyImgViewWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default
	
	CMyDockablePane::OnNcCalcSize(bCalcValidRects, lpncsp);
	//lpncsp->rgrc[0].bottom += 30;
}

BOOL CMyImgViewWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if(!m_ImgTexRender.IsTextured()) return CMyDockablePane::OnMouseWheel(nFlags, zDelta, pt);
	ScreenToClient(&pt);
	Point2D ptGeo = ptCln2Geo(pt);
	m_fZoomRateCln2Geo *= pow(1.15, 1.0*zDelta / 120);
// 	m_fZoomRateCln2Geo = min(m_fZoomRateCln2Geo, m_fZoomRateCln2GeoMax);
 //	m_fZoomRateCln2Geo = max(m_fZoomRateCln2Geo, m_fZoomRateCln2GeoMin);
	m_ptClnCenter2Geo = ClnCenter2Geo(pt, ptGeo);
//	ResetRectCln2Geo();
	Invalidate();
	return CMyDockablePane::OnMouseWheel(nFlags, zDelta, pt);
}

void CMyImgViewWnd::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_ImgTexRender.IsTextured()) CMyDockablePane::OnMButtonDown(nFlags, point);
	SetCapture();
	m_ptMBtnDnGeo = ptCln2Geo(point);
	CMyDockablePane::OnMButtonDown(nFlags, point);
}

void CMyImgViewWnd::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_ImgTexRender.IsTextured()) CMyDockablePane::OnMButtonUp(nFlags, point);
	ReleaseCapture();
	CMyDockablePane::OnMButtonUp(nFlags, point);
}

void CMyImgViewWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_ImgTexRender.IsTextured())CMyDockablePane::OnMouseMove(nFlags, point);
	if ((nFlags&MK_MBUTTON) == MK_MBUTTON)
	{
		m_ptClnCenter2Geo = ClnCenter2Geo(point, m_ptMBtnDnGeo);
//		ResetRectCln2Geo();
		Invalidate();
	}
	CMyDockablePane::OnMouseMove(nFlags, point);
}

void CMyImgViewWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (FunGetMainFrameHand()->IsImgPointLinkage)
	{
		//ScreenToClient(&pt);
		m_ptSelected = ptCln2Geo(point);
		m_bIsLinkerageClicked = true;
		FunGetMainFrameHand()->SendMessage(ID_MYMSG_IMAGEPOINTLINKAGE, NULL, NULL);

		double dMinDis = 999999;

		for (int i = 0; i < m_vecPtGeo.size(); i++)
		{
			double dTempDis = (m_ptSelected.x - m_vecPtGeo[i].x)*(m_ptSelected.x - m_vecPtGeo[i].x) + (m_ptSelected.y - m_vecPtGeo[i].y)*(m_ptSelected.y - m_vecPtGeo[i].y);
			if (dTempDis < dMinDis)
			{
				FunGetMainFrameHand()->m_nSelectedPoint = i;
				dMinDis = dTempDis;
			}
		}
		if (dMinDis > 0.1) FunGetMainFrameHand()->m_nSelectedPoint = -1;
		FunGetMainFrameHand()->SendMessage(ID_MYMSG_FILL_MODELLIST, (WPARAM)&FunGetDocHand()->m_eModelType, (LPARAM)FunGetDocHand());
	}
	CMyDockablePane::OnLButtonDown(nFlags, point);
}
