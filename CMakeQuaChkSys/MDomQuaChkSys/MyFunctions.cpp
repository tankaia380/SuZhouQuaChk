#include "stdafx.h"
#include "MyFunctions.h"
#include "MainFrm.h"
#include "MDomQuaChkSysDoc.h"
#include "MDomQuaChkSysView.h"
  
CMainFrame * FunGetMainFrameHand()
{
	return (CMainFrame *)(CFrameWndEx *)AfxGetMainWnd();
}

CMDomQuaChkSysDoc * FunGetDocHand()
{
	CMainFrame* pMain = FunGetMainFrameHand();
	if (pMain == NULL) return NULL;
	else return (CMDomQuaChkSysDoc*)pMain->GetActiveDocument();
}

CMDomQuaChkSysView * FunGetViewHand()
{
	CMainFrame* pMain = FunGetMainFrameHand();
	if (pMain == NULL) return NULL;
	else return (CMDomQuaChkSysView*)pMain->GetActiveView();
}

void FunVectorPt2VertexXY(void *pVector, int nDim, double *&pV)
{
	if (nDim == 2)
	{
		vector<Point2D> vecPt2d = *(vector<Point2D>*)pVector;
		for (int i = 0; i < vecPt2d.size(); i++)
		{
			*(pV + 2 * i + 0) = vecPt2d[i].x;
			*(pV + 2 * i + 1) = vecPt2d[i].y;
		}
		// 		*(pV+2*vecPt2d.size()+0) = vecPt2d[0].x;
		// 		*(pV+2*vecPt2d.size()+1) = vecPt2d[0].y;
		vector<Point2D>().swap(vecPt2d);
	}
	else if (nDim == 3)
	{
		vector<Point3D> vecPt3d = *(vector<Point3D>*)pVector;
		for (int i = 0; i < vecPt3d.size(); i++)
		{
			*(pV + 2 * i + 0) = vecPt3d[i].X;
			*(pV + 2 * i + 1) = vecPt3d[i].Y;
		}
		// 		*(pV+2*vecPt3d.size()+0) = vecPt3d[0].X;
		// 		*(pV+2*vecPt3d.size()+1) = vecPt3d[0].Y;
		vector<Point3D>().swap(vecPt3d);
	}
}
void FunCreateVertexColor(int nPtNum, double r, double g, double b, double *&pC)
{
	for (int i = 0; i < nPtNum; i++)
	{
		*(pC + i * 4 + 0) = r;
		*(pC + i * 4 + 1) = g;
		*(pC + i * 4 + 2) = b;
		*(pC + i * 4 + 3) = 255;
	}
}
bool FunIntersectCRectD(CRectD &rd1, CRectD &rd2, CRectD &rd1_2)  //¾ØÐÎ¿òÇó½»
{
	double l, t, r, b;
	l = max(rd1.left, rd2.left);
	r = min(rd1.right, rd2.right);
	if (r < l) return false;
	if (rd1.top >= rd1.bottom)
	{
		t = min(rd1.top, rd2.top);
		b = max(rd1.bottom, rd2.bottom);
		if (t < b) return false;
	}
	if (rd1.top < rd1.bottom)
	{
		t = max(rd1.top, rd2.top);
		b = min(rd1.bottom, rd2.bottom);
		if (t > b) return false;
	}
	rd1_2 = CRectD(l, t, r, b);
	return true;
}

int CallBakPrjPro(double dPos, const char* strInfo, void *pHand)
{
	CMainFrame* pMain = (CMainFrame*) pHand;
    if(pMain==NULL) return 0;
	pMain->SendMessage(ID_MYMSG_PROGRESSBAR,(WPARAM)&dPos,(LPARAM)strInfo);
    return 1;
}