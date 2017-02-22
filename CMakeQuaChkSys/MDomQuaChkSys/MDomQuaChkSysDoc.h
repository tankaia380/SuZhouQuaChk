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

// MDomQuaChkSysDoc.h : CMDomQuaChkSysDoc 类的接口
//


#pragma once

#include "MyFunctions.h"
#include "MyMatchPointChartDlg.h"
class CMainFrame;
class CMDomQuaChkSysView;
class CMDomQuaChkSysDoc : public CDocument
{
protected: // 仅从序列化创建
	CMDomQuaChkSysDoc();
	DECLARE_DYNCREATE(CMDomQuaChkSysDoc)
	bool IsProcessRunning(CString strExeName);
	bool StartGCProcess();
	void SendTask2GCSvr();
	bool SaveReport();
	//bool SavePointShpFiles();
	// 特性
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
	// 操作
public:
	void ClearData()
	{
		vector<CMImage4Check>().swap(m_vecDomInfo);
		vector<CMImage4Check>().swap(m_vecRefInfo);
		m_strPtjFilePath.Empty();
	}
	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// 实现
public:
	virtual ~CMDomQuaChkSysDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
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
