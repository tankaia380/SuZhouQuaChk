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

// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "Resource.h"
#include "GradientProgressCtrl.h"
#include "MyTreeCtrl.h"
#include "MySourceDataWnd.h"
#include "MyImgViewWnd.h"
#include "MyMatchPtChartWnd.h"
#include "MyMFCRibbonBar.h"
class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); if (!bNameValid) strName.Empty(); }
};

class CMainFrame : public CFrameWndEx
{

protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// 特性
public:

	// 操作
public:

	// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	COLORREF GetDomColor();
	COLORREF GetRefColor();
	COLORREF GetFillColor1();
	COLORREF GetFillColor2();
	COLORREF GetFillColor3();
	COLORREF GetFillColor4();

	float GetTieThresold();
	float GetGcpThresold();
	void InitTieThresold(float fvalue);
	void InitGcpThresold(float fvalue);
	bool IsModelListFilled();
	MstuMchModel GetMchModel();

	CMyImgViewWnd       m_wndImgViewLeft;
	CMyImgViewWnd       m_wndImgViewRight;
protected:  // 控件条嵌入成员
	CMyMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CMySourceDataWnd  m_wndSourceData;

	CMFCCaptionBar    m_wndCaptionBar;
	CGradientProgressCtrl m_wndProgressBar;

	CMFCRibbonStatusBarPane *m_pStatusPaneLabel;
	CMFCRibbonStatusBarPane *m_pStatusPaneProcessing;
	CMFCRibbonStatusBarPane *m_pStatusPanePosition;
	CMFCRibbonStatusBarPane *m_pStatusPaneSysTime;

	bool m_bWndReady;
public:
	bool IsImgPointLinkage;
	int  m_nSelectedPoint;
protected:
	void InitStatusBar();
	void InitWndProgressBar();
	void ResetStatusPaneSize();
	// 生成的消息映射函数
public:
	
	afx_msg LRESULT OnMsgProgressPos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgShowProgressBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgUpdateStatusPos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgFillImgTree(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgFillPropList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgFillModelList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreateImgRender(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectPoint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnImgPointLinkage(WPARAM wParam, LPARAM lParam);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
// 	afx_msg void OnViewCaptionBar();
// 	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;
public:
	virtual void ActivateFrame(int nCmdShow = -1);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCheckViewProperitywnd();
	afx_msg void OnUpdateCheckViewProperitywnd(CCmdUI *pCmdUI);
	afx_msg void OnCheckViewLeftimgwnd();
	afx_msg void OnUpdateCheckViewLeftimgwnd(CCmdUI *pCmdUI);
	afx_msg void OnCheckViewRightimgwnd();
	afx_msg void OnUpdateCheckViewRightimgwnd(CCmdUI *pCmdUI);
	afx_msg void OnCheckViewSourcedatawnd();
	afx_msg void OnUpdateCheckViewSourcedatawnd(CCmdUI *pCmdUI);
	afx_msg void OnCheckViewModelwnd();
	afx_msg void OnUpdateCheckViewModelwnd(CCmdUI *pCmdUI);
};


