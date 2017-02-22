// GCDef.h
/*----------------------------------------------------------------------+
|		GCDef											 		        |
|       Author:     DuanYanSong  2010/08/16								|
|            Ver 1.0													|
|       Copyright (c)2010, WHU RSGIS DPGrid Group                       |
|	         All rights reserved.                                       |
|		ysduan@sohu.com													|
+----------------------------------------------------------------------*/

#ifndef GCDEF_H_DUANYANSONG_2010_08_16_23_24_3420983274098
#define GCDEF_H_DUANYANSONG_2010_08_16_23_24_3420983274098


#ifdef WIN32
#include <conio.h>
#include "Winsock.h"
#pragma comment(lib,"Ws2_32.lib") 
#pragma message("Automatically linking with Ws2_32.lib") 
#else

#define TRUE				1
#define FALSE				0
#define NULL				0


typedef 	unsigned int	HWND		;
typedef 	unsigned int	HANDLE		;
typedef 	int				BOOL		;
typedef 	const char*     LPCSTR		;
typedef 	unsigned char	BYTE		;
typedef 	unsigned int	RGBQUAD		;
typedef 	unsigned short	WORD		;
typedef 	unsigned int	DWORD		;
typedef 	unsigned int	UINT		;
typedef 	long			LONGLONG	;
typedef     int             WPARAM      ;
typedef     int             LPARAM      ;
typedef     int             SOCKET      ;

#define     cprintf         printf
#define     closesocket     close
long        GetCurrentProcessId(){ return getpid(); };

#endif

#define LXSVR_BC_PORT      4558
#define LXSVR_NT_PORT      4559

#define GCSVR_BC_PORT      4568
#define GCSVR_NT_PORT      4569

#define GCUSR_BC_PORT      4578
#define GCUSR_NT_PORT      4579

#define GCNOD_BC_PORT      4588
#define GCNOD_NT_PORT      4589

#define GCNOD_C1_PORT      4591

#define MAX_GCCOR          32
#define MAX_GCTSK          256
#define MAX_GCSVR          5

#define     NM_TASK         0x0010
#define     NM_GROUP        0x0020
#define     NM_ERROR        0xFFFF

enum TSK_CMD{
    TSK_NONE   = 0,
    TSK_START  = 0x01,	
    TSK_EXIT   = 0x02,
    TSK_MSG    = 0x03,	
	
    TSK_TERM   = 0x11,
	TSK_STAT   = 0x22,
	TSK_MSGS   = 0x33,

	TSK_ERROR  = 0xFF,
};
enum GRP_CMD{
    GRP_NONE   = 0,
    GRP_START  = 0x101,
    GRP_OVER   = 0x102,
    GRP_MSG    = 0x103,

    GRP_TERM   = 0x111, 
	GRP_STAT   = 0x122,
	GRP_MSGS   = 0x133,
	GRP_ERROR  = 0x1FF,
};

enum ERR_CMD{
    ERR_CMD_NONE   = 0,
    ERR_CMD_MSG    = 0x1001,
};

#ifndef _NETMSG
#define _NETMSG
#define NETMSG_SIZE     512
typedef struct tagNETMSG
{
    UINT    msgId;
    DWORD   wParam;
	BYTE    pBuf_lParam[500]; 
	int     lParamExSz;
}NETMSG;
#else
#pragma message("GCDef.h, Warning: NETMSG alread define, be sure it was define as: struct tagNETMSG{ UINT msgId;DWORD  wParam;BYTE pBuf_lParam[500]; int lParamExSz; }. \
               \nGCDef.h, 警告:类型 NETMSG 已经定义过,请确保其定义为: struct tagNETMSG{ UINT msgId;DWORD  wParam;BYTE pBuf_lParam[500]; int lParamExSz; }.") 
#endif


enum TSKMSG_TYPE{
    TSKPROC_MSG   =   10,
    TSKPROC_START =   11,
    TSKPROC_STEP  =   12,
    TSKPROC_OVER  =   13,
	
