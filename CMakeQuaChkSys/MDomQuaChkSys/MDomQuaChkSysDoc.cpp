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

// MDomQuaChkSysDoc.cpp : CMDomQuaChkSysDoc 类的实现
//
#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MDomQuaChkSys.h"
#endif

#include "MDomQuaChkSysDoc.h"

#include <propkey.h>
#include "MNewPrjDlg.h"
#include "SetUp.h"
#include "MultiThreadFuns.h"
#include "MainFrm.h"
#include "MDomQuaChkSysView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMDomQuaChkSysDoc

IMPLEMENT_DYNCREATE(CMDomQuaChkSysDoc, CDocument)

BEGIN_MESSAGE_MAP(CMDomQuaChkSysDoc, CDocument)
	ON_COMMAND(ID_BUTTON_NEW_PRJ, &CMDomQuaChkSysDoc::OnButtonNewPrj)
	ON_COMMAND(ID_BUTTON_AUTO_MATCH, &CMDomQuaChkSysDoc::OnButtonAutoMatch)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_AUTO_MATCH, &CMDomQuaChkSysDoc::OnUpdateButtonAutoMatch)
	ON_COMMAND(ID_BUTTON_OPEN_PRJ, &CMDomQuaChkSysDoc::OnButtonOpenPrj)
	ON_COMMAND(ID_BUTTON_QUALITY_CHECK, &CMDomQuaChkSysDoc::OnButtonQuaChk)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_QUALITY_CHECK, &CMDomQuaChkSysDoc::OnUpdateButtonQuaChk)
	ON_COMMAND(ID_BUTTON_CHECK_TIE, &CMDomQuaChkSysDoc::OnButtonCheckTie)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CHECK_TIE, &CMDomQuaChkSysDoc::OnUpdateButtonCheckTie)
	ON_COMMAND(ID_BUTTON_CHECK_CTL, &CMDomQuaChkSysDoc::OnButtonCheckCtl)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CHECK_CTL, &CMDomQuaChkSysDoc::OnUpdateButtonCheckCtl)
	ON_COMMAND(ID_BUTTON_SAVE_REPORT, &CMDomQuaChkSysDoc::OnButtonSaveReport)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SAVE_REPORT, &CMDomQuaChkSysDoc::OnUpdateButtonSaveReport)
	ON_COMMAND(ID_EDIT_TIE_THRESOLD, &CMDomQuaChkSysDoc::OnEditTieThresold)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GCP_THRESOLD, &CMDomQuaChkSysDoc::OnUpdateEditGcpThresold)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TIE_THRESOLD, &CMDomQuaChkSysDoc::OnUpdateEditTieThresold)
	ON_COMMAND(ID_EDIT_GCP_THRESOLD, &CMDomQuaChkSysDoc::OnEditGcpThresold)
	ON_UPDATE_COMMAND_UI(ID_CHECK_CQ_IMG, &CMDomQuaChkSysDoc::OnUpdateCheckCqImg)
	ON_UPDATE_COMMAND_UI(ID_CHECK_TIE_IMG, &CMDomQuaChkSysDoc::OnUpdateCheckTieImg)
	ON_UPDATE_COMMAND_UI(ID_CHECK_GCP_IMG, &CMDomQuaChkSysDoc::OnUpdateCheckGcpImg)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_NEW_PRJ, &CMDomQuaChkSysDoc::OnUpdateButtonNewPrj)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_OPEN_PRJ, &CMDomQuaChkSysDoc::OnUpdateButtonOpenPrj)
	ON_COMMAND(ID_FILE_NEW, &CMDomQuaChkSysDoc::OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CMDomQuaChkSysDoc::OnUpdateFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CMDomQuaChkSysDoc::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CMDomQuaChkSysDoc::OnUpdateFileOpen)
	ON_COMMAND(ID_CHECK_SAVE_IMGLIST, &CMDomQuaChkSysDoc::OnCheckSaveImglist)
	ON_UPDATE_COMMAND_UI(ID_CHECK_SAVE_IMGLIST, &CMDomQuaChkSysDoc::OnUpdateCheckSaveImglist)
	ON_COMMAND(ID_SETUP, &CMDomQuaChkSysDoc::OnSetup)
END_MESSAGE_MAP()


// CMDomQuaChkSysDoc 构造/析构

