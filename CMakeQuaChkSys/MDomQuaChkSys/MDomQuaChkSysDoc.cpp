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

// MDomQuaChkSysDoc.cpp : CMDomQuaChkSysDoc ���ʵ��
//
#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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


// CMDomQuaChkSysDoc ����/����

CMDomQuaChkSysDoc::CMDomQuaChkSysDoc()
{
	// TODO: �ڴ����һ���Թ������
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
			AfxMessageBox("���й�����򲻴��ڣ�");
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
			AfxMessageBox("���й���������");
		}
	}
	if (!IsProcessRunning("GCNode.exe"))
	{
		CString strGCNodeExe = FunGetFileFolder(FunGetSysExePath()) + "\\GCSvr\\WGCS\\GCNode.exe";
		if (!PathFileExists(strGCNodeExe))
		{
			AfxMessageBox("���й�����򲻴��ڣ�");
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
			AfxMessageBox("���й���������");
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
		AfxMessageBox("���д������");
	}
}

bool CMDomQuaChkSysDoc::SaveReport()
{
	if(m_strReportPath.IsEmpty()) return false;
	/************************��ʼ������***********************/
	CsswPdf pdf;
	pdf.InitPdf();
	pdf.SetLineSpace(50);
	pdf.SetPdfTitle("����Ӱ�񼸺�������鱨��", 0, 0.6, 0,2.5);
	CString strLogon = FunGetFileFolder(FunGetSysExePath()) + "\\source\\ImageSky.png";
	int nw = 31, nh = 31;
	int nx = pdf.GetPdfPageWidth() - pdf.GetLineSpace();
	int ny = pdf.GetPdfPageHeight() - pdf.GetTitleGap() - pdf.GetLineGap();
	pdf.AddImage(strLogon, 0.6, SSW_NONE, nx, ny, nw, nh);
	char strLine[2048]; memset(strLine, 0, 2048);
	sprintf(strLine, "��������ʱ�䣺%s", FunGetSysTimeStr());
	pdf.AddText(strLine, SSW_SONG, 1.2, SSW_LEFT, 0, false, 0, 0, 0, 0, 0, 0, true); pdf.AddEmptyLine(-1.2);
	pdf.AddText("@2016 Whu RS DpGrid Group", SSW_SONG, 1.2, SSW_RIGHT, 0, false, 0.4, 0.4, 0.4, 0, 0, 0, true);
	pdf.AddEmptyLine(4);
	int nListCols = 0, nListRows = 0; vector<CString> vecListInfo;
	/************************��������***********************/
	pdf.AddText("�������ݸ���", SSW_HEI, 2.5, SSW_LEFT);
	pdf.AddEmptyLine(2);
	nListCols = 4; nListRows = 3;
	vecListInfo.resize(nListRows*nListCols);
	vecListInfo[0 * nListCols + 0].Format("%s", "Ӧ������");
	vecListInfo[0 * nListCols + 1].Format("%s", "Ӱ����");
	vecListInfo[0 * nListCols + 2].Format("%s", "��������");
	vecListInfo[0 * nListCols + 3].Format("%s", "�ֱ���");
	vecListInfo[1 * nListCols + 0].Format("%s", "�����ƷDOM");
	vecListInfo[1 * nListCols + 1].Format("%d", m_DomChkPrj.GetDomNum());
	
	MstuGeoInfo DomGeoInfo = m_DomChkPrj.GetDomInfo(0).GetGeoInfo();// .eProjInfo;
	if (DomGeoInfo.eProjInfo == PROJECT_UTM) sprintf(strLine, "UTM");
	else if (DomGeoInfo.eProjInfo == PROJECT_BLH) sprintf(strLine, "BLH");
	vecListInfo[1 * nListCols + 2].Format("%s", strLine);
	sprintf(strLine, "%.6lf", m_DomChkPrj.GetDomInfo(0).GetGeoInfo().dResolutionX);
	vecListInfo[1 * nListCols + 3].Format("%s", strLine);

	vecListInfo[2 * nListCols + 0].Format("%s", "�ο�Ӱ��REF");
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
	pdf.AddText("ͼ1. ����ʾ��ͼ�����ɫΪ�ο�Ӱ�񸲸ǣ���ɫ�����������Ӱ��Χ��", SSW_HEI, 1, SSW_CENTER);
	pdf.AddEmptyLine(2.5);
	pdf.AddText("�ʼ���", SSW_HEI, 2.5, SSW_LEFT);
	pdf.AddEmptyLine(2);
	pdf.AddText("��1���ʼ챨�����", SSW_SONG, 2, SSW_LEFT);
	pdf.AddEmptyLine(1);
	char **pLabelStr = new char *[5];
	for (int i = 0; i<5; i++)
	{
		pLabelStr[i] = new char[1024];
	}
	sprintf(pLabelStr[0], "���(pix)");
	sprintf(pLabelStr[1], "0");
	sprintf(pLabelStr[2], "1");
	sprintf(pLabelStr[3], "2");
	sprintf(pLabelStr[4], "3");
	if (PathFileExists(m_strTieViewImgPath))
	{
		pdf.AddImage(m_strTieViewImgPath, 1, SSW_CENTER, 0, 0, 0, 0, true, pLabelStr, 5);
		pdf.AddText("ͼ2. �ӱ߱������", SSW_HEI, 1, SSW_CENTER);
	}
	if (PathFileExists(m_strGcpViewImgPath))
	{
		pdf.AddImage(m_strGcpViewImgPath, 1, SSW_CENTER, 0, 0, 0, 0, true, pLabelStr, 5);
		pdf.AddText("ͼ3. ��λ�������", SSW_HEI, 1, SSW_CENTER);
	}
	for (int i = 0; i<5; i++)
	{
		delete[]pLabelStr[i];
	}
	delete[]pLabelStr;

	pdf.AddEmptyLine(1.5);
	pdf.AddText("��2������Ӱ���б�", SSW_SONG, 2, SSW_LEFT);
	pdf.AddEmptyLine(1);
	sprintf(strLine, "�ӱ����(ƥ���ƽ��в��ֵ)��ֵ %.1fpix������ģ�����£�", m_fTieErrThresold);
	pdf.AddText(strLine, SSW_SONG, 1.5, SSW_LEFT, 0, false, 1, 0, 0);
	pdf.AddEmptyLine(1);
	vecListInfo.push_back("����"); vecListInfo.push_back("��Ӱ������"); vecListInfo.push_back("��Ӱ������");
	vecListInfo.push_back("������"); vecListInfo.push_back("����ֵ");
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
	sprintf(strLine, "��λ���(ƥ���ƽ��в��ֵ)��ֵ %.1fpix������Ӱ�����£�", m_fGcpErrThresold);
	pdf.AddText(strLine, SSW_SONG, 1.5, SSW_LEFT, 0, false, 1, 0, 0);
	pdf.AddEmptyLine(1);
	vecListInfo.push_back("����"); vecListInfo.push_back("Ӱ������"); 
	vecListInfo.push_back("������"); vecListInfo.push_back("����ֵ");
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
	pdf.AddText("��¼", SSW_HEI, 2.5, SSW_LEFT);
	pdf.AddEmptyLine(2);
	pdf.AddText("��1�������Ʒ�б�", SSW_SONG, 2, SSW_LEFT);
	if (m_bSaveImgList)
	{
		pdf.AddEmptyLine(1);
		vecListInfo.push_back("Ӱ������");
		vecListInfo.push_back("Ӱ����");
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

	pdf.AddText("��2���ο�Ӱ���б�", SSW_SONG, 2, SSW_LEFT);
	if (m_bSaveImgList)
	{
		pdf.AddEmptyLine(1);
		vecListInfo.push_back("Ӱ������");
		vecListInfo.push_back("Ӱ����");
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

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CMDomQuaChkSysDoc ���л�

void CMDomQuaChkSysDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CMDomQuaChkSysDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
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

// ������������֧��
void CMDomQuaChkSysDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
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

// CMDomQuaChkSysDoc ���

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


// CMDomQuaChkSysDoc ����


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
	CFileDialog dlg(TRUE, //TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("XML Files (*.xml)|*.xml|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		FilePathName = dlg.GetPathName(); //�ļ�����������FilePathName��
		if (FilePathName == m_strPtjFilePath) return;
		ClearData();
		if (!m_DomChkPrj.LoadPrj4File(FilePathName))
		{
			AfxMessageBox("���̴�ʧ�ܣ�");
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
		strInfo.Format("�ʼ챨�汣��ʧ�ܣ�\n%s", m_strReportPath);
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
	// TODO:  �ڴ���������������
	CSetUp setupDlg;
	if (setupDlg.DoModal() == IDOK)
	{
		m_DomChkPrj.m_nThreodCir = setupDlg.m_nThreodCir;
		m_DomChkPrj.m_nThreodPix = setupDlg.m_nThreodPix;
		m_DomChkPrj.m_nThreodPtNumTie = setupDlg.m_nThreoPtNum;
		m_DomChkPrj.m_nThreodPtNumControl = setupDlg.m_nThreoPtNumConrol;
	}
}
