// MNewPrjDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "MNewPrjDlg.h"
#include "afxdialogex.h"
#include "MyFunctions.h"
#include <omp.h>
// CMNewPrjDlg dialog

IMPLEMENT_DYNAMIC(CMNewPrjDlg, CDialogEx)

CMNewPrjDlg::CMNewPrjDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_NEW_PRJ, pParent)
	, m_strDomFileFolder(_T(""))
	, m_strRefFileFolder(_T(""))
	, m_bDeleteInvalidImg(TRUE)
	, m_bAddTifInNewPrj(TRUE)
	, m_bAddPixInNewPrj(TRUE)
	, m_bAddImgInNewPrj(FALSE)
	, m_strPrjFilePath(_T(""))
	, m_strLookingfor(_T("AOIB.tif"))
{
	
}

CMNewPrjDlg::~CMNewPrjDlg()
{
	ClearDomFile();
	ClearRefFile();
}
void CMNewPrjDlg::InitItemPos()
{
	CRect rect;
	GetClientRect(&rect);     //取客户区大小    
	old.x = rect.right - rect.left;
	old.y = rect.bottom - rect.top;
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	CRect rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	cy = rt.bottom;
	MoveWindow(0, 0, cx, cy);
}

void CMNewPrjDlg::ResetItemPos()
{
	float fsp[2];
	POINT Newp; //获取现在对话框的大小  
	CRect recta;
	GetClientRect(&recta);     //取客户区大小    
	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;
	fsp[0] = (float)Newp.x / old.x;
	fsp[1] = (float)Newp.y / old.y;
	CRect Rect;
	int woc;
	CPoint OldTLPoint, TLPoint; //左上角  
	CPoint OldBRPoint, BRPoint; //右下角  
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件    
	while (hwndChild)
	{
		woc = ::GetDlgCtrlID(hwndChild);//取得ID  
		GetDlgItem(woc)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		OldTLPoint = Rect.TopLeft();
		TLPoint.x = long(OldTLPoint.x*fsp[0]);
		TLPoint.y = long(OldTLPoint.y*fsp[1]);
		OldBRPoint = Rect.BottomRight();
		BRPoint.x = long(OldBRPoint.x *fsp[0]);
		BRPoint.y = long(OldBRPoint.y *fsp[1]);
		Rect.SetRect(TLPoint, BRPoint);
		GetDlgItem(woc)->MoveWindow(Rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
	old = Newp;
}

void CMNewPrjDlg::InitImgList()
{
	CRect rect; DWORD dwStyle; int nListWidth, nListHeight;
	dwStyle = (m_ListImgInfoInNewPrj).GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;     //可以选择整行
	dwStyle |= LVS_EX_GRIDLINES;         //格网形式 
	dwStyle |= LVS_OWNERDRAWFIXED;       //可重绘
	dwStyle |= LVS_EX_DOUBLEBUFFER;
	m_ListImgInfoInNewPrj.SetExtendedStyle(dwStyle);
	m_ListImgInfoInNewPrj.InsertColumn(0, "Id", LVCFMT_LEFT, 40, -1);
	m_ListImgInfoInNewPrj.InsertColumn(1, "FullPath", LVCFMT_LEFT, 500, -1);
	m_ListImgInfoInNewPrj.InsertColumn(2, "Type", LVCFMT_LEFT, 75);
//	m_ListImgInfoInNewPrj.InsertColumn(3, "Valid", LVCFMT_LEFT, 75);
}

void CMNewPrjDlg::ClearDomFile()
{
	vector<CString>().swap(m_vecDomFilePath);
	vector<bool>().swap(m_vecDomValidInfo);
}

void CMNewPrjDlg::ClearRefFile()
{
	vector<CString>().swap(m_vecRefFilePath);
	vector<bool>().swap(m_vecRefValidInfo);
}

CString CMNewPrjDlg::GetPrjFilePath()
{
	return m_strPrjFilePath;
}

vector<CString> CMNewPrjDlg::GetDomFilePath()
{
	return m_vecDomFilePath;
}

vector<CString> CMNewPrjDlg::GetRefFilePath()
{
	return m_vecRefFilePath;
}

void CMNewPrjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE_FOLDER_DOM, m_strDomFileFolder);
	DDX_Text(pDX, IDC_MFCEDITBROWSE_FOLDER_REF, m_strRefFileFolder);
	DDX_Control(pDX, IDC_LIST_IMGINFO, m_ListImgInfoInNewPrj);
	DDX_Check(pDX, IDC_CHECK_DELETE_INVALID_IMG, m_bDeleteInvalidImg);
	DDX_Check(pDX, IDC_CHECK_ADD_TIF, m_bAddTifInNewPrj);
	DDX_Check(pDX, IDC_CHECK_ADD_PIX, m_bAddPixInNewPrj);
	DDX_Check(pDX, IDC_CHECK_ADD_IMG, m_bAddImgInNewPrj);
	DDX_Text(pDX, IDC_MFCEDITBROWSE_PATH_PRJ, m_strPrjFilePath);
	DDX_Control(pDX, IDC_EDIT_NUM_DOM, m_EditNumDom);
	DDX_Control(pDX, IDC_EDIT_NUM_REF, m_EditNumRef);
	DDX_Control(pDX, IDC_CHECK_DELETE_INVALID_IMG, m_CheckDeleteInvalidImg);
	DDX_Text(pDX, IDC_LOOKINGFORBYSTRING, m_strLookingfor);
}


