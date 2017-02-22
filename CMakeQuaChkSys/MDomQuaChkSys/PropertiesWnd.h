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
#include "MyDockablePane.h"
#include "MyFunctions.h"
#include "MyMFCPropertyGridCtrl.h"
class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CMyDockablePane
{
// 构造
public:
	CPropertiesWnd();

	void AdjustLayout();
public:
	CMFCPropertyGridProperty* pGroupFile;
	CMFCPropertyGridProperty* pFileName;
	CMFCPropertyGridProperty* pFileSize;
	CMFCPropertyGridProperty* pImgSize;
	CMFCPropertyGridProperty* pImgHei;
	CMFCPropertyGridProperty* pImgWid;
	
	CMFCPropertyGridProperty* pGroupGeo;
	CMFCPropertyGridProperty* pCoordinate;
	CMFCPropertyGridProperty* pProjection;
	CMFCPropertyGridProperty* pImgRes;
	CMFCPropertyGridProperty* pImgRes_y;
	CMFCPropertyGridProperty* pImgRes_x;

// 特性
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CFont m_fntPropList;
	CMyMFCPropertyGridCtrl m_wndPropList;

// 实现
public:
	virtual ~CPropertiesWnd();
	void FillPropList(MstuPropOfImg stuPropInfo);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();

};

