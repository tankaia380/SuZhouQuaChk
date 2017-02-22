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

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"

#include "MainFrm.h"
#include "MyFunctions.h"
#include "MDomQuaChkSysDoc.h"
#include "MultiThreadFuns.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
// 	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
// 	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_MESSAGE(ID_MYMSG_PROGRESSBAR,& OnMsgProgressPos)
	ON_MESSAGE(ID_MYMSG_SHOWPROGRESSBAR,& OnMsgShowProgressBar)
	ON_MESSAGE(ID_MYMSG_UPDATESTATUSPOS,& OnMsgUpdateStatusPos)
	ON_MESSAGE(ID_MYMSG_FILL_IMGTREE, &OnMsgFillImgTree)
	ON_MESSAGE(ID_MYMSG_FILL_PROPLIST, &OnMsgFillPropList)
	ON_MESSAGE(ID_MYMSG_FILL_MODELLIST, &OnMsgFillModelList)
	ON_MESSAGE(ID_MYMSG_MODELIMGRENDER, &OnCreateImgRender)
	ON_MESSAGE(ID_MYMSG_SELECTPOINT, &OnSelectPoint)
	ON_MESSAGE(ID_MYMSG_IMAGEPOINTLINKAGE, &OnImgPointLinkage)
	ON_COMMAND(ID_CHECK_VIEW_PROPERITYWND, &CMainFrame::OnCheckViewProperitywnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIEW_PROPERITYWND, &CMainFrame::OnUpdateCheckViewProperitywnd)
	ON_COMMAND(ID_CHECK_VIEW_LEFTIMGWND, &CMainFrame::OnCheckViewLeftimgwnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIEW_LEFTIMGWND, &CMainFrame::OnUpdateCheckViewLeftimgwnd)
	ON_COMMAND(ID_CHECK_VIEW_RIGHTIMGWND, &CMainFrame::OnCheckViewRightimgwnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIEW_RIGHTIMGWND, &CMainFrame::OnUpdateCheckViewRightimgwnd)
	ON_COMMAND(ID_CHECK_VIEW_SOURCEDATAWND, &CMainFrame::OnCheckViewSourcedatawnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIEW_SOURCEDATAWND, &CMainFrame::OnUpdateCheckViewSourcedatawnd)
	ON_COMMAND(ID_CHECK_VIEW_MODELWND, &CMainFrame::OnCheckViewModelwnd)
	ON_UPDATE_COMMAND_UI(ID_CHECK_VIEW_MODELWND, &CMainFrame::OnUpdateCheckViewModelwnd)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	m_bWndReady = false;
	IsImgPointLinkage = true;
	m_nSelectedPoint = -1;
}
CMainFrame::~CMainFrame()
{
}

LRESULT CMainFrame::OnMsgProgressPos(WPARAM wParam, LPARAM lParam)
{
	double dPos = *(double*)wParam;
	const char *strInfo = (const char*)lParam;
	m_wndProgressBar.SetPos(dPos);
	m_pStatusPaneProcessing->SetText(strInfo);
	m_pStatusPaneProcessing->Redraw();
	return 1;
}
LRESULT CMainFrame::OnMsgShowProgressBar(WPARAM wParam, LPARAM lParam)
{
	bool  bShow = *(bool*)wParam;
	m_wndProgressBar.ShowWindow(bShow);
	return 1;
}