CMDomQuaChkSysDoc::CMDomQuaChkSysDoc()
{
	// TODO: 在此添加一次性构造代码
	m_pMain = NULL;
	m_pView = NULL;
	m_bDataReady = false;
	m_bIsRunning = false;
	m_bSaveImgList = true;
	m_eModelType = MODEL_NULL;
	m_pChartDlg = NULL;
	m_fGcpErrThresold = 2;
	m_fTieErrThresold = 2;
	m_bIsIdentify = true;
}

bool CMDomQuaChkSysDoc::IsProcessRunning(CString strExeName)
{
	char buf[1024]; memset(buf, 0, 1024);
	FILE *fp = _popen("tasklist", "r");
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	while (fgets(buf, sizeof(buf), fp))
	{
		buf[strlen(buf) - 1] = '\0';
		vector<CString> vPatr = FunStringTok(buf, "\t ");
		if (vPatr.size() > 0)
			if (vPatr[0] == strExeName)
				return true;
	}
	_pclose(fp);
	return false;
}

bool CMDomQuaChkSysDoc::StartGCProcess()
{
	if (!IsProcessRunning("GCSvr.exe"))
	{
		CString strGCSvrExe = FunGetFileFolder(FunGetSysExePath()) + "\\GCSvr\\WGCS\\GCSvr.exe";
		if (!PathFileExists(strGCSvrExe))
		{
			AfxMessageBox("并行管理程序不存在！");
			return false;
		}
		STARTUPINFO   si;
		PROCESS_INFORMATION   pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		si.wShowWindow = SW_HIDE;
		if (!::CreateProcess(NULL, strGCSvrExe.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		{
			AfxMessageBox("并行管理程序错误！");
		}
	}
	if (!IsProcessRunning("GCNode.exe"))
	{
		CString strGCNodeExe = FunGetFileFolder(FunGetSysExePath()) + "\\GCSvr\\WGCS\\GCNode.exe";
		if (!PathFileExists(strGCNodeExe))
		{
			AfxMessageBox("并行管理程序不存在！");
			return false;
		}
		STARTUPINFO   si;
		PROCESS_INFORMATION   pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		si.wShowWindow = SW_HIDE;
		if (!::CreateProcess(NULL, strGCNodeExe.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		{
			AfxMessageBox("并行管理程序错误！");
		}
	}
	return true;
}

void CMDomQuaChkSysDoc::SendTask2GCSvr()
{
	CString strRunTaskExe = FunGetFileFolder(FunGetSysExePath()) + "\\GCSvr\\RunTask.exe";
	char strRunTask[MAX_SIZE_FILE_PATH];
	sprintf(strRunTask, "%s -l ImageMatch -i %s -svrid 0", strRunTaskExe, m_DomChkPrj.GetGTskFilePath());
	STARTUPINFO   si; PROCESS_INFORMATION   pi;
	ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si); ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(NULL, strRunTask, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		AfxMessageBox("并行处理错误！");
	}
}

bool CMDomQuaChkSysDoc::SaveReport()
{
	if(m_strReportPath.IsEmpty()) return false;
	/************************初始化报告***********************/
	CsswPdf pdf;
	pdf.InitPdf();
	pdf.SetLineSpace(50);
	pdf.SetPdfTitle("正射影像几何质量检查报告", 0, 0.6, 0,2.5);
	CString strLogon = FunGetFileFolder(FunGetSysExePath()) + "\\source\\ImageSky.png";
	int nw = 31, nh = 31;
	int nx = pdf.GetPdfPageWidth() - pdf.GetLineSpace();
	int ny = pdf.GetPdfPageHeight() - pdf.GetTitleGap() - pdf.GetLineGap();
	pdf.AddImage(strLogon, 0.6, SSW_NONE, nx, ny, nw, nh);
	char strLine[2048]; memset(strLine, 0, 2048);
	sprintf(strLine, "报告生成时间：%s", FunGetSysTimeStr());
	pdf.AddText(strLine, SSW_SONG, 1.2, SSW_LEFT, 0, false, 0, 0, 0, 0, 0, 0, true); pdf.AddEmptyLine(-1.2);
	pdf.AddText("@2016 Whu RS DpGrid Group", SSW_SONG, 1.2, SSW_RIGHT, 0, false, 0.4, 0.4, 0.4, 0, 0, 0, true);
	pdf.AddEmptyLine(4);
	int nListCols = 0, nListRows = 0; vector<CString> vecListInfo;
	/************************测区概述***********************/
	pdf.AddText("测区数据概述", SSW_HEI, 2.5, SSW_LEFT);
	pdf.AddEmptyLine(2);
	nListCols = 4; nListRows = 3;
	vecListInfo.resize(nListRows*nListCols);
	vecListInfo[0 * nListCols + 0].Format("%s", "应用类型");
	vecListInfo[0 * nListCols + 1].Format("%s", "影像数");
	vecListInfo[0 * nListCols + 2].Format("%s", "坐标类型");
	vecListInfo[0 * nListCols + 3].Format("%s", "分辨率");
	vecListInfo[1 * nListCols + 0].Format("%s", "正射产品DOM");
	vecListInfo[1 * nListCols + 1].Format("%d", m_DomChkPrj.GetDomNum());
	
	MstuGeoInfo DomGeoInfo = m_DomChkPrj.GetDomInfo(0).GetGeoInfo();// .eProjInfo;
	if (DomGeoInfo.eProjInfo == PROJECT_UTM) sprintf(strLine, "UTM");
	else if (DomGeoInfo.eProjInfo == PROJECT_BLH) sprintf(strLine, "BLH");
	vecListInfo[1 * nListCols + 2].Format("%s", strLine);
	sprintf(strLine, "%.6lf", m_DomChkPrj.GetDomInfo(0).GetGeoInfo().dResolutionX);
	vecListInfo[1 * nListCols + 3].Format("%s", strLine);

	vecListInfo[2 * nListCols + 0].Format("%s", "参考影像REF");
	vecListInfo[2 * nListCols + 1].Format("%d", m_DomChkPrj.GetRefNum());
	
	MstuGeoInfo RefGeoInfo = m_DomChkPrj.GetRefInfo(0).GetGeoInfo();// .eProjInfo;
	if (RefGeoInfo.eProjInfo == PROJECT_UTM) sprintf(strLine, "UTM");
	else if (RefGeoInfo.eProjInfo == PROJECT_BLH) sprintf(strLine, "BLH");

	vecListInfo[2 * nListCols + 2].Format("%s", strLine);
	sprintf(strLine, "%.6lf", RefGeoInfo.dResolutionX);
	vecListInfo[2 * nListCols + 3].Format("%s", strLine);
 	pdf.AddList(nListCols, nListRows, vecListInfo);
	
	vector<CString>().swap(vecListInfo);
 	pdf.AddEmptyLine(1);
	if(PathFileExists(m_strCqViewImgPath)) pdf.AddImage(m_strCqViewImgPath);
	pdf.AddText("图1. 测区示意图（填充色为参考影像覆盖，蓝色多边形有正射影像范围）", SSW_HEI, 1, SSW_CENTER);
	pdf.AddEmptyLine(2.5);
	pdf.AddText("质检结果", SSW_HEI, 2.5, SSW_LEFT);
	pdf.AddEmptyLine(2);
	pdf.AddText("（1）质检报告填充", SSW_SONG, 2, SSW_LEFT);
	pdf.AddEmptyLine(1);
	char **pLabelStr = new char *[5];
	for (int i = 0; i<5; i++)
	{
		pLabelStr[i] = new char[1024];
	}
	sprintf(pLabelStr[0], "误差(pix)");
	sprintf(pLabelStr[1], "0");
	sprintf(pLabelStr[2], "1");
	sprintf(pLabelStr[3], "2");
	sprintf(pLabelStr[4], "3");
	if (PathFileExists(m_strTieViewImgPath))
	{
		pdf.AddImage(m_strTieViewImgPath, 1, SSW_CENTER, 0, 0, 0, 0, true, pLabelStr, 5);
		pdf.AddText("图2. 接边报告填充", SSW_HEI, 1, SSW_CENTER);
	}
	if (PathFileExists(m_strGcpViewImgPath))
	{
		pdf.AddImage(m_strGcpViewImgPath, 1, SSW_CENTER, 0, 0, 0, 0, true, pLabelStr, 5);
		pdf.AddText("图3. 定位报告填充", SSW_HEI, 1, SSW_CENTER);
	}
	for (int i = 0; i<5; i++)
	{
		delete[]pLabelStr[i];
	}
	delete[]pLabelStr;

	pdf.AddEmptyLine(1.5);
	pdf.AddText("（2）超限影像列表", SSW_SONG, 2, SSW_LEFT);
	pdf.AddEmptyLine(1);
	sprintf(strLine, "接边误差(匹配点平面残差均值)阈值 %.1fpix，超限模型如下：", m_fTieErrThresold);
	pdf.AddText(strLine, SSW_SONG, 1.5, SSW_LEFT, 0, false, 1, 0, 0);
	pdf.AddEmptyLine(1);
	vecListInfo.push_back("索引"); vecListInfo.push_back("左影像索引"); vecListInfo.push_back("右影像索引");
	vecListInfo.push_back("检查点数"); vecListInfo.push_back("误差均值");
	vector<MstuTieChkModel> vecTieModel = m_DomChkPrj.GetTieModelInfo();
	int nCount = 0;
	for (int i = 0; i<vecTieModel.size(); i++)
	{
		if (vecTieModel[i].dMeanDxy >= m_fTieErrThresold)
		{
			sprintf(strLine, "%04d", nCount); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecTieModel[i].nIdxLeft); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecTieModel[i].nIdxRight); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecTieModel[i].nPtNum); vecListInfo.push_back(strLine);
			sprintf(strLine, "%.1f", vecTieModel[i].dMeanDxy); vecListInfo.push_back(strLine);
			nCount++;
		}
	}
	for (int i = 0; i < vecTieModel.size(); i++)
	{
		if (vecTieModel[i].nPtNum==0)
		{
			sprintf(strLine, "%04d", nCount); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecTieModel[i].nIdxLeft); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecTieModel[i].nIdxRight); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecTieModel[i].nPtNum); vecListInfo.push_back(strLine);
			sprintf(strLine, "%.1f", vecTieModel[i].dMeanDxy); vecListInfo.push_back(strLine);
			nCount++;
		}
	}
	nListCols = 5; nListRows = nCount + 1;
	pdf.AddList(nListCols,nListRows, vecListInfo);
	vector<CString>().swap(vecListInfo);
	vector<MstuTieChkModel>().swap(vecTieModel);
	pdf.AddEmptyLine(1.5);
	sprintf(strLine, "定位误差(匹配点平面残差均值)阈值 %.1fpix，超限影像如下：", m_fGcpErrThresold);
	pdf.AddText(strLine, SSW_SONG, 1.5, SSW_LEFT, 0, false, 1, 0, 0);
	pdf.AddEmptyLine(1);
	vecListInfo.push_back("索引"); vecListInfo.push_back("影像索引"); 
	vecListInfo.push_back("检查点数"); vecListInfo.push_back("误差均值");
	vector<MstuGcpChkModel> vecGcpModel = m_DomChkPrj.GetGcpModelInfo();
	nCount = 0;
	for (int i = 0; i < vecGcpModel.size(); i++)
	{
		if (vecGcpModel[i].dMeanDxy >= m_fGcpErrThresold)
		{
			sprintf(strLine, "%04d", nCount); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecGcpModel[i].nDomIdx); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecGcpModel[i].nPtNum);	vecListInfo.push_back(strLine);
			sprintf(strLine, "%.1f", vecGcpModel[i].dMeanDxy); vecListInfo.push_back(strLine);
			nCount++;
		}
	}
	for (int i = 0; i < vecGcpModel.size(); i++)
	{
		if (vecGcpModel[i].nPtNum==0)
		{
			sprintf(strLine, "%04d", nCount); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecGcpModel[i].nDomIdx); vecListInfo.push_back(strLine);
			sprintf(strLine, "%d", vecGcpModel[i].nPtNum);	vecListInfo.push_back(strLine);
			sprintf(strLine, "%.1f", vecGcpModel[i].dMeanDxy); vecListInfo.push_back(strLine);
			nCount++;
		}
	}
	nListCols = 4; nListRows = nCount + 1;
	pdf.AddList(nListCols, nListRows, vecListInfo);
	vector<CString>().swap(vecListInfo);
	pdf.AddEmptyLine(2.5);
	pdf.AddText("附录", SSW_HEI, 2.5, SSW_LEFT);
	pdf.AddEmptyLine(2);
	pdf.AddText("（1）正射产品列表", SSW_SONG, 2, SSW_LEFT);
	if (m_bSaveImgList)
	{
		pdf.AddEmptyLine(1);
		vecListInfo.push_back("影像索引");
		vecListInfo.push_back("影像名");
		nCount = m_DomChkPrj.GetDomNum();
		for (int i = 0; i < nCount; i++)
		{
			sprintf(strLine, "%d", i); vecListInfo.push_back(strLine);
			vecListInfo.push_back(FunGetFileName(m_DomChkPrj.GetDomInfo(i).GetImgPath()));
		}
		nListCols = 2; nListRows = nCount + 1;
		pdf.AddList(nListCols, nListRows, vecListInfo);
		vector<CString>().swap(vecListInfo);
	}
	pdf.AddEmptyLine(1.5);

	pdf.AddText("（2）参考影像列表", SSW_SONG, 2, SSW_LEFT);
	if (m_bSaveImgList)
	{
		pdf.AddEmptyLine(1);
		vecListInfo.push_back("影像索引");
		vecListInfo.push_back("影像名");
		nCount = m_DomChkPrj.GetRefNum();
		for (int i = 0; i < nCount; i++)
		{
			sprintf(strLine, "%d", i); vecListInfo.push_back(strLine);
			vecListInfo.push_back(FunGetFileName(m_DomChkPrj.GetRefInfo(i).GetImgPath()));
		}
		nListCols = 2; nListRows = nCount + 1;
		pdf.AddList(nListCols, nListRows, vecListInfo);
	}
 	if (!pdf.SavePdf(m_strReportPath)) return false;
	
	return true;
}

