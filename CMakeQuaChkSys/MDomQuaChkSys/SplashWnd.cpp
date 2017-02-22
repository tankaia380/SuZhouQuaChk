// SplashWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "SplashWnd.h"
#include "MyFunctions.h"
#include "MDomQuaChk/MDomQuaChk.h"
// CSplashWnd

IMPLEMENT_DYNAMIC(CSplashWnd, CWnd)

CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
}


BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CSplashWnd ��Ϣ�������




bool CSplashWnd::CreateBmp()
{

	// TODO:  �ڴ������ר�õĴ�������
	CString strBakBmp = FunGetFileFolder(FunGetSysExePath())+"\\resource\\SplashView.bmp";
	m_bitmap_bak.Detach(); 
	HBITMAP hBitmapBak = (HBITMAP)::LoadImage(NULL,strBakBmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE|LR_DEFAULTSIZE);
	if(!m_bitmap_bak.Attach(hBitmapBak)) return false;
	BITMAP bitmap;
	m_bitmap_bak.GetBitmap(&bitmap);
	CreateEx(0,	AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bitmap.bmWidth, bitmap.bmHeight, NULL, NULL);
	CenterWindow();
	GetClientRect(m_WndRect);
	Sleep(2000);	
}


void CSplashWnd::OnPaint()
{
	CPaintDC dc(this); // device context forpainting
}


void CSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CWnd::OnTimer(nIDEvent);
}


BOOL CSplashWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CPaintDC dc(this);
	BITMAP bitmap;
	m_bitmap_bak.GetBitmap(&bitmap);
	CDC dcComp;
	dcComp.CreateCompatibleDC(&dc);
	dcComp.SelectObject(&m_bitmap_bak);
	dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcComp, 0, 0, SRCCOPY);
	return true;
}