LRESULT CMainFrame::OnMsgUpdateStatusPos(WPARAM wParam, LPARAM lParam)
{
	m_pStatusPanePosition->SetText((char*)wParam);
	m_pStatusPanePosition->Redraw();
	return 1;
}
LRESULT CMainFrame::OnMsgFillImgTree(WPARAM wParam, LPARAM lParam)
{
	m_wndSourceData.FillImgTree(*((vector<CMImage4Check>*)wParam), *((vector<CMImage4Check>*)lParam));
	return 1;
}
LRESULT CMainFrame::OnMsgFillPropList(WPARAM wParam, LPARAM lParam)
{
	MstuPropOfImg PropInfo;
	CString strImgPath = *(CString*)wParam;
	CMImage4Check image;
	if (image.OpenImage(strImgPath))
	{
		strcpy(PropInfo.strFileName, FunGetFileName(strImgPath, true));
		PropInfo.nWidth = image.GetCols();
		PropInfo.nHeight = image.GetRows();
		PropInfo.dResX = image.GetResolutionX();
		PropInfo.dResY = image.GetResolutionY();
		PropInfo.eCoordinateSys = image.GetGeoInfo().eCoordinateSysInfo;
		PropInfo.eProjectInfo = image.GetGeoInfo().eProjInfo;
		long long llFIleSize = FunGetFileSize(strImgPath);
		PropInfo.dFileSizeMB = llFIleSize*1.0 / 1024 / 1024;
	}
	m_wndProperties.FillPropList(PropInfo);
	return 1;
}
LRESULT CMainFrame::OnMsgFillModelList(WPARAM wParam, LPARAM lParam)
{
	MenmModelType eModelType = *(MenmModelType*)wParam;
	CMDomQuaChkSysDoc *pDoc = (CMDomQuaChkSysDoc *)lParam;

	switch (eModelType)
	{
	case MODEL_TIE:
	{
		vector<MstuTieChkModel> vecTieModel = pDoc->m_DomChkPrj.GetTieModelInfo();
		m_wndOutput.FillListTieModel(vecTieModel);
		vector<MstuTieChkModel>().swap(vecTieModel);
	}
		break;
	case MODEL_CTL:
	{
		vector<MstuGcpChkModel> vecGcpModel = pDoc->m_DomChkPrj.GetGcpModelInfo();
		m_wndOutput.FillListGcpModel(vecGcpModel);
		vector<MstuGcpChkModel>().swap(vecGcpModel);
	}
		break;
	default:
		break;
	}
	return 1;
}
LRESULT CMainFrame::OnCreateImgRender(WPARAM wParam, LPARAM lParam)
{
	if (wParam == NULL) return -1;
	vector<int>* pvDomIdx = (vector<int>*)wParam;
	MstuMchModel* pModel = (MstuMchModel*)lParam;

	CString strImgPathLeft, strImgPathRight;

	strImgPathLeft = FunGetDocHand()->m_DomChkPrj.GetDomInfo((*pvDomIdx)[0]).GetImgPath();
	m_wndImgViewLeft.SendMessage(ID_MYMSG_IMGRENDER, (WPARAM)&strImgPathLeft, (LPARAM)&pModel->vecObjPtXY); 

	vector<Point2D> vecPointR;
	
	if (pModel->eType == MODEL_TIE)
	{
		CMImage4Check DomImgChk = FunGetDocHand()->m_DomChkPrj.GetDomInfo((*pvDomIdx)[1]);
		for (int ciri = 0; ciri < pModel->vecImgPt2.size(); ++ciri)
		{
			Point2D ptTemp = DomImgChk.ImgPt2Geo(pModel->vecImgPt2[ciri]);
			vecPointR.push_back(ptTemp);
		}
		strImgPathRight = DomImgChk.GetImgPath();
		m_wndImgViewRight.SendMessage(ID_MYMSG_IMGRENDER, (WPARAM)&strImgPathRight, (LPARAM)&vecPointR);   //pModel->vecObjPtXY
	}
	else if (pModel->eType == MODEL_CTL)
	{
		CMImage4Check DomImgChk = FunGetDocHand()->m_DomChkPrj.GetRefInfo((*pvDomIdx)[1]);
		for (int ciri = 0; ciri < pModel->vecImgPt2.size(); ++ciri)
		{
			CString strTemp = pModel->vecPtIdx[ciri].c_str();
			vector<CString> vParts = FunStringTok(strTemp, "_");
			if (vParts.size() < 2) continue;
			if (atoi(vParts[2]) == (*pvDomIdx)[1])
			{
				Point2D ptTemp = DomImgChk.ImgPt2Geo(pModel->vecImgPt2[ciri]);
				vecPointR.push_back(ptTemp);
			}
		}
		strImgPathRight = DomImgChk.GetImgPath();
		m_wndImgViewRight.SendMessage(ID_MYMSG_IMGRENDER, (WPARAM)&strImgPathRight, (LPARAM)&vecPointR);   // pModel->vecObjPtXY
	}

	m_wndImgViewLeft.Invalidate();
	m_wndImgViewRight.Invalidate();
	//	pModel->Clear();
	//	vector<int>().swap(vDomIdx);
	//	pModel->Clear();
	return 1;
}
LRESULT CMainFrame::OnSelectPoint(WPARAM wParam, LPARAM lParam)
{
	vector<Point2D> pt = *(vector<Point2D>*)wParam;
	CString strPtIdx = *(CString*)lParam;
	vector<CString> vParts = FunStringTok(strPtIdx, "_");
	if (vParts[0] == "Ctl")
	{
		//int nRefIdx = atoi(vParts[2]);
		//CString strRefPath = FunGetDocHand()->m_DomChkPrj.GetRefInfo(nRefIdx).GetImgPath();
		//if (m_wndImgViewRight.m_strImgPath != strRefPath)
		//{
		//	/*vector<Point2D> vecPointR;
		//	CMImage4Check DomImgChk = FunGetDocHand()->m_DomChkPrj.GetRefInfo((*pvDomIdx)[1]);
		//	for (int ciri = 0; ciri < pModel->vecImgPt2.size(); ++ciri)
		//	{
		//		CString strTemp = pModel->vecPtIdx[ciri].c_str();
		//		vector<CString> vParts = FunStringTok(strTemp, "_");
		//		if (vParts.size() < 2) continue;
		//		if (atoi(vParts[2]) == (*pvDomIdx)[1])
		//		{
		//			Point2D ptTemp = DomImgChk.ImgPt2Geo(pModel->vecImgPt2[ciri]);
		//			vecPointR.push_back(ptTemp);
		//		}
		//	}*/

		//	m_wndImgViewRight.SendMessage(ID_MYMSG_IMGRENDER, (WPARAM)&strRefPath, NULL);		
		//}
	}
	m_wndImgViewLeft.SelectPoint(pt[0]);
	m_wndImgViewRight.SelectPoint(pt[1]);
	m_wndImgViewLeft.Invalidate();
	m_wndImgViewRight.Invalidate();
	return 1;
}

