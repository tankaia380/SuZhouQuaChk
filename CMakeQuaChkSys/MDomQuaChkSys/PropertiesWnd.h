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
// ����
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

// ����
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CFont m_fntPropList;
	CMyMFCPropertyGridCtrl m_wndPropList;

// ʵ��
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

