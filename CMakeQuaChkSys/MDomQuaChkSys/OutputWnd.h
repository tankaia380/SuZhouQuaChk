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

/////////////////////////////////////////////////////////////////////////////
// COutputList ����
#include "MyDockablePane.h"
#include "MyListCtrl.h"
#include "MyMFCTabCtrl.h"

class COutputWnd : public CMyDockablePane
{
// ����
public:
	COutputWnd();

	void UpdateFonts();

// ����	
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

// ʵ��
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