LRESULT CMainFrame::OnImgPointLinkage(WPARAM wParam, LPARAM lParam)
{
	if (m_wndImgViewLeft.m_bIsLinkerageClicked)
	{
		m_wndImgViewRight.m_ptClnCenter2Geo = m_wndImgViewRight.m_ptSelected = m_wndImgViewLeft.m_ptSelected;
		m_wndImgViewRight.m_fZoomRateCln2Geo = m_wndImgViewLeft.m_fZoomRateCln2Geo;
		m_wndImgViewLeft.m_bIsLinkerageClicked = false;
		m_wndImgViewLeft.Invalidate();
		m_wndImgViewRight.Invalidate();
	}
	else if (m_wndImgViewRight.m_bIsLinkerageClicked)
	{
		m_wndImgViewLeft.m_ptClnCenter2Geo = m_wndImgViewLeft.m_ptSelected = m_wndImgViewRight.m_ptSelected;
		m_wndImgViewLeft.m_fZoomRateCln2Geo = m_wndImgViewRight.m_fZoomRateCln2Geo;
		m_wndImgViewRight.m_bIsLinkerageClicked = false;
		m_wndImgViewLeft.Invalidate();
		m_wndImgViewRight.Invalidate();
	}
	return 1;
}


void CMainFrame::InitStatusBar()
{
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, "", TRUE), "@2016 By Whu DpGrid Group");
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, "", TRUE), "x(0), y(0)");
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, "", TRUE), "Ready");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, "System Time", TRUE), FunGetSysTimeStr());
	m_pStatusPaneLabel = (CMFCRibbonStatusBarPane*)m_wndStatusBar.GetElement(0);
	m_pStatusPanePosition = (CMFCRibbonStatusBarPane*)m_wndStatusBar.GetElement(1);
	m_pStatusPaneProcessing = (CMFCRibbonStatusBarPane*)m_wndStatusBar.GetElement(2);
	m_pStatusPaneSysTime = (CMFCRibbonStatusBarPane*)m_wndStatusBar.GetExElement(0);
	m_pStatusPaneLabel->SetAlmostLargeText("00000000000000000000000000000000000000000000");
	m_pStatusPaneProcessing->SetAlmostLargeText("00000000000000000000000000000000000");
	m_pStatusPanePosition->SetAlmostLargeText("Pos: 10000000.000, 3000000000.000, 00000.000");
	m_pStatusPaneSysTime->SetAlmostLargeText("0000-00-00  00-00-00");
}
void CMainFrame::InitWndProgressBar()
{
	CRect RectStatus, RectSystemTime, RectProgress;
	m_wndStatusBar.GetClientRect(RectStatus);
	RectSystemTime = m_pStatusPaneSysTime->GetRect();
	RectProgress.right = max(RectStatus.left + 5, RectStatus.right - RectSystemTime.Width() - 3);
	RectProgress.left = max(RectProgress.right - 200, RectStatus.left + 1);
	RectProgress.top = RectSystemTime.top;
	RectProgress.bottom = RectSystemTime.bottom;

	if (!m_wndProgressBar.Create(WS_VISIBLE | WS_CHILD | PBS_SMOOTH, RectProgress, &m_wndStatusBar, 1))
	{
		TRACE0("Failed to create windows progressctrl\n");
		return;
	}
	m_wndProgressBar.ShowWindow(false);
	m_wndProgressBar.SetStartColor(RGB(255, 0, 0));
	m_wndProgressBar.SetEndColor(RGB(30, 144, 255));
	m_wndProgressBar.SetTextColor(RGB(255, 0, 0));
	m_wndProgressBar.SetRange(0, 100);
	m_wndProgressBar.SetPos(0);
}
void CMainFrame::ResetStatusPaneSize()
{
	CRect RectStatus, RectSystemTime, RectProgress, RectLabel, RectPos;
	m_wndStatusBar.GetClientRect(RectStatus);
	RectSystemTime = m_pStatusPaneSysTime->GetRect();
	RectProgress.right = max(RectStatus.left + 5, RectSystemTime.left - 10);
	RectProgress.left = max(RectProgress.right - 200, RectStatus.left + 1);
	RectProgress.top = RectSystemTime.top;
	RectProgress.bottom = RectSystemTime.bottom;
	m_wndProgressBar.MoveWindow(RectProgress);

	RectLabel = m_pStatusPaneLabel->GetRect();
	RectPos.left = max(RectLabel.right + 5, RectLabel.right + 10);
	RectPos.right = min(RectProgress.left + 300, RectStatus.right - 1);
	RectPos.top = RectSystemTime.top;
	RectPos.bottom = RectSystemTime.bottom;
	m_pStatusPanePosition->SetRect(RectPos);
	m_pStatusPanePosition->SetText("Pos: x = 0.0, y = 0.0, z = 0.0");
	m_pStatusPaneLabel->SetText("@2016 By Whu DpGrid Group");
	m_wndStatusBar.SetRedraw();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
	InitTieThresold(2.0);
	InitGcpThresold(2.0);


	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	InitStatusBar();
	SetTimer(TIMER_SYSTEM_TIME, 1000, 0);
	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 导航窗格将创建在左侧，因此将暂时禁用左侧的停靠: 
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

// 	// 创建标题栏: 
// 	if (!CreateCaptionBar())
// 	{
// 		TRACE0("未能创建标题栏\n");
// 		return -1;      // 未能创建
// 	}

	// 已创建 Outlook 栏，应允许在左侧停靠。
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}


	m_wndSourceData.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndSourceData);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

