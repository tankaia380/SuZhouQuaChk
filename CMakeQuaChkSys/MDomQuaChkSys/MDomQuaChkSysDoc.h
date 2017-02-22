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

// MDomQuaChkSysDoc.h : CMDomQuaChkSysDoc ��Ľӿ�
//


#pragma once

#include "MyFunctions.h"
#include "MyMatchPointChartDlg.h"
class CMainFrame;
class CMDomQuaChkSysView;
class CMDomQuaChkSysDoc : public CDocument
{
protected: // �������л�����
	CMDomQuaChkSysDoc();
	DECLARE_DYNCREATE(CMDomQuaChkSysDoc)
	bool IsProcessRunning(CString strExeName);
	bool StartGCProcess();
	void SendTask2GCSvr();
	bool SaveReport();
	//bool SavePointShpFiles();
	// ����
public:
	CMainFrame* m_pMain;
	CMDomQuaChkSysView* m_pView;
	CMDomQuaChk m_DomChkPrj;
	CString m_strPtjFilePath;
	CString m_strReportPath;
	CString m_strCqViewImgPath;
	CString m_strTieViewImgPath;
	CString m_strGcpViewImgPath;
	vector<CMImage4Check> m_vecDomInfo;
	vector<CMImage4Check> m_vecRefInfo;
	bool m_bDataReady;
	bool m_bIsRunning;
	bool m_bSaveImgList;
	bool m_bIsIdentify;
	MenmModelType m_eModelType;
	CMyMatchPointChartDlg *m_pChartDlg;

	float m_fTieErrThresold;
	float m_fGcpErrThresold;
	// ����
public:
	void ClearData()
	{
		vector<CMImage4Check>().swap(m_vecDomInfo);
		vector<CMImage4Check>().swap(m_vecRefInfo);
		m_strPtjFilePath.Empty();
	}
	// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// ʵ��
public:
	virtual ~CMDomQuaChkSysDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnButtonNewPrj();
	afx_msg void OnButtonAutoMatch();
	afx_msg void OnUpdateButtonAutoMatch(CCmdUI *pCmdUI);
	afx_msg void OnButtonOpenPrj();
	afx_msg void OnButtonQuaChk();
	afx_msg void OnUpdateButtonQuaChk(CCmdUI *pCmdUI);
	afx_msg void OnButtonCheckTie();
	afx_msg void OnUpdateButtonCheckTie(CCmdUI *pCmdUI);
	afx_msg void OnButtonCheckCtl();
	afx_msg void OnUpdateButtonCheckCtl(CCmdUI *pCmdUI);
	afx_msg void OnButtonSaveReport();
	afx_msg void OnUpdateButtonSaveReport(CCmdUI *pCmdUI);
	afx_msg void OnEditTieThresold();
	afx_msg void OnUpdateEditGcpThresold(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditTieThresold(CCmdUI *pCmdUI);
	afx_msg void OnEditGcpThresold();
	afx_msg void OnUpdateCheckCqImg(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCheckTieImg(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCheckGcpImg(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonNewPrj(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonOpenPrj(CCmdUI *pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnCheckSaveImglist();
	afx_msg void OnUpdateCheckSaveImglist(CCmdUI *pCmdUI);
	afx_msg void OnSetup();
};
