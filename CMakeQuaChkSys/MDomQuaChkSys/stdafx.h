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

// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��
#include <conio.h>
#include  <stdio.h>
#include "sswPDFLib.h"

#pragma comment (lib,"MImage.lib")
#pragma comment (lib,"MDomQuaChk.lib")
#pragma comment (lib,"glew32.lib")
#pragma comment (lib,"MPDFLib.lib")

//#ifdef _DEBUG
//#pragma comment (lib,"../lib/Debug/x64/MImage.lib")
//#pragma comment (lib,"../lib/Debug/x64/MDomQuaChk.lib")
//#pragma comment (lib,"../lib/Debug/x64/glew32.lib")
//#else
//#pragma comment (lib,"MImage.lib")
//#pragma comment (lib,"MDomQuaChk.lib")
//#pragma comment (lib,"glew32.lib")
//#pragma comment (lib,"MPDFLib.lib")
//#endif



#define SIZE_IMG_TEXTURE 256
#define TIMER_SYSTEM_TIME 1
#define TIMER_MATCH_OVER 2

#define ID_MYMSG_PROGRESSBAR        (WM_USER+1001)
#define ID_MYMSG_SHOWPROGRESSBAR    (WM_USER+1002)
#define ID_MYMSG_INIT_IMGRENDER     (WM_USER+1003)
#define ID_MYMSG_UPDATESTATUSPOS    (WM_USER+1004)
#define ID_MYMSG_FILL_IMGTREE       (WM_USER+1005)
#define ID_MYMSG_FILL_PROPLIST      (WM_USER+1006)
#define ID_MYMSG_FILL_MODELLIST     (WM_USER+1007)
#define ID_MYMSG_SELECTEDRENDER     (WM_USER+1008)
#define ID_MYMSG_MODELPOINTRENDER   (WM_USER+1009)
#define ID_MYMSG_MODELIMGRENDER     (WM_USER+1010)
#define ID_MYMSG_IMGRENDER          (WM_USER+1011)
#define ID_MYMSG_SELECTPOINT        (WM_USER+1012)
#define ID_MYMSG_FILLCHART          (WM_USER+1013)
#define ID_MYMSG_FILLREPORTVIEW_TIE (WM_USER+1014)
#define ID_MYMSG_FILLREPORTVIEW_GCP (WM_USER+1015)
#define ID_MYMSG_SAVECQVIEW2IMAGE   (WM_USER+1016)
#define ID_MYMSG_IMAGEPOINTLINKAGE  (WM_USER+1017)

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