// 	m_wndChartView.EnableDocking(CBRS_ALIGN_ANY);
// 	DockPane(&m_wndChartView);
	m_wndImgViewLeft.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndImgViewLeft);
	m_wndImgViewRight.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndImgViewRight);


	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.DockToWindow(&m_wndSourceData, CBRS_BOTTOM);
	m_wndImgViewRight.DockToWindow(&m_wndImgViewLeft, CBRS_BOTTOM);
	m_wndSourceData.SetControlBarStyle(AFX_CBRS_RESIZE);   //设置窗口不可拖动
	m_wndProperties.SetControlBarStyle(AFX_CBRS_RESIZE);   //设置窗口不可拖动
	m_wndImgViewLeft.SetControlBarStyle(AFX_CBRS_RESIZE);   //设置窗口不可拖动
	m_wndImgViewRight.SetControlBarStyle(AFX_CBRS_RESIZE);   //设置窗口不可拖动
//	m_wndChartView.SetControlBarStyle(AFX_CBRS_RESIZE);  //设置窗口不可拖动
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	InitWndProgressBar();
	ResetStatusPaneSize();
	//	m_wndProgressBar.ShowWindow(true);
	m_bWndReady = true;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	m_strTitle = "DomQuaChkSys";
	cs.style &= ~FWS_ADDTOTITLE;
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	if (!m_wndOutput.Create("模型列表", this, CRect(0, 0, 100, 300), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}
	if (!m_wndSourceData.Create("资源窗口", this, CRect(0, 0, 300, 100), TRUE, ID_VIEW_SOURCEDATA, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}
	if (!m_wndProperties.Create("属性", this, CRect(0, 0, 300, 100), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}
	if (!m_wndImgViewLeft.Create("左影像", this, CRect(0, 0, 400, 100), TRUE, ID_VIEW_LEFTIMG, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}
	if (!m_wndImgViewRight.Create("右影像", this, CRect(0, 0, 400, 100), TRUE, ID_VIEW_RIGHTIMG, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}
// 	if (!m_wndChartView.Create("像点残差", this, CRect(0, 0, 300, 100), TRUE, ID_VIEW_CHART_PT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
// 	{
// 		TRACE0("未能创建“属性”窗口\n");
// 		return FALSE; // 未能创建
// 	}
	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("未能创建标题栏\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序
COLORREF CMainFrame::GetDomColor()
{
	CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(
		CMFCRibbonColorButton, m_wndRibbonBar.FindByID(1));
	return pColor->GetColor();
}
COLORREF CMainFrame::GetRefColor()
{
	CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(
		CMFCRibbonColorButton, m_wndRibbonBar.FindByID(1));
	return pColor->GetColor();
}
COLORREF CMainFrame::GetFillColor1()
{
	CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(
		CMFCRibbonColorButton, m_wndRibbonBar.FindByID(ID_BUTTON_COLOR1));
	return pColor->GetColor();
}
COLORREF CMainFrame::GetFillColor2()
{
	CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(
		CMFCRibbonColorButton, m_wndRibbonBar.FindByID(ID_BUTTON_COLOR2));
	return pColor->GetColor();
}
COLORREF CMainFrame::GetFillColor3()
{
	CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(
		CMFCRibbonColorButton, m_wndRibbonBar.FindByID(ID_BUTTON_COLOR3));
	return pColor->GetColor();
}
COLORREF CMainFrame::GetFillColor4()
{
	CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(
		CMFCRibbonColorButton, m_wndRibbonBar.FindByID(ID_BUTTON_COLOR4));
	return pColor->GetColor();
}
float CMainFrame::GetTieThresold()
{
	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(
		CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_TIE_THRESOLD));
	return atof(pEdit->GetEditText());
}
float CMainFrame::GetGcpThresold()
{
	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(
		CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_GCP_THRESOLD));
	return atof(pEdit->GetEditText());
}
void CMainFrame::InitTieThresold(float fvalue)
{
	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(
		CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_TIE_THRESOLD));
	CString strVal; strVal.Format("%.1f", fvalue);
	pEdit->SetEditText(strVal);
}
void CMainFrame::InitGcpThresold(float fvalue)
{
	CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(
		CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_GCP_THRESOLD));
	CString strVal; strVal.Format("%.1f", fvalue);
	pEdit->SetEditText(strVal);
}
bool CMainFrame::IsModelListFilled()
{
	return m_wndOutput.IsModelListFilled();
}
MstuMchModel CMainFrame::GetMchModel()
{
	return m_wndOutput.GetMchModel();
}
void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

// void CMainFrame::OnViewCaptionBar()
// {
// 	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
// 	RecalcLayout(FALSE);
// }
// 
// void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
// {
// 	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
// }

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 强制关闭“打印预览”模式
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Add your specialized code here and/or call the base class
	nCmdShow = SW_SHOWMAXIMIZED;
	CFrameWndEx::ActivateFrame(nCmdShow);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TIMER_SYSTEM_TIME:
		m_pStatusPaneSysTime->SetText(FunGetSysTimeStr());
		m_pStatusPaneSysTime->Redraw();
		break;
	case TIMER_MATCH_OVER:
	{
		CString strTmpFolder = FunGetDocHand()->m_DomChkPrj.GetMatchFolder() + "\\Model\\temp";
		vector<CString> vecTmpPath;
		FunSearchFile(strTmpFolder, "tmp", vecTmpPath);
		if (vecTmpPath.size() == FunGetDocHand()->m_DomChkPrj.GetTskNum())
		{
			/*for (int i = 0; i < vecTmpPath.size(); i++)
			{
			remove(vecTmpPath[i]);
			}
			remove(strTmpFolder);
			vector<CString>().swap(vecTmpPath);*/
			HANDLE hThread; DWORD IdThread;
			MatchCheckPara *pt = new MatchCheckPara((LPVOID)FunGetDocHand(),true);	
			hThread = CreateThread(NULL, 0,
				(LPTHREAD_START_ROUTINE)MultiThreadMatchCheck,
				(LPVOID *)pt, 0, &IdThread);
			KillTimer(TIMER_MATCH_OVER);
		}
		else
		{
			double dPos = vecTmpPath.size()*100.0 / FunGetDocHand()->m_DomChkPrj.GetTskNum();
			const char* strInfo = "Image matching";
			SendMessage(ID_MYMSG_PROGRESSBAR, (WPARAM)&dPos, (LPARAM)strInfo);
			vector<CString>().swap(vecTmpPath);
		}
	}
	default:
		break;
	}
	CFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);
	if (m_bWndReady) ResetStatusPaneSize();
	// TODO: Add your message handler code here
}


