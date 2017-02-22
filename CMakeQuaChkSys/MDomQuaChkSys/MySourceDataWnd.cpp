// MySourceDataWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "MySourceDataWnd.h"
#include "MainFrm.h"
#include "MDomQuaChkSysDoc.h"
// CMySourceDataWnd

IMPLEMENT_DYNAMIC(CMySourceDataWnd, CMyDockablePane)

CMySourceDataWnd::CMySourceDataWnd()
{

}

CMySourceDataWnd::~CMySourceDataWnd()
{
}



BEGIN_MESSAGE_MAP(CMySourceDataWnd, CMyDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK,ID_ITEM_IMGTREE,&OnNMClickImgTree)
END_MESSAGE_MAP()



// CMySourceDataWnd message handlers




int CMySourceDataWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMyDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy; rectDummy.SetRectEmpty();
	DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS /*| TVS_CHECKBOXES*/;
	m_ImgTree.Create(dwTreeStyle, rectDummy, this, ID_ITEM_IMGTREE);
//	m_ImgTree.InsertItem("Ӱ���б�");
	return 0;
}


void CMySourceDataWnd::OnSize(UINT nType, int cx, int cy)
{
	CMyDockablePane::OnSize(nType, cx, cy);
	m_ImgTree.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	// TODO: Add your message handler code here
}

void CMySourceDataWnd::OnNMClickImgTree(NMHDR * pNMHDR, LRESULT * pResult)
{
	CPoint point;
	GetCursorPos(&point);//����������λ��
	m_ImgTree.ScreenToClient(&point);//ת��Ϊ�ͻ�����

	UINT uFlags;
	HTREEITEM CurrentItem = m_ImgTree.HitTest(point, &uFlags); //��õ�ǰ����ڵ��ITEM
	HTREEITEM hParent = m_ImgTree.GetParentItem(CurrentItem);

	if (hParent == NULL) return;
	if (m_ImgTree.GetItemData(hParent) == -1) return;
	CString strImgPath;
	if (m_ImgTree.GetItemData(hParent) == -2)  //hDom
	{
		strImgPath = FunGetDocHand()->m_DomChkPrj.GetDomInfo(m_ImgTree.GetItemData(CurrentItem)).GetImgPath();
	}
	else if (m_ImgTree.GetItemData(hParent) == -3) //hRef
	{
		strImgPath = FunGetDocHand()->m_DomChkPrj.GetRefInfo(m_ImgTree.GetItemData(CurrentItem)).GetImgPath();
	}
	FunGetMainFrameHand()->SendMessage(ID_MYMSG_FILL_PROPLIST, (WPARAM)&strImgPath, 0);
}

void CMySourceDataWnd::FillImgTree(vector<CMImage4Check> vecDomInfo, vector<CMImage4Check> vecRefInfo)
{
	m_ImgTree.DeleteAllItems();
	if (vecDomInfo.size() + vecRefInfo.size() == 0) return;
	HTREEITEM hRoot = m_ImgTree.InsertItem("Ӱ���б�");
	m_ImgTree.SetItemData(hRoot, -1);

	if (vecDomInfo.size() > 0)
	{
		HTREEITEM hDomRoot = m_ImgTree.InsertItem("DOM�����Ʒ", hRoot);
		m_ImgTree.SetItemData(hDomRoot, -2);
		for (int i = 0; i < vecDomInfo.size(); i++)
		{
			HTREEITEM hDom = m_ImgTree.InsertItem(FunGetFileName(vecDomInfo[i].GetImgPath()), hDomRoot);
			m_ImgTree.SetItemData(hDom, i);
		}
		m_ImgTree.Expand(hDomRoot, TVE_EXPAND);
	}
	if (vecRefInfo.size() > 0)
	{
		HTREEITEM hRefRoot = m_ImgTree.InsertItem("REF�ο�Ӱ��", hRoot);
		m_ImgTree.SetItemData(hRefRoot, -3);
		for (int i = 0; i < vecRefInfo.size(); i++)
		{
			HTREEITEM hRef = m_ImgTree.InsertItem(FunGetFileName(vecRefInfo[i].GetImgPath()), hRefRoot);
			m_ImgTree.SetItemData(hRef, i);
			
		}
		m_ImgTree.Expand(hRefRoot, TVE_EXPAND);
	}
	m_ImgTree.Expand(hRoot, TVE_EXPAND);
}

