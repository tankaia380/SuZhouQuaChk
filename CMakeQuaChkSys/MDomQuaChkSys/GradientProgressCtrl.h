#if !defined(AFX_ENHPROGRESSCTRL_H__12909D73_C393_11D1_9FAE_8192554015AD__INCLUDED_)
#define AFX_ENHPROGRESSCTRL_H__12909D73_C393_11D1_9FAE_8192554015AD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EnhProgressCtrl.h : header file
//



// GradientProgressCtrl.h : header file
//
// Written by matt weagle (matt_weagle@hotmail.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
//
// Thanks to Chris Maunder (Chris.Maunder@cbr.clw.csiro.au) for the 
// foregoing disclaimer.


#include "MemDC.h"

/////////////////////////////////////////////////////////////////////////////


/**
* @brief ����������
*
* �ṩ����������ӿڣ�����ӳ��ǰ�㷨�Ľ���ֵ
*/
class  CProcessBase
{
public:
	/**
	* @brief ���캯��
	*/
	CProcessBase()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

	/**
	* @brief ��������
	*/
	virtual ~CProcessBase() {}

	/**
	* @brief ���ý�����Ϣ
	* @param pszMsg         ������Ϣ
	*/
	virtual void SetMessage(const char* pszMsg) = 0;

	/**
	* @brief ���ý���ֵ
	* @param dPosition      ����ֵ
	* @return �����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
	*/
	virtual bool SetPosition(double dPosition) = 0;

	/**
	* @brief ������ǰ��һ��������true��ʾ������false��ʾȡ��
	* @return �����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
	*/
	virtual int StepIt() = 0;

	/**
	* @brief ���ý��ȸ���
	* @param iStepCount     ���ȸ���
	*/
	virtual void SetStepCount(int iStepCount)
	{
		ReSetProcess();
		m_iStepCount = iStepCount;
	}

	/**
	* @brief ��ȡ������Ϣ
	* @return ���ص�ǰ������Ϣ
	*/
// 	string GetMessage()
// 	{
// 		return m_strMessage;
// 	}

	/**
	* @brief ��ȡ����ֵ
	* @return ���ص�ǰ����ֵ
	*/
	double GetPosition()
	{
		return m_dPosition;
	}

	/**
	* @brief ���ý�����
	*/
	void ReSetProcess()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
		m_bIsContinue = true;
	}

// 	/*! ������Ϣ */
// 	string m_strMessage;
	/*! ����ֵ */
	double m_dPosition;
	/*! ���ȸ��� */
	int m_iStepCount;
	/*! ���ȵ�ǰ���� */
	int m_iCurStep;
	/*! �Ƿ�ȡ����ֵΪfalseʱ��ʾ����ȡ�� */
	bool m_bIsContinue;
};

class CConsoleProcess : public CProcessBase
{
public:
	/**
	* @brief ���캯��
	*/
	CConsoleProcess()
	{
		m_dPosition = 0.0;
		m_iStepCount = 100;
		m_iCurStep = 0;
	};

	/**
	* @brief ��������
	*/
	~CConsoleProcess()
	{
		//remove(m_pszFile);  
	};

	/**
	* @brief ���ý�����Ϣ
	* @param pszMsg         ������Ϣ
	*/
	void SetMessage(const char* pszMsg)
	{
		//m_strMessage = pszMsg;
		printf("%s\n", pszMsg);
	}

	/**
	* @brief ���ý���ֵ
	* @param dPosition      ����ֵ
	* @return �����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
	*/
	bool SetPosition(double dPosition)
	{
		m_dPosition = dPosition;
		TermProgress(m_dPosition);
		m_bIsContinue = true;
		return true;
	}

	/**
	* @brief ������ǰ��һ��
	* @return �����Ƿ�ȡ����״̬��trueΪ��ȡ����falseΪȡ��
	*/
	int StepIt()
	{
		m_iCurStep++;
		m_dPosition = m_iCurStep*1.0 / m_iStepCount;

		TermProgress(m_dPosition);
		m_bIsContinue = true;
		return true;
	}

private:
	void TermProgress(double dfComplete)
	{
		static int nLastTick = -1;
		int nThisTick = (int)(dfComplete * 40.0);

		nThisTick = min(40, max(0, nThisTick));

		// Have we started a new progress run?    
		if (nThisTick < nLastTick && nLastTick >= 39)
			nLastTick = -1;

		if (nThisTick <= nLastTick)
			return;

		while (nThisTick > nLastTick)
		{
			nLastTick++;
			if (nLastTick % 4 == 0)
				fprintf(stdout, "%d", (nLastTick / 4) * 10);
			else
				fprintf(stdout, ".");
		}

		if (nThisTick == 40)
			fprintf(stdout, " - done.\n");
		else
			fflush(stdout);
	}
};


// CGradientProgressCtrl window

class CGradientProgressCtrl : public CProgressCtrl
{
// Construction
public:
	CGradientProgressCtrl();

protected:
	void DrawGradient(CPaintDC *pDC, const RECT &rectClient, const int &nMaxWidth, const BOOL &bVertical);	
	int m_nLower, m_nUpper, m_nStep, m_nCurrentPosition;
	COLORREF m_clrStart, m_clrEnd, m_clrBkGround, m_clrText;
	BOOL m_bShowPercent;

// Attributes
public:
// Attributes
	
	void SetRange(int nLower, int nUpper);
	void SetRange32(int nLower, int nUpper);
	int SetPos(int nPos);
	int SetStep(int nStep);
	int StepIt(void);
// Operations
public:
	
	// Set Functions
	void SetTextColor(COLORREF color)	{m_clrText = color;}
	void SetBkColor(COLORREF color)		{m_clrBkGround = color;}
	void SetStartColor(COLORREF color)	{m_clrStart = color;}
	void SetEndColor(COLORREF color)	{m_clrEnd = color;}

	// Show the percent caption
	void ShowPercent(BOOL bShowPercent = TRUE)	{m_bShowPercent = bShowPercent;}
	
	// Get Functions
	COLORREF GetTextColor(void)	{return m_clrText;}
	COLORREF GetBkColor(void)	{return m_clrBkGround;}
	COLORREF GetStartColor(void){return m_clrStart;}
	COLORREF GetEndColor(void)	{return m_clrEnd;}


// Overrides
	// ClaMyizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGradientProgressCtrl)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGradientProgressCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGradientProgressCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

//CMFCGdalProgressCtrl
class CMFCGdalProgressCtrl :
	public CProcessBase,
	public CGradientProgressCtrl
{
public:
	CMFCGdalProgressCtrl();
	~CMFCGdalProgressCtrl();
	/**
	* @brief ���ý�����Ϣ
	* @param pszMsg         ������Ϣ
	*/
	void SetMessage(const char* pszMsg){};

	/**
	* @brief ���ý���ֵ
	* @param dPosition      ����ֵ
	*/
	bool SetPosition(double dPosition);

	/**
	* @brief ������ǰ��һ��
	*/
	int StepIt();

public:
	
private:

};

#ifndef STD_API  
#define STD_API __stdcall  
#endif  
int STD_API updateProgress(double, const char *, void*);

#endif // !defined(AFX_ENHPROGRESSCTRL_H__12909D73_C393_11D1_9FAE_8192554015AD__INCLUDED_)