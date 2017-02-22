#pragma once


// CMyImgViewWnd
#include "MyDockablePane.h"
#include "MyFunctions.h"
#include "MyViewerRender.h"
class CMyImgViewWnd : public CMyDockablePane
{
	DECLARE_DYNAMIC(CMyImgViewWnd)

public:
	CMyImgViewWnd();
	virtual ~CMyImgViewWnd();
	CMyTextureRender  m_ImgTexRender;
	CMyViewerRender m_PointRender;
	CMyViewerRender m_PolygonSelectPt;
	CMImage4Check m_Img2Render;
	CRect  m_RectCln;
	CRectD m_RectCln2Geo;
	CRectD m_RectImgGeo;
	Point2D m_ptClnCenter2Geo;
	Point2D m_ptMBtnDnGeo;
	float m_fZoomRateCln2Geo;
	float m_fZoomRateCln2GeoMin;
	float m_fZoomRateCln2GeoMax;
	HPALETTE      m_hPalette; //opengl调色表
	CClientDC*    m_pDC;      //opengl绘图设备
	bool m_bIsLinkerageClicked;

	vector<Point2D> m_vecPtGeo;
	Point2D m_ptSelected;
	CString m_strImgPath;
	CPoint ptGeo2Cln(Point2D& ptGeo);
	Point2D ptCln2Geo(CPoint& ptCln);
	Point2D ClnCenter2Geo(CPoint ptCln, Point2D ptGeo);
	void ResetRectCln2Geo();
	void InitImgViewInfo(CString strImgPath);
	void RenderImage();
	void SelectPoint(Point2D pt);
	void RenderSelectPoint();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnRenderImage(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