void CMainFrame::OnCheckViewProperitywnd()
{
	// TODO: Add your command handler code here
	if (m_wndProperties.IsVisible())
		m_wndProperties.ShowPane(FALSE, FALSE, TRUE);
	else m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateCheckViewProperitywnd(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndProperties.IsVisible());
}


void CMainFrame::OnCheckViewLeftimgwnd()
{
	// TODO: Add your command handler code here
	if (m_wndImgViewLeft.IsVisible())
		m_wndImgViewLeft.ShowPane(FALSE, FALSE, TRUE);
	else m_wndImgViewLeft.ShowPane(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateCheckViewLeftimgwnd(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndImgViewLeft.IsVisible());
}


void CMainFrame::OnCheckViewRightimgwnd()
{
	// TODO: Add your command handler code here	
	if (m_wndImgViewRight.IsVisible())
		m_wndImgViewRight.ShowPane(FALSE, FALSE, TRUE);
	else m_wndImgViewRight.ShowPane(TRUE, FALSE, TRUE);
}


void CMainFrame::OnUpdateCheckViewRightimgwnd(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code 

	pCmdUI->SetCheck(m_wndImgViewRight.IsVisible());
}

void CMainFrame::OnCheckViewSourcedatawnd()
{
	if (m_wndSourceData.IsVisible())
		m_wndSourceData.ShowPane(FALSE, FALSE, TRUE);
	else m_wndSourceData.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnUpdateCheckViewSourcedatawnd(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(m_wndSourceData.IsVisible());
}

void CMainFrame::OnCheckViewModelwnd()
{
	if (m_wndOutput.IsVisible())
		m_wndOutput.ShowPane(FALSE, FALSE, TRUE);
	else m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnUpdateCheckViewModelwnd(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(m_wndOutput.IsVisible());
}
