#pragma once


// CMyMFCPropertyGridCtrl

class CMyMFCPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CMyMFCPropertyGridCtrl)

public:
	CMyMFCPropertyGridCtrl();
	virtual ~CMyMFCPropertyGridCtrl();
	void SetLeftColWidth(int nWidth)
	{
		HDITEM item;
		item.cxy = nWidth;
		item.mask = HDI_WIDTH;
		GetHeaderCtrl().SetItem(0, new HDITEM(item));
	//	m_nLeftColumnWidth = nWidth;
	}
protected:
	DECLARE_MESSAGE_MAP()
};


