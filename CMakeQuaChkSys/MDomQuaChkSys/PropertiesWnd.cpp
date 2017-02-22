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

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MDomQuaChkSys.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	pGroupFile = new CMFCPropertyGridProperty("文件");
	pFileName = new CMFCPropertyGridProperty("文件名", (_variant_t)"", "影像文件名");
	pFileSize = new CMFCPropertyGridProperty("文件大小(MB)", (_variant_t)(double)0.0, "影像文件大小");
	pImgSize = new CMFCPropertyGridProperty("影像大小", 0, TRUE);
	pImgHei = new CMFCPropertyGridProperty("高度", (_variant_t)(int)0, "影像高-像素数");;
	pImgWid = new CMFCPropertyGridProperty("宽度", (_variant_t)(int)0, "影像宽-像素数");

	pGroupGeo = new CMFCPropertyGridProperty("地理参考信息");
	pCoordinate = new CMFCPropertyGridProperty("坐标系", (_variant_t)"", "参考坐标系");
	pProjection = new CMFCPropertyGridProperty("投影坐标", (_variant_t)"", "投影坐标类型");
	pImgRes = new CMFCPropertyGridProperty("影像分辨率", 0, TRUE);
	pImgRes_x = new CMFCPropertyGridProperty("横向", (_variant_t)(double)0.0, "X方向分辨率");
	pImgRes_y = new CMFCPropertyGridProperty("纵向", (_variant_t)(double)0.0, "Y方向分辨率");
}

CPropertiesWnd::~CPropertiesWnd()
{
	int nCount = m_wndPropList.GetPropertyCount();
	for (int i = 0; i < nCount; i++)
	{
		CMFCPropertyGridProperty *p = m_wndPropList.GetProperty(i);
		m_wndPropList.DeleteProperty(p, 1, 1);
	}
}


BEGIN_MESSAGE_MAP(CPropertiesWnd, CMyDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}



int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMyDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;


	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}
//	m_wndPropList.EnableWindow(FALSE);
	InitPropList();

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CMyDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	m_wndPropList.SetLeftColWidth(1);

	pGroupFile->AddSubItem(pFileName);
	pGroupFile->AddSubItem(pFileSize);
	pImgSize->AddSubItem(pImgHei);
	pImgSize->AddSubItem(pImgWid);
	pImgSize->Expand(TRUE);
	pGroupFile->AddSubItem(pImgSize);

	pGroupGeo->AddSubItem(pCoordinate);
	pGroupGeo->AddSubItem(pProjection);
	pImgRes->AddSubItem(pImgRes_y);
	pImgRes->AddSubItem(pImgRes_x);
	pImgRes->Expand(TRUE);
	pGroupGeo->AddSubItem(pImgRes);

	m_wndPropList.AddProperty(pGroupFile);
	m_wndPropList.AddProperty(pGroupGeo);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CMyDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}
void CPropertiesWnd::FillPropList(MstuPropOfImg stuPropInfo)
{
	pFileName->SetValue((_variant_t)stuPropInfo.strFileName);
	pFileSize->SetValue((_variant_t)(double)stuPropInfo.dFileSizeMB);
	pImgHei->SetValue((_variant_t)(int)stuPropInfo.nHeight);
	pImgWid->SetValue((_variant_t)(int)stuPropInfo.nWidth);

	CString strValue;
	if (stuPropInfo.eCoordinateSys == COORDINATE_SYS_WGS84) strValue = "WGS84";
	pCoordinate->SetValue((_variant_t)strValue);
	if (stuPropInfo.eProjectInfo == PROJECT_NONE) strValue = "NONE";
	if (stuPropInfo.eProjectInfo == PROJECT_UTM) strValue = "UTM";
	if (stuPropInfo.eProjectInfo == PROJECT_BLH) strValue = "BLH";
	pProjection->SetValue((_variant_t)strValue);
	pImgRes_y->SetValue((_variant_t)(double)stuPropInfo.dResY);
	pImgRes_x->SetValue((_variant_t)(double)stuPropInfo.dResX);

	m_wndPropList.UpdateData();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMyDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}