CMDomQuaChkSysDoc::~CMDomQuaChkSysDoc()
{
	if (NULL != m_pChartDlg)
	{
		delete m_pChartDlg;
	}
}

BOOL CMDomQuaChkSysDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMDomQuaChkSysDoc 序列化

void CMDomQuaChkSysDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMDomQuaChkSysDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMDomQuaChkSysDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMDomQuaChkSysDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMDomQuaChkSysDoc 诊断

#ifdef _DEBUG
void CMDomQuaChkSysDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMDomQuaChkSysDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMDomQuaChkSysDoc 命令


void CMDomQuaChkSysDoc::OnButtonNewPrj()
{
	// TODO: Add your command handler code here
	CMNewPrjDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		ClearData();
		m_strPtjFilePath = dlg.GetPrjFilePath();
		vector<CString> vecDomFilePath = dlg.GetDomFilePath();
		vector<CString> vecRefFilePath = dlg.GetRefFilePath();
		m_vecDomInfo.resize(vecDomFilePath.size());
		m_vecRefInfo.resize(vecRefFilePath.size());
		for (int i = 0; i < vecDomFilePath.size(); i++)
		{
			m_vecDomInfo[i].SetImgPath(vecDomFilePath[i]);
			m_vecDomInfo[i].SetImgType(IMGTYPE_DOM);
		}
		for (int i = 0; i < vecRefFilePath.size(); i++)
		{
			m_vecRefInfo[i].SetImgPath(vecRefFilePath[i]);
			m_vecRefInfo[i].SetImgType(IMGTYPE_REF);
		}
		if (m_pMain == NULL) m_pMain = FunGetMainFrameHand();
		if (m_pView == NULL) m_pView = FunGetViewHand();
		m_strReportPath = FunGetFileFolder(m_strPtjFilePath) + "\\" + FunGetFileName(m_strPtjFilePath, false) + ".pdf";
		m_strCqViewImgPath = FunGetFileFolder(m_strPtjFilePath) + "\\source\\" + "CqView.jpg";
		m_strTieViewImgPath = FunGetFileFolder(m_strPtjFilePath) + "\\source\\" + "TieView.jpg";
		m_strGcpViewImgPath = FunGetFileFolder(m_strPtjFilePath) + "\\source\\" + "GcpView.jpg";
		FunCreateDir4Path(m_strCqViewImgPath);
		HANDLE hThread; DWORD IdThread;
		hThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)MultiThreadImgPrePro,
			(LPVOID*)this, 0, &IdThread);
		if (m_pChartDlg == NULL) 
		{
			m_pChartDlg = new CMyMatchPointChartDlg;
			m_pChartDlg->Create(IDD_DIALOG_MATCHPOINT_CHART);
			m_pChartDlg->ShowWindow(FALSE);
		}

	}
}
void CMDomQuaChkSysDoc::OnButtonOpenPrj()
{
	// TODO: Add your command handler code here
	if (m_pView == NULL) m_pView = FunGetViewHand();
	if (m_pMain == NULL) m_pMain = FunGetMainFrameHand();
	CString FilePathName;
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		FilePathName = dlg.GetPathName(); //文件名保存在了FilePathName里
		if (FilePathName == m_strPtjFilePath) return;
		ClearData();
		if (!m_DomChkPrj.LoadPrj4File(FilePathName))
		{
			AfxMessageBox("工程打开失败！");
			m_bDataReady = false;
			return;
		}
		m_strPtjFilePath = FilePathName;
		m_vecDomInfo = m_DomChkPrj.GetAllDomInfo();
		m_vecRefInfo = m_DomChkPrj.GetAllRefInfo();
		m_DomChkPrj.ClearPrjInfo();
// 		vector<CString> vecDomPath, vecRefPath;
// 		for (int i = 0; i < m_DomChkPrj.GetDomNum(); i++)
// 		{
// 			vecDomPath.push_back(m_DomChkPrj.GetDomInfo(i).GetImgPath());
// 		}
// 		for (int i = 0; i < m_DomChkPrj.GetRefNum(); i++)
// 		{
// 			vecRefPath.push_back(m_DomChkPrj.GetRefInfo(i).GetImgPath());
// 		}
//		m_pMain->SendMessage(ID_MYMSG_FILL_IMGTREE, (WPARAM)&vecDomPath, (LPARAM)&vecRefPath);
		m_strReportPath = FunGetFileFolder(m_strPtjFilePath) + "\\" + FunGetFileName(m_strPtjFilePath, false) + ".pdf";
		m_strCqViewImgPath = FunGetFileFolder(m_strPtjFilePath) + "\\source\\" + "CqView.jpg";
		m_strTieViewImgPath = FunGetFileFolder(m_strPtjFilePath) + "\\source\\" + "TieView.jpg";
		m_strGcpViewImgPath = FunGetFileFolder(m_strPtjFilePath) + "\\source\\" + "GcpView.jpg";
		FunCreateDir4Path(m_strCqViewImgPath);
		HANDLE hThread; DWORD IdThread;
		hThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)MultiThreadImgPrePro,
			(LPVOID*)this, 0, &IdThread);
		if (m_pChartDlg == NULL)
		{
			m_pChartDlg = new CMyMatchPointChartDlg;
			m_pChartDlg->Create(IDD_DIALOG_MATCHPOINT_CHART,NULL);   //IDD_DIALOG_MATCHPOINT_CHART
			m_pChartDlg->ShowWindow(FALSE);
		}
	}
}

