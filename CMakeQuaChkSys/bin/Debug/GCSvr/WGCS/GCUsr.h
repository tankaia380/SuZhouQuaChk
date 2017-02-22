// GCUsr.h : main header file for the GCUSR DLL
/*----------------------------------------------------------------------+
|		GCUsr											 				|
|       Author:     DuanYanSong  2010/08/19								|
|            Ver 1.0													|
|       Copyright (c)2010, WHU RSGIS DPGrid Group                       |
|	         All rights reserved.                                       |
|		ysduan@sohu.com													|
+----------------------------------------------------------------------*/
#ifndef GCUSR_H_DUANYANSONG_2010_08_19_16_29_07109
#define GCUSR_H_DUANYANSONG_2010_08_19_16_29_07109

#include "GCDef.h"

#ifndef GCUSR_LIB
#define GCUSR_LIB  __declspec(dllimport)
#ifdef _DEBUG_GCUSR
#pragma comment(lib,"GCUsrD.lib") 
#pragma message("Automatically linking with GCUsrD.lib") 
#else
#pragma comment(lib,"GCUsr.lib") 
#pragma message("Automatically linking with GCUsr.lib") 
#endif

#else

#endif

class GCUSR_LIB CGCUsr
{
public:
    CGCUsr();
    virtual ~CGCUsr();

	//	创建一个任务组, 每个任务组由一个任务文件来描述。
	//  任务文件必须放在公共空间中，如： z:\Data\TskGrp.tsk
	//  任务文件格式如下：
	//	第1行:   任务数
	//  第2行:   任务ID   任务执行程序全路径名    任务参数文件全路径名   最大执行时间
	//  .....    任务ID   任务执行程序全路径名    任务参数文件全路径名   最大执行时间
	//  第 (任务数+1) 行	
	//  注意： 任务执行程序必须在公共空间中，如： Z:\Soft\gcTsk.exe
	//		   任务参数文件必须在公共空间中，如： Z:\Soft\tsk1.par
	// 
	//  函数执行成功返回任务组ID ,失败时返回为 0 ,可以使用 GetLastErrMsg 获取错误信息
	//
	UINT			CreateTaskGrp( UINT prjId,LPCSTR lpstrTsk );
	//	强行结束一个任务组
	UINT			TerminateTaskGrp( UINT prjId,UINT tskGrpId );
	//  查询任务组执行情况
	UINT			QueryTaskStat( UINT prjId,UINT tskGrpId );
	//  获取出错信息
	LPCSTR			GetLastErrMsg();

	// 通过 SendUsrMsg 将消息发到 任务执行程序，即发到 CGCTsk::OnUsrMsg(...)
	BOOL			SendUsrMsg( UINT tskGrpId,UINT tskId,TSKMSG tskMsg );

	// 相应所有网络消息
	virtual void	OnNetMsg( UINT msgId,DWORD wParam,TSKINF tskInf ){
		if ( msgId==NM_TASK && wParam==TSK_TERM ) OnTaskTerm( tskInf.tskGrp,tskInf.tskId );
		if ( msgId==NM_TASK && wParam==TSK_EXIT ) OnTaskExit( tskInf.tskGrp,tskInf.tskId );
		if ( msgId==NM_TASK && wParam==TSK_MSGS ) OnTaskMsg( tskInf.tskGrp,tskInf.tskId,tskInf.tskMsg );
	};
	// 响应 任务执行程序中发的消息，即响应 CGCTsk::SendTskMsg(...)
	// 需要重载后使用。
	virtual void	OnTaskMsg( UINT tskGrpId,UINT tskId,TSKMSG tskMsg ){};
	
	// 任务组中的某个任务执行完后，此函数被引用，需要重载后使用。
	// 这个函数被调用时，结果不知道是否正确，只要执行程序退出消息就发回了。
	virtual	void    OnTaskExit( UINT tskGrpId,UINT tskId ){};
	
	// 任务组中的某个任务在 GCSvr 中被人为终止后，此函数被引用，需要重载后使用。	
	virtual	void    OnTaskTerm( UINT tskGrpId,UINT tskId ){};
	

//For private use
public: 
    void    OnBcRecv();
    UINT    m_hSocketBc;
    HANDLE	m_hThreadBc;
	WORD	m_UsrBcPort;

protected:
    virtual void	printfMsg(const char *fmt, ... ){ char strMsg[512]; va_list ap; va_start( ap, fmt ); vsprintf( strMsg,fmt,ap ); va_end(ap);  cprintf("%s\n",strMsg);   };
private:
	GCSVRINF		GetGCSvr(UINT prjId);
	UINT			CreateTskGrp( UINT prjId,LPCSTR lpstrTsk,WORD port );
	char			m_strErMsg[512];

};

class GCUSR_LIB CGCTsk
{
public:
	CGCTsk();
	virtual ~CGCTsk();

	const TSKINF GetTskInf();	
	// For user use function
	// 通过 SendTskMsg 将消息发到用户申请计算资源的对象，即发到 CGCUsr::OnTaskMsg(...)
	BOOL	SendTskMsg( TSKMSG tskMsg ); 
	void	ProgBegin(int range){
				TSKMSG tskMsg; memset( &tskMsg,0,sizeof(tskMsg) ); 
				tskMsg.wParam = TSKPROC_START;  tskMsg.lParam = m_progRange = range; m_progCur=0;
				SendTskMsg( tskMsg ); 
			};
    void	ProgStep(){
				TSKMSG tskMsg; memset( &tskMsg,0,sizeof(tskMsg) ); 
				tskMsg.lParam = MAKELONG(m_progCur,m_progRange);
                tskMsg.wParam = TSKPROC_STEP; m_progCur ++;
				SendTskMsg( tskMsg ); 
			};
    void	ProgEnd(){
				TSKMSG tskMsg; memset( &tskMsg,0,sizeof(tskMsg) ); 
				tskMsg.wParam = TSKPROC_OVER; m_progRange = m_progCur = 0;
				SendTskMsg( tskMsg ); SendTskMsg( tskMsg ); 
			};
    void	PrintMsg( LPCSTR lpstrMsg ){
				TSKMSG tskMsg; memset( &tskMsg,0,sizeof(tskMsg) ); 
				tskMsg.wParam = TSKPROC_MSG;  tskMsg.lParam = -1;
				strcpy( (char*)tskMsg.pBuf_lParam,lpstrMsg );
				SendTskMsg( tskMsg ); 
			};	
	
	// 响应 用户申请计算资源的对象中发的消息，即响应 CGCUsr::SendUsrMsg(...)
	// 需要重载后使用。
	virtual void OnUsrMsg( UINT tskGrpId,UINT tskId,TSKMSG tskMsg ){};
    virtual void WinProc( UINT message, WPARAM wParam, LPARAM lParam );
protected:
    void*       m_pWuSHM;
    HWND        m_hWnd;
    WORD        m_progRange,m_progCur;
    DWORD       m_pExtPar[8];
};

#endif
