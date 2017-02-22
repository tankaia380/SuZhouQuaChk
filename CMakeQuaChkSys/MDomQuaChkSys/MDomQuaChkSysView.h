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

// MDomQuaChkSysView.h : CMDomQuaChkSysView 类的接口
//

#pragma once

#include "MyViewerRender.h"
class CMDomQuaChkSysView : public CView
{
protected: // 仅从序列化创建
	CMDomQuaChkSysView();
	DECLARE_DYNCREATE(CMDomQuaChkSysView)

	// 特性
public:
	CMDomQuaChkSysDoc* GetDocument() const;

	// 操作
public:
	CMyViewerRender m_BkgrdRender;   //背景
	vector<CMyViewerRender> m_vecRenderDomArea;
	vector<CMyViewerRender> m_vecRenderRefArea;
	vector<CMyViewerRender> m_vecRenderSelected;
	vector<CMyViewerRender> m_vecRenderFillPolygonTie;
	vector<CMyViewerRender> m_vecRenderFillPolygonGcp;
	vector<int> m_vecLevelIdPolygonTie;
	vector<int> m_vecLevelIdPolygonGcp;
	vector<int> m_vecSelectModelFromView;     //add by tk

	CMyViewerRender m_RenderModelPoint;
	CMyViewerRender m_vecIdentifyPoint;

	CRect  m_RectCln;
	CRectD m_RectCln2Geo;
	CRectD m_RectCqViewGeo;
	Point2D m_ptClnCenter2Geo;
	Point2D m_ptMBtnDnGeo;
	float m_fZoomRateCln2Geo;
	float m_fZoomRateCln2GeoMin;
	float m_fZoomRateCln2GeoMax;
	bool m_bSaveView2Image;
	bool m_bAddlViewImgLabel;
	CString m_strViewImgSavePath;
	HPALETTE      m_hPalette; //opengl调色表
	CClientDC*    m_pDC;      //opengl绘图设备

	COLORREF m_ColorDom;
	COLORREF m_ColorRef;
	COLORREF m_pColorFillLevel[4];
	bool m_bViewDom;
	bool m_bViewRef;
	bool m_bViewFill;
	bool m_bViewMatchPoint;
	bool m_bViewSelectPolygon;
	bool m_bViewChart;
	CPoint ptGeo2Cln(Point2D& ptGeo);
	Point2D ptCln2Geo(CPoint& ptCln);
	Point2D ClnCenter2Geo(CPoint ptCln, Point2D ptGeo);
	void ResetRectCln2Geo();
	void DeleteAreaRender()
	{
		for (int i = 0; i < m_vecRenderDomArea.size(); i++)
			m_vecRenderDomArea[i].DeleteVBO();
		vector<CMyViewerRender>().swap(m_vecRenderDomArea);
		for (int i = 0; i < m_vecRenderRefArea.size(); i++)
			m_vecRenderRefArea[i].DeleteVBO();
		vector<CMyViewerRender>().swap(m_vecRenderRefArea);
		DeleteSelectRender();
	}
	void DeleteSelectRender()
	{
		for (int i = 0; i < m_vecRenderSelected.size(); i++)
			m_vecRenderSelected[i].DeleteVBO();
		vector<CMyViewerRender>().swap(m_vecRenderSelected);
	}
	void Ready4SaveView2Img(CString strSavePath, bool bAddLabel);
	void SaveView2Image(CRect RectSaveView, CString strSavePath, bool bAddLabel = false);
	void RefreshFillColor(int nLevel);
	void RefreshPointColor();
	bool IsPointInPolygon(Point2D point, vector<Point2D> poly);
	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CMDomQuaChkSysView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
public:
	afx_msg LRESULT OnFillReportViewTie(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFillReportViewGcp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitImgRender(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectedImgRender(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreateModelPointRender(WPARAM wParam, LPARAM lParam);
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnButtonReportFill();
	afx_msg void OnUpdateButtonReportFill(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCheckViewMatchpoint(CCmdUI *pCmdUI);
	afx_msg void OnCheckViewResidualchart();
	afx_msg void OnUpdateCheckViewResidualchart(CCmdUI *pCmdUI);
	afx_msg void OnCheckViewMatchpoint();
	afx_msg void OnCheck5ViewSelectPolygon();
	afx_msg void OnUpdateCheck5ViewSelectPolygon(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonColor1(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonColor2(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonColor3(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonColor4(CCmdUI *pCmdUI);
	afx_msg void OnButtonColor1();
	afx_msg void OnButtonColor2();
	afx_msg void OnButtonColor3();
	afx_msg void OnButtonColor4();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // MDomQuaChkSysView.cpp 中的调试版本
inline CMDomQuaChkSysDoc* CMDomQuaChkSysView::GetDocument() const
{
	return reinterpret_cast<CMDomQuaChkSysDoc*>(m_pDocument);
}
#endif