void CMDomQuaChkSysDoc::OnButtonAutoMatch()
{
	// TODO: Add your command handler code here
	m_bIsRunning = true;
	if (!StartGCProcess()) return;
	CString strTmpFolder = m_DomChkPrj.GetMatchFolder() + "\\Model\\temp";
	vector<CString> vecTmpPath;
	FunSearchFile(strTmpFolder, "tmp", vecTmpPath);
	for (int i = 0; i < vecTmpPath.size(); i++)
	{
		remove(vecTmpPath[i]);
	}
	remove(strTmpFolder);
	vector<CString>().swap(vecTmpPath);
	SendTask2GCSvr();

	m_pMain->SetTimer(TIMER_MATCH_OVER, 100, NULL);
 	bool bShow = TRUE;
 	m_pMain->SendMessage(ID_MYMSG_SHOWPROGRESSBAR, (WPARAM)&bShow, 0);
// 	HANDLE hThread; DWORD IdThread;
// 	hThread = CreateThread(NULL, 0,
// 		(LPTHREAD_START_ROUTINE)MultiThreadMatchCheck,
// 		(LPVOID*)FunGetDocHand(), 0, &IdThread);
}
void CMDomQuaChkSysDoc::OnUpdateButtonAutoMatch(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(!m_bIsRunning);
}