	TSKEXE_OVER	  =   20,
	TSKEXE_EXIT	  =   40, // Auto Send This Message On Execute Exit.
};
#ifndef _TSKMSG
#define _TSKMSG
#define TSKMSG_SIZE     252
typedef struct tagTSKMSG
{
    WPARAM	wParam; // TSKMSG_TYPE 
	LPARAM	lParam; // if (lParamO==-1) the value in pBuf_lParamO
	BYTE	pBuf_lParam[244];    
}TSKMSG;
#else
#pragma message("GCDef.h, Warning: TSKMSG alread define, be sure it was define as: struct tagTSKMSG{ WPARAM wParam;LPARAM lParam;BYTE pBuf_lParam[244]; }. \
               \nGCDef.h, 警告:类型 TSKMSG 已经定义过,请确保其定义为: struct tagTSKMSG{ WPARAM wParam;LPARAM lParam;BYTE pBuf_lParam[244]; }.") 
#endif

#ifndef _TSKCMD
#define _TSKCMD
typedef struct tagTSKCMD
{
    char strCmd[240];
    char strPar[240];    
}TSKCMD;
#else
#pragma message("GCDef.h, Warning: TSKCMD alread define, be sure it was define as: struct tagTSKCMD{ char strCmd[240];char strPar[240]; }. \
               \nGCDef.h, 警告:类型 TSKCMD 已经定义过,请确保其定义为: struct tagTSKCMD{ char strCmd[240];char strPar[240]; }.") 
#endif

#ifndef _TSKINF
#define _TSKINF
#define TSKINF_SIZE    492
typedef struct tagTSKINF
{
    UINT tskId;
    UINT tskGrp;
	UINT tskSta;
	union{
		TSKMSG	tskMsg;
		TSKCMD	tskCmd;
	};
}TSKINF;
#else
#pragma message("GCDef.h, Warning: TSKINF alread define, be sure it was define as: struct tagTSKINF{ UINT tskId;char strCmd[240];char strPar[256] }. \
               \nGCDef.h, 警告:类型 TSKINF 已经定义过,请确保其定义为: struct tagTSKINF{ UINT tskId;char strCmd[240];char strPar[256] }.") 
#endif



#ifndef _GRPINF
#define _GRPINF
#define GRPINF_SIZE  256
typedef struct tagGRPINF
{
    UINT grpId;
    char strGrpPN[252];    
}GRPINF;
#else
#pragma message("GCDef.h, Warning: GRPINF alread define, be sure it was define as: struct tagGRPINF{ UINT grpId;char strGrpPN[252]; }. \
               \nGCDef.h, 警告:类型 GRPINF 已经定义过,请确保其定义为: struct tagGRPINF{ UINT grpId;char strGrpPN[252]; }.") 
#endif


#ifndef _GCSVRHDL
#define _GCSVRHDL
typedef struct tagGCSVRHDL
{
    HWND hMainWnd;
    UINT nPsID;
    UINT nPrjID;
    WORD nPortBc,nPortNt;
    BYTE pBuf_lParam[112];
}GCSVRHDL;
#else
#pragma message("GCDef.h, Warning: GCSVRHDL alread define, be sure it was define as: struct tagGCSVRHDL{ UINT nPsID;HWND hMainWnd;BYTE pBuf_lParam[120]; }. \
               \nGCDef.h, 警告:类型 GCSVRHDL 已经定义过,请确保其定义为: struct tagGCSVRHDL{ UINT nPsID;HWND hMainWnd;BYTE pBuf_lParam[120]; }.") 
#endif

#define     GSUSR_QUERY         "GCUsr_Query"
#define     GSSVR_ANSWER        "GCSvr_Answer"
#define     GSSVR_QUERY         "GCSvr_Query"
#define     GSNOD_ANSWER        "GCNod_Answer"
#define     GSUSR_TSKINF		"GCUsr_TskInf"

#define     LXSVR_QUERY         "LXSvr_Query"
#define     LXSVR_ANSWER        "LXSvr_Answer"

#ifndef _GC_QUERY_ANSWER_FRM
#define _GC_QUERY_ANSWER_FRM
enum USR_QUERY_ANSWER{
    QA_NONE = 0x00,
    QA_SVRS = 0x01,
    QA_NODS = 0x02,
    
    QA_TSKS = 0x10,
    QA_GRPS = 0x11,

	QA_USRS = 0x20,

    QA_CORS = 0x21, // Svr query Cores
    QA_STAT = 0x22,
    QA_ICMP = 0x23,

