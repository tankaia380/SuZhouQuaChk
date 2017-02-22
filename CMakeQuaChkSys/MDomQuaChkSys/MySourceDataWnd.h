#pragma once
#include "MyDockablePane.h"
#include "MyTreeCtrl.h"
#include <vector>
#include "MyFunctions.h"
using namespace std;
// CMySourceDataWnd

class CMySourceDataWnd : public CMyDockablePane
{
	DECLARE_DYNAMIC(CMySourceDataWnd)

public:
	CMySourceDataWnd();
	virtual ~CMySourceDataWnd();
	void FillImgTree(vector<CMImage4Check> vecDomInfo, vector<CMImage4Check> vecRefInfo);
protected:
	CMyTreeCtrl  m_ImgTree;

	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickImgTree(NMHDR *pNMHDR, LRESULT *pResult);
};


