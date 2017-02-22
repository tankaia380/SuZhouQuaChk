// MyMFCTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "MyMFCTabCtrl.h"
#include "OutputWnd.h"
#include "MainFrm.h"
#include "MDomQuaChkSysDoc.h"
#include "MDomQuaChkSysView.h"
// CMyMFCTabCtrl

IMPLEMENT_DYNAMIC(CMyMFCTabCtrl, CMFCTabCtrl)

CMyMFCTabCtrl::CMyMFCTabCtrl()
{

}

CMyMFCTabCtrl::~CMyMFCTabCtrl()
{
}




BEGIN_MESSAGE_MAP(CMyMFCTabCtrl, CMFCTabCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MODEL, &OnDClickModelList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_POINT, &OnDClickPointList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MODEL, &OnNClickModelList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MODEL, &OnGetdispinfoListModelinfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_POINT, &OnGetdispinfoListPointinfo)
END_MESSAGE_MAP()

// CMyMFCTabCtrl message handlers



void CMyMFCTabCtrl::OnDClickModelList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//	((COutputWnd*)GetParent())->OnDClickModelList(pNMHDR, pResult);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nRowIdx = pNMListView->iItem;
	if (nRowIdx < 0) return;
	int nModelIdx = nRowIdx;

	vector<int> vecSelectedDomIdx = FunGetDocHand()->m_DomChkPrj.GetModelImgIdx(nModelIdx, FunGetDocHand()->m_eModelType);
	m_stuPointChkInfo.Clear();
	m_stuPointChkInfo = FunGetDocHand()->m_DomChkPrj.GetCheckModelPoint(nModelIdx, FunGetDocHand()->m_eModelType);
	((COutputWnd*)GetParent())->FillListPoint(m_stuPointChkInfo);
	FunGetDocHand()->m_pChartDlg->SendMessage(ID_MYMSG_FILLCHART, (WPARAM)&m_stuPointChkInfo, 0);
	FunGetViewHand()->SendMessage(ID_MYMSG_MODELPOINTRENDER, (WPARAM)&m_stuPointChkInfo, 0);
	FunGetMainFrameHand()->SendMessage(ID_MYMSG_MODELIMGRENDER, (WPARAM)&vecSelectedDomIdx, (LPARAM)&m_stuPointChkInfo);
	vector<int>().swap(vecSelectedDomIdx);
}

void CMyMFCTabCtrl::OnDClickPointList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//	((COutputWnd*)GetParent())->OnDClickPointList(pNMHDR, pResult);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nRowIdx = pNMListView->iItem;
	if (nRowIdx < 0) return;
	int nPointIdx = nRowIdx;

	vector<Point2D> ptSelectedLR;
	ptSelectedLR.push_back(m_stuPointChkInfo.vecObjPtXY[nPointIdx]);

	Point2D ptTemp;
	if (m_stuPointChkInfo.eType == MODEL_TIE)
	{
		ptTemp = FunGetDocHand()->m_DomChkPrj.GetDomInfo(m_stuPointChkInfo.nImgIdx2).ImgPt2Geo(m_stuPointChkInfo.vecImgPt2[nPointIdx]);
	}
	else if (m_stuPointChkInfo.eType == MODEL_CTL)
	{
		CString strTemp = m_stuPointChkInfo.vecPtIdx[nPointIdx].c_str();
		vector<CString> vParts = FunStringTok(strTemp, "_");
		if (vParts.size() < 2) return;
		CMImage4Check DomImgChk = FunGetDocHand()->m_DomChkPrj.GetRefInfo(atoi(vParts[2]));
		ptTemp = DomImgChk.ImgPt2Geo(m_stuPointChkInfo.vecImgPt2[nPointIdx]);

		int nRefIdx = atoi(vParts[2]);
		CString strRefPath = FunGetDocHand()->m_DomChkPrj.GetRefInfo(nRefIdx).GetImgPath();
		if (FunGetMainFrameHand()->m_wndImgViewRight.m_strImgPath != strRefPath)
		{
			vector<Point2D> vecPointR;
			for (int ciri = 0; ciri < m_stuPointChkInfo.vecImgPt2.size(); ++ciri)
			{
				CString strTemp = m_stuPointChkInfo.vecPtIdx[ciri].c_str();
				vector<CString> vParts = FunStringTok(strTemp, "_");
				if (vParts.size() < 2) continue;
				if (atoi(vParts[2]) == nRefIdx)
				{
					Point2D ptTemp = DomImgChk.ImgPt2Geo(m_stuPointChkInfo.vecImgPt2[ciri]);
					vecPointR.push_back(ptTemp);
				}
				else
				{
					Point2D ptTemp = DomImgChk.ImgPt2Geo(m_stuPointChkInfo.vecImgPt2[0]);
					vecPointR.push_back(ptTemp);
				}
			}
			FunGetMainFrameHand()->m_wndImgViewRight.SendMessage(ID_MYMSG_IMGRENDER, (WPARAM)&strRefPath, (LPARAM)&vecPointR);
		}
	}
	else
	{
		return;
	}
	ptSelectedLR.push_back(ptTemp);

	//Point2D ptSelected = m_stuPointChkInfo.vecObjPtXY[nPointIdx];   
	CString strPtIdx = m_stuPointChkInfo.vecPtIdx[nPointIdx].c_str();
	FunGetMainFrameHand()->SendMessage(ID_MYMSG_SELECTPOINT, (WPARAM)&ptSelectedLR,(LPARAM)&strPtIdx);
}