BEGIN_MESSAGE_MAP(CMNewPrjDlg, CDialogEx)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_FOLDER_DOM, &CMNewPrjDlg::OnEnChangeMfceditbrowseFolderDom)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE_FOLDER_REF, &CMNewPrjDlg::OnEnChangeMfceditbrowseFolderRef)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_IMGINFO, &CMNewPrjDlg::OnLvnGetdispinfoListImginfo)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &CMNewPrjDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_LOOKINGFOR, &CMNewPrjDlg::OnBnClickedLookingfor)
	ON_BN_CLICKED(IDC_DELETEFILES, &CMNewPrjDlg::OnBnClickedDeletefiles)
END_MESSAGE_MAP()


// CMNewPrjDlg message handlers


void CMNewPrjDlg::OnEnChangeMfceditbrowseFolderDom()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_bAddImgInNewPrj) ClearDomFile();
	if (!m_bAddPixInNewPrj&&!m_bAddTifInNewPrj)
	{
		MessageBox("请先勾选待检索影像类型！");
		return;
	}
	vector<CString> vecNewImgPath;
//	vector<bool> vecNewImgValidInfo;
	if (m_bAddTifInNewPrj)
	{
		FunSearchFile(m_strDomFileFolder, "tif", vecNewImgPath);
		FunSearchFile(m_strDomFileFolder, "TIF", vecNewImgPath);
		FunSearchFile(m_strDomFileFolder, "tiff", vecNewImgPath);
		FunSearchFile(m_strDomFileFolder, "TIFF", vecNewImgPath);
	}
	if (m_bAddPixInNewPrj)
	{
		FunSearchFile(m_strDomFileFolder, "pix", vecNewImgPath);
		FunSearchFile(m_strDomFileFolder, "PIX", vecNewImgPath);
	}
	m_vecDomFilePath.insert(m_vecDomFilePath.end(),vecNewImgPath.begin(),vecNewImgPath.end());
//	m_vecDomValidInfo.insert(m_vecDomValidInfo.end(),vecNewImgValidInfo.begin(),vecNewImgValidInfo.end());

	vecNewImgPath.~vector();
//	vecNewImgValidInfo.~vector();
	CString strInfo;
	strInfo.Format("共检索得到%d景正射产品", m_vecDomFilePath.size());
	FunOutPutLog2File(strInfo);
//	MessageBox(strInfo);
	m_ListImgInfoInNewPrj.SetItemCountEx(m_vecDomFilePath.size() + m_vecRefFilePath.size());
	m_ListImgInfoInNewPrj.Invalidate();
	CString strDomNum; strDomNum.Format("%d", m_vecDomFilePath.size());
	m_EditNumDom.SetWindowText(strDomNum);
}


