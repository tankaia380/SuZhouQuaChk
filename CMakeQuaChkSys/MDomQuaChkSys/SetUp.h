#pragma once


// CSetUp �Ի���

class CSetUp : public CDialogEx
{
	DECLARE_DYNAMIC(CSetUp)

public:
	CSetUp(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetUp();

// �Ի�������
	enum { IDD = IDD_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nThreodCir;
//	CString m_nThreoPix;
	int m_nThreoPtNum;
	int m_nThreodPix;
	int m_nThreoPtNumConrol;
};
