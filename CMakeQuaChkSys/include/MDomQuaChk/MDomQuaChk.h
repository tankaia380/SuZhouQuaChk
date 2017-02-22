#ifndef M_DomQuaChk_h
#define M_DomQuaChk_h
#pragma once
#include <afx.h>  
#include "MImage/MImage.h"
#include <vector>
using namespace std;

#ifdef  MDOMQUACHK_EXPORT
#define MDOMQUACHK __declspec(dllexport)
#else
#define MDOMQUACHK __declspec(dllimport)
#endif

//typedef void(*pCallBackProFun)(double dPercent, const char *strProInfo);
typedef int(*pCallBackProFun)(double dStp, const char *pStrInfo, void *pProgressCtrl);
#ifndef _global_functions
#define _global_functions
MDOMQUACHK CString FunGetSysExePath();
MDOMQUACHK CString FunGetSyslogPath();
MDOMQUACHK CString FunGetSysTimeStr();
MDOMQUACHK void    FunGetSysTimeDat(short *pTime);
MDOMQUACHK long    FunGetFileSize(CString strFilePath);
MDOMQUACHK void    FunOutPutLog2File(CString strLogInfo);
MDOMQUACHK CString FunGetFileFolder(CString strFullPath);
MDOMQUACHK CString FunGetFileName(CString strFullPath, bool bAddExt = false);
MDOMQUACHK CString FunGetFileExt(CString strFullPath);
MDOMQUACHK void	   FunSearchFile(CString strFolder, CString strExt, vector<CString> &vecFilePath);
MDOMQUACHK void	   FunCreateDir4Path(CString strFullPath);
MDOMQUACHK vector<CString> FunStringTok(CString strFull, CString strTok);
MDOMQUACHK void    FunSaveShpFile(CString strSavePath, void *pGeo);
#endif // !_global_functions

class MDOMQUACHK CMDomQuaChk
{
public:
	CMDomQuaChk();
	~CMDomQuaChk();

	bool SendData2Prj(vector<CMImage4Check> &vecDomImgs, vector<CMImage4Check> &vecRefImgs);
	bool SavePrj2File(const char *strPrjPath);
	bool LoadPrj4File(const char *strPrjPath);
	void ClearPrjInfo();
	void BulidPyramids(bool bReBuild = false, pCallBackProFun pFun = NULL, void *pPara3OfpFun = NULL);
	void CatchTfwInfo(bool bSave2File = true, pCallBackProFun pFun = NULL, void *pPara3OfpFun = NULL);
	void BuildColorMap(bool bReBuild = false, bool bFast = true, pCallBackProFun pFun = NULL, void *pPara3OfpFun = NULL);
	void ImageMatch();
	bool fIsTiemodelAndGcpExist();   //add by tk
	bool LoadModelAndGcpFile(pCallBackProFun pFun = NULL, void *pPara3 = NULL);  //add by tk

	void MatchCheck(pCallBackProFun pFun, void *pPara3);
	int GetDomNum();
	int GetRefNum();
	vector<CMImage4Check> GetAllDomInfo();
	vector<CMImage4Check> GetAllRefInfo();
	CMImage4Check GetDomInfo(int nIdx);
	CMImage4Check GetRefInfo(int nIdx);
	CString GetMatchFolder();
	CString GetGTskFilePath();


	CString GetTieChkMdlPath(int nIdx);   //add by tk
	CString GetGcpChkMdlPath(int nIdx);  //add by tk
	int     m_nThreodPix;                //add by tk
	int     m_nThreodPtNumTie;           //add by tk
	int     m_nThreodPtNumControl;       //add by tk
	int     m_nThreodCir;                //add by tk

	int GetTskNum();
	vector<MstuGcpChkModel> GetGcpModelInfo();
	vector<MstuTieChkModel> GetTieModelInfo();
	vector<vector<Point2D>> GetGcpModelArea();
	vector<vector<Point2D>> GetTieModelArea();
	vector<int> GetModelImgIdx(int nModelIdx, MenmModelType eModelType);
	MstuMchModel GetCheckModelPoint(int nModelIdx, MenmModelType eType);
	inline BOOL IsExist(LPCSTR lpstrPathName)   //判断文件是否存在 added by tk
	{
		WIN32_FIND_DATA fd; HANDLE hFind = INVALID_HANDLE_VALUE;
		hFind = ::FindFirstFile(lpstrPathName, &fd);
		if (hFind == INVALID_HANDLE_VALUE) return FALSE;
		::FindClose(hFind); return TRUE;
	}
protected:
	bool WritePrjXml();
	bool ReadPrjXml();
	void CalMatchMatrix();
	void CreateMatchTsk(bool bCheckExe = true, MenmMatchExe eMatchExe = MATCH_HARRIS);
	void CreateChkModlePath();   //added by tk
	void ClearMatchInfo();
	void ClearCheckInfo();
	bool ReadTieCheckFile(int nIdx, MstuMchModel &model);
	bool ReadGcpCheckFile(int nIdx, MstuMchModel &model);
	bool CheckTieMchFile(pCallBackProFun pFun = NULL, void *pPara3 = NULL);
	bool CheckCtlMchFile(pCallBackProFun pFun = NULL, void *pPara3 = NULL);
	void CheckGcpFile(pCallBackProFun pFun = NULL, void *pPara3 = NULL);
	void DeleteMatchErr(MstuMchModel &model);
	void DeleteCloudPoints(MstuMchModel &model);   //add by tk


	void MchModel2TieModel(MstuMchModel stuModel, MstuTieChkModel &TieModel);
	void MchModel2GcpModel(MstuMchModel stuModel, MstuGcpChkModel &GcpModel);
// 	void TieMchModelThining(vector<CString> vecMchLint, vector<CString>& vecLineStay, int nNumGet = 3000);
// 	void GcpChkModelThining(vector<CString> vecGcpLine, vector<CString>& vecLineStay, MstuGcpChkModel &GcpModel, int nNumGet=3000);
	void MchModelThining(MstuMchModel &MchModel, int nPtNumStay = 5000);
	CString m_strPrjPath;
	vector<CMImage4Check> m_vecDomImgs;
	vector<CMImage4Check> m_vecRefImgs;
	vector<CString> m_vecTskPath;
	vector<CString> m_vecTieModelPath;
	vector<CString> m_vecCtlModelPath;
	vector<CString> m_vecTieChkMdlPath;
	vector<CString> m_vecCtlChkMdlPath;
	vector<CString> m_vecGcpChkFilePath;
	vector<vector<int>> m_vvImgRefIdx;
	vector<MstuTieChkModel> m_vecTieChkModelInfo;
	vector<MstuGcpChkModel> m_vecGcpChkModelInfo;
	vector<vector<Point2D>> m_vvAreaPtTieModel;
	vector<vector<Point2D>> m_vvAreaPtGcpModel;
	CString m_strMatchFolder;
	CString m_strGTskFilePath;
	BYTE* m_pTieMatchMatrix;
	BYTE* m_pCtlMatchMatrix;

};

#endif