void CMNewPrjDlg::OnEnChangeMfceditbrowseFolderRef()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if (!m_bAddImgInNewPrj) ClearRefFile();
	if (!m_bAddPixInNewPrj&&!m_bAddTifInNewPrj)
	{
		MessageBox("请先勾选待检索影像类型！");
		return;
	}
	vector<CString> vecNewImgPath;
//	vector<bool> vecNewImgValidInfo;
	if (m_bAddTifInNewPrj)
	{
		FunSearchFile(m_strRefFileFolder, "tif",vecNewImgPath);
		FunSearchFile(m_strRefFileFolder, "TIF",vecNewImgPath);
		FunSearchFile(m_strRefFileFolder, "tiff",vecNewImgPath);
		FunSearchFile(m_strRefFileFolder, "TIFF",vecNewImgPath);
	}
	if (m_bAddPixInNewPrj)
	{
		FunSearchFile(m_strRefFileFolder, "pix", vecNewImgPath);
		FunSearchFile(m_strRefFileFolder, "PIX", vecNewImgPath);
	}
	m_vecRefFilePath.insert(m_vecRefFilePath.end(), vecNewImgPath.begin(), vecNewImgPath.end());

	vecNewImgPath.~vector();
	CString strInfo;
	strInfo.Format("共检索得到%d景参考影像", m_vecRefFilePath.size());
	FunOutPutLog2File(strInfo);
//	MessageBox(strInfo);
	m_ListImgInfoInNewPrj.SetItemCountEx(m_vecDomFilePath.size() + m_vecRefFilePath.size());
	m_ListImgInfoInNewPrj.Invalidate();

	CString strRefNum; strRefNum.Format("%d", m_vecRefFilePath.size());
	m_EditNumRef.SetWindowText(strRefNum);
}


void CMNewPrjDlg::OnLvnGetdispinfoListImginfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	LV_ITEM *pItem = &(pDispInfo)->item;
	if (m_vecDomFilePath.size() + m_vecRefFilePath.size() == 0) return;
	if (pItem->mask & LVIF_TEXT)
	{
		switch (pItem->iSubItem)
		{
		case 0:
			char strId[10]; sprintf(strId, "%d", pItem->iItem);
			lstrcpyn(pItem->pszText, strId, pItem->cchTextMax);
			break;
		case 1:
			if (pItem->iItem < m_vecDomFilePath.size())
			{
				lstrcpyn(pItem->pszText, m_vecDomFilePath[pItem->iItem], pItem->cchTextMax);
			}
			else
			{
				lstrcpyn(pItem->pszText, m_vecRefFilePath[pItem->iItem - m_vecDomFilePath.size()], pItem->cchTextMax);
			}
			break;
		case 2:
			if (pItem->iItem < m_vecDomFilePath.size())
			{
				lstrcpyn(pItem->pszText, "DOM", pItem->cchTextMax);
			}
			else
			{
				lstrcpyn(pItem->pszText, "REF", pItem->cchTextMax);
			}
			break;
// 		case 3:
// 			if (pItem->iItem < m_vecDomFilePath.size())
// 			{
// 				char strValid[10];
// 				if (m_vecDomValidInfo[pItem->iItem]) strcpy(strValid, "true");
// 				else strcpy(strValid, "false");
// 				lstrcpyn(pItem->pszText, strValid, pItem->cchTextMax);
// 			}
// 			else
// 			{
// 				char strValid[10];
// 				if (m_vecRefValidInfo[pItem->iItem - m_vecDomFilePath.size()]) strcpy(strValid, "true");
// 				else strcpy(strValid, "false");
// 				lstrcpyn(pItem->pszText, strValid, pItem->cchTextMax);
// 			}
// 			break;
		default:
			break;
		}
	}
	*pResult = 0;
}