void CMyMFCTabCtrl::OnNClickModelList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//	((COutputWnd*)GetParent())->OnDClickPointList(pNMHDR, pResult);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nRowIdx = pNMListView->iItem;
	if (nRowIdx < 0) return;
	int nModelIdx = nRowIdx;
	vector<int> vecSelectedDomIdx = FunGetDocHand()->m_DomChkPrj.GetModelImgIdx(nModelIdx, FunGetDocHand()->m_eModelType);
	FunGetViewHand()->SendMessage(ID_MYMSG_SELECTEDRENDER, (WPARAM)&vecSelectedDomIdx, (LPARAM)& FunGetDocHand()->m_eModelType);
	vector<int>().swap(vecSelectedDomIdx);
}

void CMyMFCTabCtrl::OnGetdispinfoListModelinfo(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMLVDISPINFO *pGetInfoTip = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	char strInfo[100];
	char strId[20];
	LV_ITEM* pItem = &(pGetInfoTip)->item;
	switch (FunGetDocHand()->m_eModelType)
	{
	case MODEL_TIE:
	{
		if (m_vecTieChkModelInfo.size() == 0) break;
		if (pItem->mask & LVIF_TEXT)
		{
			MstuTieChkModel TieModel = m_vecTieChkModelInfo[pItem->iItem];

			switch (pItem->iSubItem)
			{
			case 0:
				sprintf(strId, "%d", pItem->iItem);
				lstrcpyn(pItem->pszText, strId, pItem->cchTextMax);
				break;
			case 1:
				sprintf(strInfo, "%s", FunGetFileName(TieModel.strDomPath1, true));
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 2:
				sprintf(strInfo, "%s", FunGetFileName(TieModel.strDomPath2, true));
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 3:
				sprintf(strInfo, "%d", TieModel.nPtNum);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 4:
				sprintf(strInfo, "%lf", TieModel.dMeanDx);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 5:
				sprintf(strInfo, "%lf", TieModel.dMeanDy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 6:
				sprintf(strInfo, "%lf", TieModel.dMeanDxy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 7:
				sprintf(strInfo, "%lf", TieModel.dMseDx);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 8:
				sprintf(strInfo, "%lf", TieModel.dMseDy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 9:
				sprintf(strInfo, "%lf", TieModel.dMseDxy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			default:
				break;
			}
		}
	}
		break;
	case MODEL_CTL:
	{
		if (m_vecGcpChkModelInfo.size() == 0) break;
		if (pItem->mask & LVIF_TEXT)
		{
			MstuGcpChkModel GcpModel = m_vecGcpChkModelInfo[pItem->iItem];
			switch (pItem->iSubItem)
			{
			case 0:
				sprintf(strId, "%d", pItem->iItem);
				lstrcpyn(pItem->pszText, strId, pItem->cchTextMax);
				break;
			case 1:
				sprintf(strInfo, "%s", FunGetFileName(GcpModel.strDomPath, true));
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 2:
				sprintf(strInfo, "%d", GcpModel.nPtNum);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 3:
				sprintf(strInfo, "%lf", GcpModel.dMeanDx);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 4:
				sprintf(strInfo, "%lf", GcpModel.dMeanDy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 5:
				sprintf(strInfo, "%lf", GcpModel.dMeanDxy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 6:
				sprintf(strInfo, "%lf", GcpModel.dMseDx);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 7:
				sprintf(strInfo, "%lf", GcpModel.dMseDy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			case 8:
				sprintf(strInfo, "%lf", GcpModel.dMseDxy);
				lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
				break;
			default:
				break;
			}
		}
	}
	}

	pItem->mask |= LVIF_STATE;
	pItem->stateMask = LVIS_STATEIMAGEMASK;
	if (FunGetViewHand()->m_vecSelectModelFromView.size())
	{
		bool bHasSelected = false;
		for (int ciri = 0; ciri < FunGetViewHand()->m_vecSelectModelFromView.size(); ++ciri)
		{
			if (pItem->iItem == FunGetViewHand()->m_vecSelectModelFromView[ciri])
			{
				bHasSelected = true;
				break;
			}
		}
		if (bHasSelected) pItem->state |= INDEXTOSTATEIMAGEMASK(2);   //选中
		else pItem->state |= INDEXTOSTATEIMAGEMASK(1);   //未选中
	}
	else
	{
		pItem->state |= INDEXTOSTATEIMAGEMASK(1);   //未选中
	}
	*pResult = 0;
}

void CMyMFCTabCtrl::OnGetdispinfoListPointinfo(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMLVDISPINFO *pGetInfoTip = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	char strInfo[100];
	char strId[20];
	LV_ITEM* pItem = &(pGetInfoTip)->item;
	if (m_stuPointChkInfo.nPtValid == 0) return;
	if (pItem->mask & LVIF_TEXT)
	{
		switch (pItem->iSubItem)
		{
		case 0:
			sprintf(strId, "%d", pItem->iItem);
			lstrcpyn(pItem->pszText, strId, pItem->cchTextMax);
			break;
		case 1:
			lstrcpyn(pItem->pszText, m_stuPointChkInfo.vecPtIdx[pItem->iItem].c_str(), pItem->cchTextMax);
			break;
		case 2:
			sprintf(strInfo, "%lf", m_stuPointChkInfo.vecImgPt1[pItem->iItem].x);
			lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
			break;
		case 3:
			sprintf(strInfo, "%lf", m_stuPointChkInfo.vecImgPt1[pItem->iItem].y);
			lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
			break;
		case 4:
			sprintf(strInfo, "%lf", m_stuPointChkInfo.vecImgPt2[pItem->iItem].x);
			lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
			break;
		case 5:
			sprintf(strInfo, "%lf", m_stuPointChkInfo.vecImgPt2[pItem->iItem].y);
			lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
			break;
		case 6:
			sprintf(strInfo, "%lf", m_stuPointChkInfo.vecPtResxy[pItem->iItem].x);
			lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
			break;
		case 7:
			sprintf(strInfo, "%lf", m_stuPointChkInfo.vecPtResxy[pItem->iItem].y);
			lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
			break;
		case 8:
			sprintf(strInfo, "%lf", sqrt(m_stuPointChkInfo.vecPtResxy[pItem->iItem].x *m_stuPointChkInfo.vecPtResxy[pItem->iItem].x +
				m_stuPointChkInfo.vecPtResxy[pItem->iItem].y*m_stuPointChkInfo.vecPtResxy[pItem->iItem].y));
			lstrcpyn(pItem->pszText, strInfo, pItem->cchTextMax);
			break;
		default:
			break;
		}
	}
	*pResult = 0;
}
