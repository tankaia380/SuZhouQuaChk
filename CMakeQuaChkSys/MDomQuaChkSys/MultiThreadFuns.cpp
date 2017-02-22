#include "stdafx.h"
#include "MultiThreadFuns.h"
#include "MainFrm.h"
#include "MDomQuaChkSysDoc.h"
#include "MDomQuaChkSysView.h"
#include "MyFunctions.h"

void WINAPI MultiThreadImgPrePro(LPVOID p)
{
	CMDomQuaChkSysDoc *pDoc = (CMDomQuaChkSysDoc*)p;
	bool bShow = true;
	pDoc->m_bIsRunning = true;
	pDoc->m_pMain->SendMessage(ID_MYMSG_SHOWPROGRESSBAR, (WPARAM)&bShow);
	CallBakPrjPro(1, "Checking images", pDoc->m_pMain);

	for (int i = 0; i < pDoc->m_vecDomInfo.size(); i++)
	{
		CString STRTEMP = pDoc->m_vecDomInfo[i].GetImgPath();
		if (PathFileExists(STRTEMP))
		{
			if (pDoc->m_vecDomInfo[i].OpenImage(STRTEMP))
			{
				if (pDoc->m_vecDomInfo[i].GetGeoInfo().eProjInfo == PROJECT_NONE)
				{
					pDoc->m_vecDomInfo.erase(pDoc->m_vecDomInfo.begin() + i);
					i--;
				}
			}
			else
			{
				pDoc->m_vecDomInfo.erase(pDoc->m_vecDomInfo.begin() + i);
				i--;
			}
		}
		else
		{
			pDoc->m_vecDomInfo.erase(pDoc->m_vecDomInfo.begin() + i);
			i--;
		}
		CallBakPrjPro((i+1)*100.0/(pDoc->m_vecDomInfo.size()+pDoc->m_vecRefInfo.size()),
			"Checking images", pDoc->m_pMain);
	}
	for (int i = 0; i < pDoc->m_vecRefInfo.size(); i++)
	{
		CString STRTEMP = pDoc->m_vecRefInfo[i].GetImgPath();
		if (PathFileExists(STRTEMP))
		{
			if (pDoc->m_vecRefInfo[i].OpenImage(STRTEMP))
			{
				if (pDoc->m_vecRefInfo[i].GetGeoInfo().eProjInfo == PROJECT_NONE)
				{
					pDoc->m_vecRefInfo.erase(pDoc->m_vecRefInfo.begin() + i);
					i--;
				}
			}
			else
			{
				pDoc->m_vecRefInfo.erase(pDoc->m_vecRefInfo.begin() + i);
				i--;
			}
		}
		else
		{
			pDoc->m_vecRefInfo.erase(pDoc->m_vecRefInfo.begin() + i);
			i--;
		}
		CallBakPrjPro((i + 1+ pDoc->m_vecDomInfo.size())*100.0 / (pDoc->m_vecDomInfo.size() + pDoc->m_vecRefInfo.size()),
			"Checking images", pDoc->m_pMain);
	}

	if (pDoc->m_vecDomInfo.size() == 0) return;
    pDoc->m_DomChkPrj.SendData2Prj(pDoc->m_vecDomInfo, pDoc->m_vecRefInfo);
	pDoc->m_DomChkPrj.SavePrj2File(pDoc->m_strPtjFilePath);
	pDoc->m_pMain->SendMessage(ID_MYMSG_FILL_IMGTREE, (WPARAM)&pDoc->m_vecDomInfo, (LPARAM)&pDoc->m_vecRefInfo);
	pDoc->m_DomChkPrj.BulidPyramids(false, CallBakPrjPro, pDoc->m_pMain);
	pDoc->m_DomChkPrj.CatchTfwInfo(true, CallBakPrjPro, pDoc->m_pMain);
	pDoc->m_DomChkPrj.SavePrj2File(pDoc->m_strPtjFilePath);
	pDoc->m_DomChkPrj.BuildColorMap(false, true, CallBakPrjPro, pDoc->m_pMain);
	
	pDoc->m_pView->SendMessage(ID_MYMSG_INIT_IMGRENDER,0,0);
	/*pDoc->m_bIsRunning = false;*/
	pDoc->m_bDataReady = true;
	pDoc->m_pView->PostMessage(ID_MYMSG_SAVECQVIEW2IMAGE);

	pDoc->m_DomChkPrj.ImageMatch();   //提前准备影像配准文件tsk等文件路径

	if (pDoc->m_DomChkPrj.fIsTiemodelAndGcpExist() &&
		AfxMessageBox("检测到模型文件以及GCP文件均已存在，是否直接载入?", MB_YESNO) == IDYES)
	{
		pDoc->m_DomChkPrj.LoadModelAndGcpFile(CallBakPrjPro, pDoc->m_pMain);
		MatchCheckPara *pt = new MatchCheckPara(pDoc, false);
		MultiThreadMatchCheck((LPVOID)pt);
	}
	else
	{
		bShow = false;
		CallBakPrjPro(0, "", pDoc->m_pMain);
		pDoc->m_pMain->SendMessage(ID_MYMSG_SHOWPROGRESSBAR, (WPARAM)&bShow, 0);
	}
	pDoc->m_bIsRunning = false;
}

