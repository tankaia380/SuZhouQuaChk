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

// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
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