BOOL CMNewPrjDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	InitItemPos();
	int nWndX = GetSystemMetrics(SM_CXSCREEN);
	int nWndY = GetSystemMetrics(SM_CYSCREEN);
	int nDlgX = min(700, nWndX);
	int nDlgY = nDlgX * 3 / 4;
	MoveWindow(0, 0, nDlgX, nDlgY);
	CenterWindow();
	InitImgList();
 	m_strPrjFilePath = "Prj.xml";
	m_EditNumDom.EnableWindow(FALSE);
	m_EditNumDom.SetWindowText("0");

	m_EditNumRef.EnableWindow(FALSE);
	m_EditNumRef.SetWindowText("0");

	m_CheckDeleteInvalidImg.EnableWindow(FALSE);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CMNewPrjDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	ResetItemPos();
	// TODO: Add your message handler code here
}


void CMNewPrjDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if (m_strPrjFilePath.IsEmpty())
	{
		AfxMessageBox("请设置工程文件路径！");
		return;
	}
	if (m_strDomFileFolder.IsEmpty())
	{
		AfxMessageBox("请添加待质检正射影像产品！");
		return;
	}
	if (m_vecDomFilePath.size() == 0)
	{
		AfxMessageBox("待质检正射影像数量为0，请添加！");
		return;
	}
	if (FunGetFileExt(m_strPrjFilePath) != "xml")
	{
		m_strPrjFilePath += ".xml";
	}
	CDialogEx::OnOK();
}

void CMNewPrjDlg::OnBnClickedLookingfor()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_strLookingfor.IsEmpty()) return;
	
	int nCount = m_ListImgInfoInNewPrj.GetItemCount();
	CString strItem;
	for (int ciri = 0; ciri < nCount; ++ciri)
	{
		strItem = m_ListImgInfoInNewPrj.GetItemText(ciri, 1);
		if (strItem.Find(m_strLookingfor)!=-1)
		{
			m_ListImgInfoInNewPrj.SetItemState(ciri, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
		}
		else
		{
			m_ListImgInfoInNewPrj.SetItemState(ciri, 0, -1);
		}
	}
	m_ListImgInfoInNewPrj.SetFocus();
}


void CMNewPrjDlg::OnBnClickedDeletefiles()
{
	// TODO:  在此添加控件通知处理程序代码
	int nCount = m_ListImgInfoInNewPrj.GetItemCount();
	CString strItem;
	UINT nstate;
	vector<int> vecDeleDom, vecDeleRef;
	vecDeleDom.clear();
	vecDeleRef.clear();

	for (int ciri = 0; ciri < nCount; ++ciri)
	{
		nstate = m_ListImgInfoInNewPrj.GetItemState(ciri, LVIS_SELECTED);
		if (nstate == LVIS_SELECTED)
		{
			strItem = m_ListImgInfoInNewPrj.GetItemText(ciri, 1);
			for (int cirm = 0; cirm < m_vecDomFilePath.size(); ++cirm)
			{
				if (strItem == m_vecDomFilePath[cirm])
				{
					vecDeleDom.push_back(cirm);
					break;
				}
			}
			for (int cirm = 0; cirm < m_vecRefFilePath.size(); ++cirm)
			{
				if (strItem == m_vecRefFilePath[cirm])
				{
					vecDeleRef.push_back(cirm);
					break;
				}
			}
		}
	}

	for (int ciri = vecDeleDom.size() - 1; ciri >= 0; --ciri)
	{
		m_vecDomFilePath.erase(m_vecDomFilePath.begin() + vecDeleDom[ciri]);
	}
	for (int ciri = vecDeleRef.size() - 1; ciri >= 0; --ciri)
	{
		m_vecRefFilePath.erase(m_vecRefFilePath.begin() + vecDeleRef[ciri]);
	}

	m_ListImgInfoInNewPrj.SetItemCountEx(m_vecDomFilePath.size() + m_vecRefFilePath.size());

	nCount = m_ListImgInfoInNewPrj.GetItemCount();
	for (int ciri = 0; ciri < nCount; ++ciri)
	{
		m_ListImgInfoInNewPrj.SetItemState(ciri, 0, -1);
	}
	m_ListImgInfoInNewPrj.SetFocus();

	m_ListImgInfoInNewPrj.Invalidate();
}


BOOL CMNewPrjDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_DELETE)
	{
		OnBnClickedDeletefiles();
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
