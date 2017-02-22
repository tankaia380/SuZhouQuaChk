#ifndef M_Image_h
#define M_Image_h
#pragma once
#include "MDataTypeBase.h"

#ifdef  MIMAGE_EXPORTS
#define MIMAGE __declspec(dllexport)
#else
#define MIMAGE __declspec(dllimport)
#endif

typedef int (*pCallBackProFun)(double dStp, const char *pStrInfo, void *pProgressCtrl);


class MIMAGE CMImage
{
public:
	CMImage();
	~CMImage();
	bool		OpenImg(const char* strImgPath, bool bReadOnly = true);
	void		CloseImg();
	bool        SaveImg(const char* strSavePath, int nCols, int nRows, int nBands, BYTE*pMem, const char *pszFormat  = "JPEG");
	bool        SavePointShp(const char* strSavePath, MstuMchModel moeldInfo, const char *prj);
	bool        IsOpen();
	int			GetBands();
	int			GetCols();
	int			GetRows();
	int			GetBufSize();

protected:
	void InitImg();
protected:
	int m_nBands;
	int m_nCols;
	int m_nRows;
	MenmImageBufType m_eBufType;
	char m_strImgFilePath[MAX_SIZE_FILE_PATH];
	void *m_pDataSet;

};

class MIMAGE CMImage4Check :
	public CMImage
{
public:
	CMImage4Check();
	~CMImage4Check();
	bool OpenImage(const char *strPath, bool bReadOnly = true);

	double		GetResolutionX();
	double		GetResolutionY();
	MstuGeoInfo GetGeoInfo();
	double GetBandMaxValue(int nBandIdx, bool bFast = false);
	double GetBandMinValue(int nBandIdx, bool bFast = false);
	double GetBandMeanValue(int nBandIdx, bool bFast = false);
	double GetBandStaDevValue(int nBandIdx, bool bFast = false);
	int * GetBandHistogram(int nBandIdx, double dMax, double dMin, bool bFast = false);
	int * GetColorMap16To8(bool bFast = false);
	bool BuildPryamid(bool bReBuild = false, MenmResampleMode = RESMODE_NEARST, int nBottomSize = 64, pCallBackProFun pFun = NULL, void *pProgressCtrl = NULL);
	void ReadTfwInfo(int nTskId, int nTskNum, bool bSave2File = true, pCallBackProFun pFun = NULL, void *pPara3OfpFun = NULL);
	bool ReadImg(int stCol, int stRow, int nCols, int nRows, int nMemSizeX, int nMemSizeY, void *pMem);
	bool ReadImg8(int stCol, int stRow, int nCols, int nRows, int nMemSizeX, int nMemSizeY, BYTE *pMem);
	void ReadData4Texture(float stCol, float stRow, int nMemSizeX, int nMemSizeY, float fScale, int *pShowBandOrder, BYTE *pData4Tex);
	void SetImgType(MenmImageType eType);
	void SetImgPath(const char* strImgPath);
	void SetTfwInfo(vector<Point2D> vecVertexValid);
	char *GetProjectionRef();
	MenmImageType GetImgType();
	char* GetImgPath();
	Point2D ImgPt2Geo(Point2D ptImg);
	Point2D GeoPt2Img(Point2D ptGeo);


protected:
	void ReadGeoInfo();
	bool ScanValidCorners(vector<Point2D>&pCornersPt, int nTskId, int nTskNum, int nBlockSize = 100, pCallBackProFun pFun = NULL, void* pPara3OfpFun = NULL);

protected:
	MenmImageType m_eImgType;
	MstuGeoInfo m_stuGeoInfo;
	int m_pColorMap16To8[MAX_SIZE_BUF16];
	char m_strWKTInfo[MAX_SIZE_FILE_LINE];
	char m_strStasticXmlPath[MAX_SIZE_FILE_PATH];
	char m_strColorMapPath[MAX_SIZE_FILE_PATH];
	char m_strTfwInfoPath[MAX_SIZE_FILE_PATH];
};




#endif

