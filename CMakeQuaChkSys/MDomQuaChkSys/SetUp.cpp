// SetUp.cpp : 实现文件
//

#include "stdafx.h"
#include "MDomQuaChkSys.h"
#include "SetUp.h"
#include "afxdialogex.h"


// CSetUp 对话框

IMPLEMENT_DYNAMIC(CSetUp, CDialogEx)

CSetUp::CSetUp(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetUp::IDD, pParent)
	, m_nThreodCir(20)
	, m_nThreoPtNum(3000)
	, m_nThreodPix(30)
	, m_nThreoPtNumConrol(10000)
{

}

CSetUp::~CSetUp()
{
}

void CSetUp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_THREOD_CIR, m_nThreodCir);
	DDX_Text(pDX, IDC_THREOD_PTNUM, m_nThreoPtNum);
	DDX_Text(pDX, IDC_THREOD_PIX, m_nThreodPix);
	DDX_Text(pDX, IDC_THREOD_PTNUMControl, m_nThreoPtNumConrol);
}


BEGIN_MESSAGE_MAP(CSetUp, CDialogEx)
END_MESSAGE_MAP()


// CSetUp 消息处理程序
