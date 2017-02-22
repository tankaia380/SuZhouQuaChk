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

#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MDomQuaChkSysDoc.h"
#include "MDomQuaChkSysView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
	m_bIsModelListInit = false;
	m_bIsPointListInit = false;
	m_nSelectedModel = 0;
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CMyDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}
	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL| LVS_OWNERDATA;

	if (!m_ListModel.Create(dwStyle, rectDummy, &m_wndTabs, IDC_LIST_MODEL) ||
		!m_ListPoint.Create(dwStyle, rectDummy, &m_wndTabs, IDC_LIST_POINT))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	UpdateFonts();

	// Attach list windows to tab:
	m_wndTabs.AddTab(&m_ListModel, "质检单元", (UINT)0);
	m_wndTabs.AddTab(&m_ListPoint, "检查点", (UINT)1);
	InitListModel(MODEL_TIE);
	InitListPoint();
// 	vector<MstuTieChkModel> vecTieModel(2);
// 	FillListTieModel(vecTieModel);

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CMyDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区: 

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndTabs.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::FillListTieModel(vector<MstuTieChkModel> vecTieModel)
{
	//	m_ListModel.DeleteAllItems();
	if (FunGetViewHand()->m_vecSelectModelFromView.size() == 0) m_nSelectedModel == 0;   //add by tk
	InitListModel(MODEL_TIE);
	m_wndTabs.m_vecTieChkModelInfo = vecTieModel;
	m_ListModel.SetItemCountEx(m_wndTabs.m_vecTieChkModelInfo.size());

	m_ListModel.Invalidate();
}

void COutputWnd::FillListGcpModel(vector<MstuGcpChkModel> vecGcpModel)
{
//	m_ListModel.DeleteAllItems();
	if (FunGetViewHand()->m_vecSelectModelFromView.size() == 0) m_nSelectedModel == 0;   //add by tk
	InitListModel(MODEL_CTL);
	m_wndTabs.m_vecGcpChkModelInfo = vecGcpModel;
	m_ListModel.SetItemCountEx(m_wndTabs.m_vecGcpChkModelInfo.size());
	m_ListModel.Invalidate();
}

void COutputWnd::FillListPoint(MstuMchModel Model)
{
//	m_ListPoint.DeleteAllItems();
	InitListPoint();
	m_ListPoint.SetItemCountEx(Model.nPtValid);
	m_ListPoint.Invalidate();

}