    QA_PRJ  = 0x1001,
    QA_FB   = 0x1002,
};
#define GC_QUERY_FRM_SIZE     64
typedef struct tagGC_QUERY_FRM
{
    char    strTag[32]; // "GCUsr_Query" ,"GCSvr_Query" ,"LXSvr_Query"
    WORD    qaType;     // QA_SVRS, QA_TSKS,QA_LXSVR,QA_GRPS
    WORD    usrPort;    
    char    strRev[28];
}GC_QUERY_FRM;
#define GS_ANSWER_FRM_SIZE     512
typedef struct tagGS_ANSWER_FRM
{
    char    strTag[32]; // "GCSvr_Answer","GCNod_Answer","LXSvr_Answer"
    WORD    qaType;     // QA_SVRS, QA_TSKS,QA_LXSVR,QA_GRPS
    WORD    answerSz;   
    BYTE    answer[476]; 
}GS_ANSWER_FRM;

#ifndef _GCSVRINF
#define _GCSVRINF
typedef struct tagGCSVRINF // size must be less 476
{
    UINT    ip,port;
    UINT    prjId;
}GCSVRINF;
#else
#pragma message("GCDef.h, Warning: GCSVRINF alread define, be sure it was define as: struct tagGCSVRINF{ UINT ip,port;UINT prjId; }. \
               \nGCDef.h, 警告:类型 GCSVRINF 已经定义过,请确保其定义为: struct tagGCSVRINF{ UINT ip,port;UINT prjId; }.") 
#endif

#ifndef _GCGRPSTA
#define _GCGRPSTA
enum GRP_STAT{
    GS_ERR   =   0,
    GS_END   =   1,
    GS_RUN   =   2,
    GS_WAIT  =   3,    
};
typedef struct tagGCGRPSTA // size must be less 476
{
    UINT ip,port;
    UINT prjId,grpId;
    UINT grpStat;
    char strRev[64];
}GCGRPSTA;
#else
#pragma message("GCDef.h, Warning: GCGRPSTA alread define, be sure it was define as: struct tagGCGRPSTA{ UINT ip,port;UINT prjId,grpId;char strStat[64]; }. \
               \nGCDef.h, 警告:类型 GCGRPSTA 已经定义过,请确保其定义为: struct tagGCGRPSTA{ UINT ip,port;UINT prjId,grpId;char strStat[64]; }.") 
#endif


#define GC_TSKINF_FRM_SIZE			520
typedef struct tagGS_TSKINF_FRM
{
    char    strTag[20]; // "GCUsr_TskInf",
	UINT    msgId;		// User Define
    DWORD   wParam;		// User Define
    TSKINF	tskInf;     // 492
}GS_TSKINF_FRM;
#else
#pragma message("GCDef.h, Warning: GC_QUERY_FRM alread define, be sure it was define as: struct tagGC_QUERY_FRM{ char strTag[32];; }. \
               \nGCDef.h, 警告:类型 GC_QUERY_FRM 已经定义过,请确保其定义为: struct tagGC_QUERY_FRM{ char strTag[32];; }.") 
#endif


#ifndef _GCCORINF
#define _GCCORINF
enum CORE_STAT{
    CS_IDL  =   0,
    CS_RUN  =   1,
    CS_ERR  =   2,
};
typedef struct tagGCCORINF
{
    UINT ip;
    WORD port,core;
    UINT corStat;    
}GCCORINF;
#else
#pragma message("GCDef.h, Warning: GCCORINF alread define, be sure it was define as: struct tagGCCORINF{ UINT ip,port;UINT corStat; }. \
               \nGCDef.h, 警告:类型 GCCORINF 已经定义过,请确保其定义为: struct tagGCCORINF{ UINT ip,port;UINT corStat; }.") 
#endif


#ifdef WIN32
inline char* itostr(int num)
{
    static char p[16];  p[8] = 0; char *Hex = "0123456789ABCDEFF";
    p[0] = Hex[num     & 0xF]; p[1] = Hex[num>>4  & 0xF];
    p[2] = Hex[num>>8  & 0xF]; p[3] = Hex[num>>12 & 0xF]; 
    p[4] = Hex[num>>16 & 0xF]; p[5] = Hex[num>>20 & 0xF]; 
    p[6] = Hex[num>>24 & 0xF]; p[7] = Hex[num>>28 & 0xF];
	return p;
}
///////////////////////////////////////
// For Ping Command.
inline u_short in_cksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;
    while( nleft > 1 )  { sum += *w++; nleft -= 2; }
    if( nleft == 1 ) {  u_short	u = 0; *(u_char *)(&u) = *(u_char *)w ; sum += u; }
	sum = (sum >> 16) + (sum & 0xffff);	
	sum += (sum >> 16);			 
	answer = ~sum;
	return (answer);
}