void WINAPI MultiThreadSavePointShpFiles(LPVOID p)
{
	//output tie check model point shp	
	CMDomQuaChkSysDoc *pDoc = (CMDomQuaChkSysDoc*)p;
	bool bShow = true;
	pDoc->m_pMain->SendMessage(ID_MYMSG_SHOWPROGRESSBAR, (WPARAM)&bShow);
	CallBakPrjPro(1, "Saving point shape files", pDoc->m_pMain);


	CString str = pDoc->m_strReportPath.Left(pDoc->m_strReportPath.ReverseFind('\\')) + "\\PointShp\\";
	FunCreateDir4Path(str);
	vector<MstuTieChkModel> VecTieChkModelInfo = pDoc->m_DomChkPrj.GetTieModelInfo();
	CMImage4Check saveshp;
	MstuMchModel model;
	CString strShpPath;
	int nStart, nEnd;
	for (int ciri = 0; ciri < VecTieChkModelInfo.size(); ciri++)
	{
		model = pDoc->m_DomChkPrj.GetCheckModelPoint(ciri, MODEL_TIE);
		strShpPath = pDoc->m_DomChkPrj.GetTieChkMdlPath(ciri);
		nStart = strShpPath.ReverseFind('\\') + 1;
		nEnd = strShpPath.ReverseFind('.');
		if (nStart >= nEnd || nStart < 0) continue;
		strShpPath = str + strShpPath.Mid(nStart, nEnd - nStart) + ".shp";

		saveshp.SavePointShp(strShpPath, model, pDoc->m_DomChkPrj.GetDomInfo(VecTieChkModelInfo[ciri].nIdxLeft).GetProjectionRef());

		CallBakPrjPro((ciri + 1)*100.0 / (VecTieChkModelInfo.size()),
			"Saving tie point shape files", pDoc->m_pMain);
	}

	vector<MstuGcpChkModel> VecGcpChkModelInfo = pDoc->m_DomChkPrj.GetGcpModelInfo();
	for (int ciri = 0; ciri < VecGcpChkModelInfo.size(); ciri++)
	{
		model = pDoc->m_DomChkPrj.GetCheckModelPoint(ciri, MODEL_CTL);

		strShpPath = pDoc->m_DomChkPrj.GetGcpChkMdlPath(ciri);
		nStart = strShpPath.ReverseFind('\\') + 1;
		nEnd = strShpPath.ReverseFind('.');
		if (nStart >= nEnd || nStart < 0) continue;
		strShpPath = str + strShpPath.Mid(nStart, nEnd - nStart) + ".shp";
		saveshp.SavePointShp(strShpPath, model, pDoc->m_DomChkPrj.GetDomInfo(VecGcpChkModelInfo[ciri].nDomIdx).GetProjectionRef());

		CallBakPrjPro((ciri + 1)*100.0 / (VecGcpChkModelInfo.size()),
			"Saving Gcp point shape files", pDoc->m_pMain);
	}
	bShow = false;
	CallBakPrjPro(0, "", pDoc->m_pMain);
	pDoc->m_pMain->SendMessage(ID_MYMSG_SHOWPROGRESSBAR, (WPARAM)&bShow, 0);
	AfxMessageBox("点shap文件保存成功!");
	return;
}


void WINAPI MultiThreadMatchCheck(LPVOID p)
{
	MatchCheckPara *pt = (MatchCheckPara *)p;	
	CMDomQuaChkSysDoc *pDoc = (CMDomQuaChkSysDoc*)(pt->p);
	bool bMatchCheck = pt->bMatchCheck;

	bool bShow = true;
	pDoc->m_bIsRunning = true;
	pDoc->m_pMain->SendMessage(ID_MYMSG_SHOWPROGRESSBAR, (WPARAM)&bShow, 0);
	if (bMatchCheck)
	{
		pDoc->m_DomChkPrj.MatchCheck(CallBakPrjPro, pDoc->m_pMain);
	}
	pDoc->m_DomChkPrj.SavePrj2File(pDoc->m_strPtjFilePath);
	bShow = false;
	CallBakPrjPro(0, "", pDoc->m_pMain);
	pDoc->m_pMain->SendMessage(ID_MYMSG_SHOWPROGRESSBAR, (WPARAM)&bShow, 0);
	//pDoc->m_pMain->SendMessage(ID_MYMSG_FILL_MODELLIST, (WPARAM)&pDoc->m_eModelType, (LPARAM)pDoc);

	pDoc->m_pView->SendMessage(ID_MYMSG_FILLREPORTVIEW_TIE, (WPARAM)&pDoc->m_DomChkPrj.GetTieModelInfo(), (LPARAM)&pDoc->m_DomChkPrj.GetTieModelArea());
	pDoc->m_pView->SendMessage(ID_MYMSG_FILLREPORTVIEW_GCP, (WPARAM)&pDoc->m_DomChkPrj.GetGcpModelInfo(), (LPARAM)&pDoc->m_DomChkPrj.GetGcpModelArea());
	pDoc->m_pView->Ready4SaveView2Img(pDoc->m_strCqViewImgPath, false);
	pDoc->m_pView->m_bViewMatchPoint = true;
	pDoc->m_pView->m_bViewSelectPolygon = true;
	pDoc->m_pView->SendMessage(WM_ERASEBKGND);
	pDoc->m_bIsRunning = false;
}