void COutputWnd::InitListModel(MenmModelType eType)
{
	if (m_bIsModelListInit)
	{
		int nCols = m_ListModel.GetHeaderCtrl()->GetItemCount();
		for (int i = 0; i < nCols; i++)
		{
			m_ListModel.DeleteColumn(0);
		}
	}
	switch (eType)
	{
	case MODEL_TIE:
	{
		vector<MstuTieChkModel>().swap(m_wndTabs.m_vecTieChkModelInfo);
		m_ListModel.ModifyStyle(0, LVS_REPORT);
		m_ListModel.ModifyStyle(0, LVS_SHOWSELALWAYS);
		m_ListModel.ModifyStyle(0, LVS_SINGLESEL);
		DWORD dwStyle = m_ListModel.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;     //可以选择整行
		dwStyle |= LVS_EX_GRIDLINES;         //格网形式 
		dwStyle |= LVS_EX_DOUBLEBUFFER;      //双缓冲

		dwStyle |= LVS_EX_CHECKBOXES;
		m_ListModel.SetExtendedStyle(dwStyle);

		int nListWidth = 1200;
		m_ListModel.InsertColumn(0, "nIdx", LVCFMT_LEFT, (float)nListWidth / 20, -1);
		m_ListModel.InsertColumn(1, "DomL_ImgName", LVCFMT_LEFT, (float)nListWidth / 6, -1);
		m_ListModel.InsertColumn(2, "DomR_ImgName", LVCFMT_LEFT, (float)nListWidth / 6, -1);
		m_ListModel.InsertColumn(3, "Point_Num", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(4, "Mean_Dx", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(5, "Mean_Dy", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(6, "Mean_Dxy", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(7, "Mse_X", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(8, "Mse_Y", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(9, "Mse_XY", LVCFMT_LEFT, (float)nListWidth / 12, -1);
	}
	break;
	case MODEL_CTL: {
		vector<MstuGcpChkModel>().swap(m_wndTabs.m_vecGcpChkModelInfo);
		m_ListModel.ModifyStyle(0, LVS_REPORT);
		m_ListModel.ModifyStyle(0, LVS_SHOWSELALWAYS);
		//m_ListModel.ModifyStyle(0, LVS_SINGLESEL);
//		m_ListModel.ModifyStyle(0, LVS_OWNERDATA);
//		m_ListModel.ModifyStyle(0, LVS_OWNERDRAWFIXED);
		DWORD dwStyle = m_ListModel.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;     //可以选择整行
		dwStyle |= LVS_EX_GRIDLINES;         //格网形式 
		dwStyle |= LVS_EX_DOUBLEBUFFER;      //双缓冲

		dwStyle |= LVS_EX_CHECKBOXES;

		m_ListModel.SetExtendedStyle(dwStyle);
		int nListWidth = 1000;
		m_ListModel.InsertColumn(0, "nIdx", LVCFMT_LEFT, (float)nListWidth / 20, -1);
		m_ListModel.InsertColumn(1, "Dom_ImgName", LVCFMT_LEFT, (float)nListWidth / 6, -1);
		m_ListModel.InsertColumn(2, "Point_Num", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(3, "Mean_Dx", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(4, "Mean_Dy", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(5, "Mean_Dxy", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(6, "Mse_X", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(7, "Mse_Y", LVCFMT_LEFT, (float)nListWidth / 12, -1);
		m_ListModel.InsertColumn(8, "Mse_XY", LVCFMT_LEFT, (float)nListWidth / 12, -1);
	}
					break;
	default:
		break;
	}

	m_bIsModelListInit = true;
}

void COutputWnd::InitListPoint()
{
	if (m_bIsPointListInit)
	{
		int nCols = m_ListPoint.GetHeaderCtrl()->GetItemCount();
		for (int i = 0; i < nCols; i++)
		{
			m_ListPoint.DeleteColumn(0);
		}
	}
//	m_wndTabs.m_stuPointChkInfo.Clear();
	m_ListPoint.ModifyStyle(0, LVS_REPORT);
	m_ListPoint.ModifyStyle(0, LVS_SHOWSELALWAYS);
	m_ListPoint.ModifyStyle(0, LVS_SINGLESEL);
//	m_ListPoint.ModifyStyle(0, LVS_OWNERDATA);
//	m_ListPoint.ModifyStyle(0, LVS_OWNERDRAWFIXED);
	DWORD dwStyle = m_ListPoint.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;     //可以选择整行
	dwStyle |= LVS_EX_GRIDLINES;         //格网形式 
	dwStyle |= LVS_EX_DOUBLEBUFFER;      //双缓冲
// 		m_ListPoint.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_OWNERDRAWFIXED |
// 			LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE | LVS_EX_FLATSB);
	m_ListPoint.SetExtendedStyle(dwStyle);
	int nListWidth = 1000;
	m_ListPoint.InsertColumn(0, "nIdx", LVCFMT_LEFT, (float)nListWidth / 20, -1);
	m_ListPoint.InsertColumn(1, "PtId", LVCFMT_LEFT, (float)nListWidth / 10, -1);
	m_ListPoint.InsertColumn(2, "Left_px", LVCFMT_LEFT, (float)nListWidth / 10, -1);
	m_ListPoint.InsertColumn(3, "Left_py", LVCFMT_LEFT, (float)nListWidth / 10, -1);
	m_ListPoint.InsertColumn(4, "Right_px", LVCFMT_LEFT, (float)nListWidth / 10, -1);
	m_ListPoint.InsertColumn(5, "Right_py", LVCFMT_LEFT, (float)nListWidth / 10, -1);
	m_ListPoint.InsertColumn(6, "Dx", LVCFMT_LEFT, (float)nListWidth / 10, -1);
	m_ListPoint.InsertColumn(7, "Dy", LVCFMT_LEFT, (float)nListWidth / 10, -1);
	m_ListPoint.InsertColumn(8, "Dxy", LVCFMT_LEFT, (float)nListWidth / 10, -1);
//	m_ListPoint.InsertColumn(9, "Seed_Pt", LVCFMT_LEFT, (float)nListWidth / 15, -1);

	m_bIsPointListInit = TRUE;
}

void COutputWnd::UpdateFonts()
{
	m_ListModel.SetFont(&afxGlobalData.fontRegular);
	m_ListPoint.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1


BOOL COutputWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CMyDockablePane::OnEraseBkgnd(pDC);
}

// void COutputWnd::OnDClickModelList(NMHDR * pNMHDR, LRESULT * pResult)
// {
// 	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
// 	int nRowIdx = pNMListView->iItem;
// 	if (nRowIdx < 0) return;
// 	int nModelIdx = nRowIdx;
// 	m_wndTabs.SetActiveTab(1);
// 	MstuMchModel model;
// 	model = FunGetDocHand()->m_DomChkPrj.GetCheckModelPoint(nModelIdx, FunGetDocHand()->m_eModelType);
// 	vector<int> vecSelectedDomIdx; 
// 	vecSelectedDomIdx.push_back(model.nImgIdx1); vecSelectedDomIdx.push_back(model.nImgIdx2);
// 	FunGetViewHand()->SendMessage(ID_MYMSG_SELECTEDRENDER, (WPARAM)&vecSelectedDomIdx, 0);
// 	vector<int>().swap(vecSelectedDomIdx);
// 	FillListPoint(model);
// 
// }
// 
// void COutputWnd::OnDClickPointList(NMHDR * pNMHDR, LRESULT * pResult)
// {
// 
// 	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
// 	int nRowIdx = pNMListView->iItem;
// 	if (nRowIdx < 0) return;
// 	int nModelIdx = nRowIdx;
// }


BOOL COutputWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->wParam == VK_TAB&&pMsg->message == WM_KEYDOWN)
	{
		int nWinPos = m_wndTabs.GetActiveTab();
		int nShowItm = 0;
		
		if (nWinPos == 0)
		{
			if (FunGetViewHand()->m_vecSelectModelFromView.size() == 0) return false;
			if (m_nSelectedModel >= FunGetViewHand()->m_vecSelectModelFromView.size())
			{
				AfxMessageBox("已搜索到达文件末尾!");
				m_nSelectedModel = 0;
				return CMyDockablePane::PreTranslateMessage(pMsg);
			}
			nShowItm = FunGetViewHand()->m_vecSelectModelFromView[m_nSelectedModel];
			if (nShowItm >= m_ListModel.GetItemCount()) return false;

			UINT nState = m_ListModel.GetItemState(nShowItm, -1);
			nState |= LVIS_FOCUSED | LVIS_SELECTED;
			m_ListModel.SetItemState(nShowItm, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
			m_ListModel.EnsureVisible(nShowItm, true);
			m_nSelectedModel++;
		}
		else if (nWinPos==1)
		{
			if (FunGetMainFrameHand()->m_nSelectedPoint > 0 && FunGetMainFrameHand()->m_nSelectedPoint < m_ListPoint.GetItemCount())
			{
				UINT nState = m_ListPoint.GetItemState(FunGetMainFrameHand()->m_nSelectedPoint, -1);
				nState |= LVIS_FOCUSED | LVIS_SELECTED;
				m_ListPoint.SetItemState(FunGetMainFrameHand()->m_nSelectedPoint, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
				m_ListPoint.EnsureVisible(FunGetMainFrameHand()->m_nSelectedPoint, true);
			}
		}
	}
	return CMyDockablePane::PreTranslateMessage(pMsg);
}
