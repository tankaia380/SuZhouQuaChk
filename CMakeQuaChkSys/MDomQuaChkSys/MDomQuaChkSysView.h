// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ������� 
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// MDomQuaChkSysView.h : CMDomQuaChkSysView ��Ľӿ�
//

#pragma once

#include "MyViewerRender.h"
class CMDomQuaChkSysView : public CView
{
protected: // �������л�����
	CMDomQuaChkSysView();
	DECLARE_DYNCREATE(CMDomQuaChkSysView)

	// ����
public:
	CMDomQuaChkSysDoc* GetDocument() const;

	// ����
public:
	CMyViewerRender m_BkgrdRender;   //����
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
	HPALETTE      m_hPalette; //opengl��ɫ��
	CClientDC*    m_pDC;      //opengl��ͼ�豸

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
	// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// ʵ��
public:
	virtual ~CMDomQuaChkSysView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // MDomQuaChkSysView.cpp �еĵ��԰汾
inline CMDomQuaChkSysDoc* CMDomQuaChkSysView::GetDocument() const
{
	return reinterpret_cast<CMDomQuaChkSysDoc*>(m_pDocument);
}
#endif