inline BOOL CanConnect(sockaddr_in addr)
{
    #define ICMP_ECHOREPLY	0
    #define ICMP_ECHOREQ	8
    // IP Header -- RFC 791
    typedef struct tagIPHDR
    {
	    u_char  VIHL;			// Version and IHL
	    u_char	TOS;			// Type Of Service
	    short	TotLen;			// Total Length
	    short	ID;				// Identification
	    short	FlagOff;		// Flags and Fragment Offset
	    u_char	TTL;			// Time To Live
	    u_char	Protocol;		// Protocol
	    u_short	Checksum;		// Checksum
	    struct	in_addr iaSrc;	// Internet Address - Source
	    struct	in_addr iaDst;	// Internet Address - Destination
    }IPHDR, *PIPHDR;
    // ICMP Header - RFC 792
    typedef struct tagICMPHDR
    {
	    u_char	Type;			// Type
	    u_char	Code;			// Code
	    u_short	Checksum;		// Checksum
	    u_short	ID;				// Identification
	    u_short	Seq;			// Sequence
	    char	Data;			// Data
    }ICMPHDR, *PICMPHDR;
    #define REQ_DATASIZE 32		// Echo Request Data size
    // ICMP Echo Request
    typedef struct tagECHOREQUEST
    {
	    ICMPHDR icmpHdr;
	    DWORD	dwTime;
	    char	cData[REQ_DATASIZE];
    }ECHOREQUEST, *PECHOREQUEST;
    // ICMP Echo Reply
    typedef struct tagECHOREPLY
    {
	    IPHDR	    ipHdr;
	    ECHOREQUEST	echoRequest;
	    char        cFiller[256];
    }ECHOREPLY, *PECHOREPLY;

    cprintf("Ping %d.%d.%d.%d ... \n",
        addr.sin_addr.S_un.S_un_b.s_b1,addr.sin_addr.S_un.S_un_b.s_b2,
        addr.sin_addr.S_un.S_un_b.s_b3,addr.sin_addr.S_un.S_un_b.s_b4 );

    struct sockaddr_in dest=addr; dest.sin_family = AF_INET; 
    SOCKET sockRaw = ::socket( AF_INET,SOCK_RAW,IPPROTO_ICMP ); if ( sockRaw==INVALID_SOCKET ){ cprintf("Ping Error @socket( AF_INET,SOCK_RAW,IPPROTO_ICMP ).\n"); return FALSE; }
    
    // Fill in echo request
    ECHOREQUEST echoReq; memset( &echoReq,0,sizeof(echoReq));
	echoReq.icmpHdr.Type		= ICMP_ECHOREQ;
	echoReq.icmpHdr.ID			= (USHORT)GetCurrentProcessId();
	echoReq.icmpHdr.Checksum	= in_cksum((u_short *)&echoReq, sizeof(ECHOREQUEST));
	if ( SOCKET_ERROR==::sendto( sockRaw,(LPSTR)&echoReq,sizeof(ECHOREQUEST),0,(struct sockaddr*)&dest,sizeof(dest) ) ){ ::closesocket( sockRaw ); cprintf("Ping Error @sendto( ... ).\n"); return FALSE; }
    if ( SOCKET_ERROR==::sendto( sockRaw,(LPSTR)&echoReq,sizeof(ECHOREQUEST),0,(struct sockaddr*)&dest,sizeof(dest) ) ){ ::closesocket( sockRaw ); cprintf("Ping Error @sendto( ... ).\n"); return FALSE; }

    fd_set readfds; readfds.fd_count = 1; readfds.fd_array[0] = sockRaw;
    struct timeval Timeout; Timeout.tv_usec = 0; Timeout.tv_sec = 2; 
	int ret = ::select( 1,&readfds,NULL,NULL,&Timeout );
    if ( ret==SOCKET_ERROR || ret==0 ){ ::closesocket( sockRaw ); cprintf("Ping Error @select( ... ).\n");  return FALSE; }

    ECHOREPLY echoReply; memset( &echoReply,0,sizeof(echoReply)); int nAddrLen = sizeof(dest);
	if ( SOCKET_ERROR==recvfrom( sockRaw,(LPSTR)&echoReply,sizeof(ECHOREPLY),0,(struct sockaddr*)&dest,&nAddrLen ) ){ ::closesocket( sockRaw ); cprintf("Ping Error @recvfrom( ... ).\n"); return FALSE; }
    ::closesocket( sockRaw );
    
    return echoReply.echoRequest.icmpHdr.ID==echoReq.icmpHdr.ID; 
}

