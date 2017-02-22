#pragma once


// CSetUp 对话框

class CSetUp : public CDialogEx
{
	DECLARE_DYNAMIC(CSetUp)

public:
	CSetUp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetUp();

// 对话框数据
	enum { IDD = IDD_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nThreodCir;
//	CString m_nThreoPix;
	int m_nThreoPtNum;
	int m_nThreodPix;
	int m_nThreoPtNumConrol;
};
