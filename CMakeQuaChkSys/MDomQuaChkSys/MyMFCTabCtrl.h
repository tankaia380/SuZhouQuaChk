#pragma once


// CMyMFCTabCtrl
#include "MyFunctions.h"
class CMyMFCTabCtrl : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CMyMFCTabCtrl)

public:
	CMyMFCTabCtrl();
	virtual ~CMyMFCTabCtrl();
	vector<MstuTieChkModel> m_vecTieChkModelInfo;
	vector<MstuGcpChkModel> m_vecGcpChkModelInfo;
	MstuMchModel m_stuPointChkInfo;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDClickModelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDClickPointList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNClickModelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetdispinfoListModelinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetdispinfoListPointinfo(NMHDR *pNMHDR, LRESULT *pResult);
public:
};