inline BOOL CanConnect(int b1,int b2,int b3,int b4){ 
    sockaddr_in addr; memset(&addr,0,sizeof(addr)); addr.sin_addr.S_un.S_un_b.s_b1 = b1;  
    addr.sin_addr.S_un.S_un_b.s_b2 = b2; addr.sin_addr.S_un.S_un_b.s_b3 = b3; addr.sin_addr.S_un.S_un_b.s_b4 = b4;
    return CanConnect(addr);    
}
inline void PrintfErr(int errCode)
{
    LPVOID lpMsgBuf=NULL; char strMsg[512];
    ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,errCode,0,(LPTSTR)&lpMsgBuf,0,NULL );
    strcpy( strMsg,(LPTSTR)lpMsgBuf ); if (lpMsgBuf) ::LocalFree( lpMsgBuf );
    cprintf( "ErrCod=%d \nMessage= %s\n",errCode,strMsg );
}
inline void ReportErr(int errCode)
{
    LPVOID lpMsgBuf=NULL; char strMsg[512];
    ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,errCode,0,(LPTSTR)&lpMsgBuf,0,NULL );
    strcpy( strMsg,(LPTSTR)lpMsgBuf ); if (lpMsgBuf) ::LocalFree( lpMsgBuf );
    cprintf( "%s\n",strMsg );
    ::MessageBox( GetFocus(),strMsg,"Error",MB_OK );
}

inline int Send( SOCKET s,char *pBuf,int bufSize,int flag)
{
    int allSnd = 0,curSnd = 0, sndSz;
    while( allSnd<bufSize ){
        sndSz = (bufSize-allSnd)>4096?4096:(bufSize-allSnd);
        curSnd = ::send( s,pBuf+allSnd,sndSz,0 );
        if ( (curSnd==0||curSnd==SOCKET_ERROR) && WSAGetLastError()!=WSAETIMEDOUT )  break;
        allSnd += curSnd;        
    }
    return allSnd;
}
inline int Recv( SOCKET s,char *pBuf,int bufSize,int flag)
{
    int allRec = 0,curRec = 0, recSz;
    while( allRec<bufSize ){
        recSz = (bufSize-allRec)>4096?4096:(bufSize-allRec);
        curRec = ::recv( s,pBuf+allRec,recSz,0 );
        if ( (curRec==0||curRec==SOCKET_ERROR) && WSAGetLastError()!=WSAETIMEDOUT )  break;
        allRec += curRec;        
    }
    return allRec==0?SOCKET_ERROR:allRec;
}

inline void Bc2Usr(UINT msgType,WPARAM wParam,TSKINF *pTskInf,WORD port=GCUSR_BC_PORT )
{
	sockaddr_in addr; memset( &addr,0,sizeof(addr) );
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    addr.sin_port        = htons( port );
    SOCKET hSocketSnd    = socket( AF_INET,SOCK_DGRAM,0 );
    BOOL optval = TRUE; int addrlen = sizeof(addr); 
    setsockopt( hSocketSnd,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));    
    
	GS_TSKINF_FRM tskFrm; 
	strcpy( tskFrm.strTag,GSUSR_TSKINF );
	tskFrm.msgId  = msgType;
	tskFrm.wParam = wParam;
	tskFrm.tskInf = *pTskInf;
	
	sendto( hSocketSnd,(char*)&tskFrm,GC_TSKINF_FRM_SIZE,0,(sockaddr*)&addr,addrlen ); 
    closesocket( hSocketSnd );
}