void CMDomQuaChkSysDoc::OnButtonQuaChk()
{
	// TODO: Add your command handler code here
//	m_DomChkPrj.MatchCheck();
}

void CMDomQuaChkSysDoc::OnUpdateButtonQuaChk(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(!m_bIsRunning);
}

void CMDomQuaChkSysDoc::OnButtonCheckTie()
{
	// TODO: Add your command handler code here
	m_eModelType = MODEL_TIE;
	FunGetViewHand()->m_vecSelectModelFromView.clear();

	m_pMain->SendMessage(ID_MYMSG_FILL_MODELLIST, (WPARAM)&m_eModelType, (LPARAM)this);
	if (m_pView->m_bViewFill) m_pView->Ready4SaveView2Img(m_strTieViewImgPath, true);
	m_pView->Invalidate();
}

void CMDomQuaChkSysDoc::OnUpdateButtonCheckTie(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE); else
		pCmdUI->Enable(!m_bIsRunning && m_DomChkPrj.GetTieModelInfo().size());
	pCmdUI->SetCheck(m_eModelType == MODEL_TIE ? true : false);
}


void CMDomQuaChkSysDoc::OnButtonCheckCtl()
{
	// TODO: Add your command handler code here
	m_eModelType = MODEL_CTL;
	FunGetViewHand()->m_vecSelectModelFromView.clear();

	m_pMain->SendMessage(ID_MYMSG_FILL_MODELLIST, (WPARAM)&m_eModelType, (LPARAM)this);
	if (m_pView->m_bViewFill) m_pView->Ready4SaveView2Img(m_strGcpViewImgPath, true);
	m_pView->Invalidate();
}


