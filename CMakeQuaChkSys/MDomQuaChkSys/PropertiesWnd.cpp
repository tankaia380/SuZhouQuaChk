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
	pGroupFile = new CMFCPropertyGridProperty("�ļ�");
	pFileName = new CMFCPropertyGridProperty("�ļ���", (_variant_t)"", "Ӱ���ļ���");
	pFileSize = new CMFCPropertyGridProperty("�ļ���С(MB)", (_variant_t)(double)0.0, "Ӱ���ļ���С");
	pImgSize = new CMFCPropertyGridProperty("Ӱ���С", 0, TRUE);
	pImgHei = new CMFCPropertyGridProperty("�߶�", (_variant_t)(int)0, "Ӱ���-������");;
	pImgWid = new CMFCPropertyGridProperty("���", (_variant_t)(int)0, "Ӱ���-������");

	pGroupGeo = new CMFCPropertyGridProperty("����ο���Ϣ");
	pCoordinate = new CMFCPropertyGridProperty("����ϵ", (_variant_t)"", "�ο�����ϵ");
	pProjection = new CMFCPropertyGridProperty("ͶӰ����", (_variant_t)"", "ͶӰ��������");
	pImgRes = new CMFCPropertyGridProperty("Ӱ��ֱ���", 0, TRUE);
	pImgRes_x = new CMFCPropertyGridProperty("����", (_variant_t)(double)0.0, "X����ֱ���");
	pImgRes_y = new CMFCPropertyGridProperty("����", (_variant_t)(double)0.0, "Y����ֱ���");
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
// CResourceViewBar ��Ϣ�������

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

	// �������: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;


	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("δ�ܴ�����������\n");
		return -1;      // δ�ܴ���
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
	// TODO: �ڴ˴���������������
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: �ڴ˴����������� UI ����������
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
