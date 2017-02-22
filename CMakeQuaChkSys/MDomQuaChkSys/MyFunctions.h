#pragma once
#include "MDomQuaChk/MDomQuaChk.h"

class CMainFrame;
class CMDomQuaChkSysDoc;
class CMDomQuaChkSysView;

CMainFrame* FunGetMainFrameHand();
CMDomQuaChkSysDoc* FunGetDocHand();
CMDomQuaChkSysView* FunGetViewHand();

void FunVectorPt2VertexXY(void *pVector, int nDim, double *&pV);
void FunCreateVertexColor(int nPtNum, double r, double g, double b, double *&pC);
bool FunIntersectCRectD(CRectD &rd1, CRectD &rd2, CRectD &rd1_2);


int CallBakPrjPro(double dPos, const char* strInfo, void *pHand);