void CMDomQuaChkSysDoc::OnUpdateButtonCheckCtl(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE); else
		pCmdUI->Enable(!m_bIsRunning && m_DomChkPrj.GetGcpModelInfo().size());
	pCmdUI->SetCheck(m_eModelType == MODEL_CTL ? true : false);
}

void CMDomQuaChkSysDoc::OnButtonSaveReport()
{
	// TODO: Add your command handler code here
	CString strInfo;
	if (!SaveReport()) 
	{
		strInfo.Format("质检报告保存失败！\n%s", m_strReportPath);
		AfxMessageBox(strInfo);
	}

	HANDLE hThread; DWORD IdThread;
	hThread = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)MultiThreadSavePointShpFiles,
		(LPVOID*)this, 0, &IdThread);
}


void CMDomQuaChkSysDoc::OnUpdateButtonSaveReport(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(!m_bIsRunning);
}


void CMDomQuaChkSysDoc::OnEditTieThresold()
{
	// TODO: Add your command handler code here
	m_fTieErrThresold = m_pMain->GetTieThresold();
}

void CMDomQuaChkSysDoc::OnUpdateEditTieThresold(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(!m_bIsRunning);
}

void CMDomQuaChkSysDoc::OnEditGcpThresold()
{
	// TODO: Add your command handler code here
	m_fGcpErrThresold = m_pMain->GetGcpThresold();
}

