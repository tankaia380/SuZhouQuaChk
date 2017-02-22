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

	//	����һ��������, ÿ����������һ�������ļ���������
	//  �����ļ�������ڹ����ռ��У��磺 z:\Data\TskGrp.tsk
	//  �����ļ���ʽ���£�
	//	��1��:   ������
	//  ��2��:   ����ID   ����ִ�г���ȫ·����    ��������ļ�ȫ·����   ���ִ��ʱ��
	//  .....    ����ID   ����ִ�г���ȫ·����    ��������ļ�ȫ·����   ���ִ��ʱ��
	//  �� (������+1) ��	
	//  ע�⣺ ����ִ�г�������ڹ����ռ��У��磺 Z:\Soft\gcTsk.exe
	//		   ��������ļ������ڹ����ռ��У��磺 Z:\Soft\tsk1.par
	// 
	//  ����ִ�гɹ�����������ID ,ʧ��ʱ����Ϊ 0 ,����ʹ�� GetLastErrMsg ��ȡ������Ϣ
	//
	UINT			CreateTaskGrp( UINT prjId,LPCSTR lpstrTsk );
	//	ǿ�н���һ��������
	UINT			TerminateTaskGrp( UINT prjId,UINT tskGrpId );
	//  ��ѯ������ִ�����
	UINT			QueryTaskStat( UINT prjId,UINT tskGrpId );
	//  ��ȡ������Ϣ
	LPCSTR			GetLastErrMsg();

	// ͨ�� SendUsrMsg ����Ϣ���� ����ִ�г��򣬼����� CGCTsk::OnUsrMsg(...)
	BOOL			SendUsrMsg( UINT tskGrpId,UINT tskId,TSKMSG tskMsg );

	// ��Ӧ����������Ϣ
	virtual void	OnNetMsg( UINT msgId,DWORD wParam,TSKINF tskInf ){
		if ( msgId==NM_TASK && wParam==TSK_TERM ) OnTaskTerm( tskInf.tskGrp,tskInf.tskId );
		if ( msgId==NM_TASK && wParam==TSK_EXIT ) OnTaskExit( tskInf.tskGrp,tskInf.tskId );
		if ( msgId==NM_TASK && wParam==TSK_MSGS ) OnTaskMsg( tskInf.tskGrp,tskInf.tskId,tskInf.tskMsg );
	};
	// ��Ӧ ����ִ�г����з�����Ϣ������Ӧ CGCTsk::SendTskMsg(...)
	// ��Ҫ���غ�ʹ�á�
	virtual void	OnTaskMsg( UINT tskGrpId,UINT tskId,TSKMSG tskMsg ){};
	
	// �������е�ĳ������ִ����󣬴˺��������ã���Ҫ���غ�ʹ�á�
	// �������������ʱ�������֪���Ƿ���ȷ��ֻҪִ�г����˳���Ϣ�ͷ����ˡ�
	virtual	void    OnTaskExit( UINT tskGrpId,UINT tskId ){};
	
	// �������е�ĳ�������� GCSvr �б���Ϊ��ֹ�󣬴˺��������ã���Ҫ���غ�ʹ�á�	
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
	// ͨ�� SendTskMsg ����Ϣ�����û����������Դ�Ķ��󣬼����� CGCUsr::OnTaskMsg(...)
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
	
	// ��Ӧ �û����������Դ�Ķ����з�����Ϣ������Ӧ CGCUsr::SendUsrMsg(...)
	// ��Ҫ���غ�ʹ�á�
	virtual void OnUsrMsg( UINT tskGrpId,UINT tskId,TSKMSG tskMsg ){};
    virtual void WinProc( UINT message, WPARAM wParam, LPARAM lParam );
protected:
    void*       m_pWuSHM;
    HWND        m_hWnd;
    WORD        m_progRange,m_progCur;
    DWORD       m_pExtPar[8];
};

#endif