inline void Bc2Svr(UINT msgType,WPARAM wParam,LPARAM lParam,LPARAM lParam2=0,WORD port=GCSVR_BC_PORT) 
{
	sockaddr_in addr; memset( &addr,0,sizeof(addr) );
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    addr.sin_port        = htons( port );

    SOCKET hSocketSnd    = ::socket( AF_INET,SOCK_DGRAM,0 );
    BOOL optval = TRUE; int addrlen = sizeof(addr); 
    ::setsockopt( hSocketSnd,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));    
    
	GC_QUERY_FRM usrQuery; memset( &usrQuery,0,sizeof(usrQuery) ); 
	strcpy( usrQuery.strTag,GSUSR_QUERY );
	usrQuery.qaType  = msgType;
	usrQuery.usrPort = wParam ; // port
    *((UINT*)(usrQuery.strRev)  )  = lParam ;
    *((UINT*)(usrQuery.strRev+4))  = lParam2;

    ::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen ); Sleep(8);
	::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen );
    // For IP by IP
    char str[64]; ::gethostname( str,64 );
    hostent* pEnt = ::gethostbyname(str);
    for (int i=0;i<8;i++ ){
        if ( pEnt->h_addr_list[i]==NULL ) break;
        BYTE*pIp = (BYTE*)pEnt->h_addr_list[i];
        addr.sin_addr.S_un.S_un_b.s_b1 = pIp[0];
        addr.sin_addr.S_un.S_un_b.s_b2 = pIp[1];
        addr.sin_addr.S_un.S_un_b.s_b3 = pIp[2];
        ::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen );
    }
	
    ::closesocket( hSocketSnd );
}

inline void Bc2Nds(WORD qaType,WORD myPort,BYTE *myIP=NULL,WORD port=GCNOD_BC_PORT) 
{   
    sockaddr_in addr; memset( &addr,0,sizeof(addr) );
    BOOL optval = TRUE; int addrlen = sizeof(addr); 
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    addr.sin_port        = htons( port );
    SOCKET hSocketSnd    = ::socket( AF_INET,SOCK_DGRAM,0 );
    ::setsockopt( hSocketSnd,SOL_SOCKET,SO_BROADCAST,(char FAR *)&optval,sizeof(optval));    

    GC_QUERY_FRM usrQuery; memset( &usrQuery,0,sizeof(usrQuery) ); 
    strcpy( usrQuery.strTag,GSSVR_QUERY );
    usrQuery.qaType  = qaType;
    usrQuery.usrPort = myPort;
	if ( myIP ){
		addr.sin_addr.S_un.S_un_b.s_b1 = myIP[0];
		addr.sin_addr.S_un.S_un_b.s_b2 = myIP[1];
		addr.sin_addr.S_un.S_un_b.s_b3 = myIP[2];        
		::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen ); Sleep(32); 
		::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen ); 		
	}else{
		::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen ); Sleep(8); 
		::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen );

        // For IP by IP
        char str[64]; ::gethostname( str,64 );
        hostent* pEnt = ::gethostbyname(str);
        for (int i=0;i<8;i++ ){
            if ( pEnt->h_addr_list[i]==NULL ) break;
            BYTE*pIp = (BYTE*)pEnt->h_addr_list[i];
            addr.sin_addr.S_un.S_un_b.s_b1 = pIp[0];
            addr.sin_addr.S_un.S_un_b.s_b2 = pIp[1];
            addr.sin_addr.S_un.S_un_b.s_b3 = pIp[2];
            ::sendto( hSocketSnd,(char*)&usrQuery,GC_QUERY_FRM_SIZE,0,(sockaddr*)&addr,addrlen );
            cprintf("[BROADCAST %d]SendTo %d@%d.%d.%d.%d Msg=%s Sz=%d\n",GetTickCount(),ntohs(addr.sin_port),addr.sin_addr.S_un.S_un_b.s_b1,addr.sin_addr.S_un.S_un_b.s_b2,addr.sin_addr.S_un.S_un_b.s_b3,addr.sin_addr.S_un.S_un_b.s_b4,usrQuery.strTag,GC_QUERY_FRM_SIZE );
        }
	}
    ::closesocket( hSocketSnd );
    cprintf("[BROADCAST %d]SendTo %d@%d.%d.%d.%d Msg=%s Sz=%d\n",GetTickCount(),ntohs(addr.sin_port),addr.sin_addr.S_un.S_un_b.s_b1,addr.sin_addr.S_un.S_un_b.s_b2,addr.sin_addr.S_un.S_un_b.s_b3,addr.sin_addr.S_un.S_un_b.s_b4,usrQuery.strTag,GC_QUERY_FRM_SIZE );
}

inline TSKINF *Get_TskInf( BYTE *pTskInf,int idx ){ return pTskInf?(TSKINF*)(pTskInf+idx*sizeof(TSKINF)*2):NULL; }
inline int Get_PIDIdx(BYTE *pPID,UINT pid){
    for ( int idx=0;idx<MAX_GCCOR;idx++ ){ BYTE *p = pPID+sizeof(UINT)*idx; if ( *((UINT*)p)==pid ) return idx; }
    return -1;        
}
#endif


#endif