void CMDomQuaChkSysDoc::OnUpdateEditGcpThresold(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(!m_bIsRunning);
}


void CMDomQuaChkSysDoc::OnUpdateCheckCqImg(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(PathFileExists(m_strCqViewImgPath));
}


void CMDomQuaChkSysDoc::OnUpdateCheckTieImg(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(PathFileExists(m_strTieViewImgPath));
}


void CMDomQuaChkSysDoc::OnUpdateCheckGcpImg(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(PathFileExists(m_strGcpViewImgPath));
}


void CMDomQuaChkSysDoc::OnUpdateButtonNewPrj(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_bIsRunning);
}


void CMDomQuaChkSysDoc::OnUpdateButtonOpenPrj(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_bIsRunning);
}


void CMDomQuaChkSysDoc::OnFileNew()
{
	// TODO: Add your command handler code here
	OnButtonNewPrj();
}


void CMDomQuaChkSysDoc::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_bIsRunning);
}


void CMDomQuaChkSysDoc::OnFileOpen()
{
	// TODO: Add your command handler code here
	OnButtonOpenPrj();
}


void CMDomQuaChkSysDoc::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_bIsRunning);
}



void CMDomQuaChkSysDoc::OnCheckSaveImglist()
{
	// TODO: Add your command handler code here
	m_bSaveImgList = !m_bSaveImgList;
}


void CMDomQuaChkSysDoc::OnUpdateCheckSaveImglist(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_bDataReady) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(!m_bIsRunning);
	pCmdUI->SetCheck(m_bSaveImgList);
}


void CMDomQuaChkSysDoc::OnSetup()
{
	// TODO:  在此添加命令处理程序代码
	CSetUp setupDlg;
	if (setupDlg.DoModal() == IDOK)
	{
		m_DomChkPrj.m_nThreodCir = setupDlg.m_nThreodCir;
		m_DomChkPrj.m_nThreodPix = setupDlg.m_nThreodPix;
		m_DomChkPrj.m_nThreodPtNumTie = setupDlg.m_nThreoPtNum;
		m_DomChkPrj.m_nThreodPtNumControl = setupDlg.m_nThreoPtNumConrol;
	}
}
