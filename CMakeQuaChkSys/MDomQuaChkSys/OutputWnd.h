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

#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口
#include "MyDockablePane.h"
#include "MyListCtrl.h"
#include "MyMFCTabCtrl.h"

class COutputWnd : public CMyDockablePane
{
// 构造
public:
	COutputWnd();

	void UpdateFonts();

// 特性	
	void FillListTieModel(vector<MstuTieChkModel> vecTieModel);
	void FillListGcpModel(vector<MstuGcpChkModel> vecGcpModel);
	void FillListPoint(MstuMchModel Model);
	bool IsModelListFilled()
	{
		if (m_ListModel.GetItemCount() > 0) return true;
		return false;
	}
	MstuMchModel GetMchModel() {
		return m_wndTabs.m_stuPointChkInfo;
	}
protected:
	CMyMFCTabCtrl	m_wndTabs;
	CMyListCtrl m_ListModel;
	CMyListCtrl m_ListPoint;
	bool m_bIsModelListInit;
	bool m_bIsPointListInit;
	int  m_nSelectedModel;


protected:
	void InitListModel(MenmModelType eType);
	void InitListPoint();

// 实现
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
// 	afx_msg void OnDClickModelList(NMHDR *pNMHDR, LRESULT *pResult);
// 	afx_msg void OnDClickPointList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

