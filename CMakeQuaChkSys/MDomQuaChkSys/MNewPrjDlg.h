#pragma once
#include "afxcmn.h"
//#include "MyFunctions.h"
#include <vector>
#include "afxwin.h"
//#include "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\include\afxwin.h"
using namespace std;
// CMNewPrjDlg dialog

class CMNewPrjDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMNewPrjDlg)

public:
	CMNewPrjDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMNewPrjDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEW_PRJ };
#endif
protected:
	void InitItemPos();
	void ResetItemPos();
	void InitImgList();
	void ClearDomFile();
	void ClearRefFile();
public:
	CString GetPrjFilePath();
	vector<CString> GetDomFilePath();
	vector<CString> GetRefFilePath();
protected:
	POINT old;
	vector<CString> m_vecDomFilePath;
	vector<CString> m_vecRefFilePath;
	vector<bool> m_vecDomValidInfo;
	vector<bool> m_vecRefValidInfo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPrjFilePath;
	CString m_strDomFileFolder;
	CString m_strRefFileFolder;
	CListCtrl m_ListImgInfoInNewPrj;
	BOOL m_bDeleteInvalidImg;
	BOOL m_bAddTifInNewPrj;
	BOOL m_bAddPixInNewPrj;
	BOOL m_bAddImgInNewPrj;

	afx_msg void OnEnChangeMfceditbrowseFolderDom();
	afx_msg void OnEnChangeMfceditbrowseFolderRef();
	afx_msg void OnLvnGetdispinfoListImginfo(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnBnClickedOk();
	CEdit m_EditNumDom;
	CEdit m_EditNumRef;
	CButton m_CheckDeleteInvalidImg;
	CString m_strLookingfor;
	afx_msg void OnBnClickedLookingfor();
	afx_msg void OnBnClickedDeletefiles();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
