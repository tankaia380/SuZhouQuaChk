// MDomQuaChk.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MDomQuaChk/MDomQuaChk.h"
#include "xml/pugixml.hpp"
#include <omp.h>
#include <Shlwapi.h>
#include "gdal201/gdal.h"
#include "gdal201/gdal_priv.h"
#include "gdal201/ogrsf_frmts.h"
#include "gdal201/ogr_spatialref.h"
#define MAX_SIZE_LOG_FILE (1024*1024)

CString FunGetSysExePath()
{
	char strExePath[MAX_SIZE_FILE_PATH];
	memset(strExePath, 0, MAX_SIZE_FILE_PATH);
	GetModuleFileName(NULL, strExePath, sizeof(strExePath));
	return strExePath;
}

CString FunGetSyslogPath()
{
	CString strLogPath = FunGetSysExePath();
	int np1 = strLogPath.ReverseFind('.');
	strLogPath = strLogPath.Left(np1);
	strLogPath += ".log";
	return strLogPath;
}

CString FunGetSysTimeStr()
{
	short dTime[8]; FunGetSysTimeDat(dTime);
	char strTime[100], strTemp[10];
	if (dTime[0] < 1000) sprintf(strTemp, "0%d-", dTime[0]);
	else sprintf(strTemp, "%d-", dTime[0]);
	sprintf(strTime, "%s", strTemp);//Y-
	if (dTime[1] < 10) sprintf(strTemp, "0%d-", dTime[1]);
	else sprintf(strTemp, "%d-", dTime[1]);
	sprintf(strTime, "%s%s", strTime, strTemp);//Y-M-
	if (dTime[2] < 10) sprintf(strTemp, "0%d ", dTime[2]);
	else sprintf(strTemp, "%d ", dTime[2]);
	sprintf(strTime, "%s%s", strTime, strTemp);//Y-M-D
	if (dTime[3] < 10) sprintf(strTemp, "0%d:", dTime[3]);
	else sprintf(strTemp, "%d:", dTime[3]);
	sprintf(strTime, "%s%s", strTime, strTemp);//Y-M-D H:
	if (dTime[4] < 10) sprintf(strTemp, "0%d:", dTime[4]);
	else sprintf(strTemp, "%d:", dTime[4]);
	sprintf(strTime, "%s%s", strTime, strTemp);//Y-M-D h:m:
	if (dTime[5] < 10) sprintf(strTemp, "0%d", dTime[5]);
	else sprintf(strTemp, "%d", dTime[5]);
	sprintf(strTime, "%s%s", strTime, strTemp);//Y-M-D h:m:s
	return strTime;
}

void FunGetSysTimeDat(short * pTime)
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	pTime[0] = (short)systime.wYear;
	pTime[1] = (short)systime.wMonth;
	pTime[2] = (short)systime.wDay;
	pTime[3] = (short)systime.wHour;
	pTime[4] = (short)systime.wMinute;
	pTime[5] = (short)systime.wSecond;
	pTime[6] = (short)systime.wMilliseconds;
}

long FunGetFileSize(CString strFilePath)
{
	FILE *pfCheckSize = fopen(strFilePath, "rb");
	long nFileSize = 0;
	if (pfCheckSize != NULL)
	{
		fseek(pfCheckSize, 0, SEEK_END);
		nFileSize = ftell(pfCheckSize);
	}
	if (pfCheckSize != NULL)
	{
		fclose(pfCheckSize); pfCheckSize = NULL;
	}
	return nFileSize;
}

void FunOutPutLog2File(CString strLogInfo)
{
	CString strLogFilePath = FunGetSyslogPath();
	CString strTime = FunGetSysTimeStr();
	long nFileSize = FunGetFileSize(strLogFilePath);
	if (nFileSize >= MAX_SIZE_LOG_FILE) DeleteFile(strLogFilePath);
	FILE *pfW = fopen(strLogFilePath, "at+");
	if (pfW != NULL)
	{
		if (nFileSize > 0 && strLogInfo == "\n") fprintf(pfW, "\n");
		else if (strLogInfo != "\n")
			fprintf(pfW, "%s ===> %s\n", strTime, strLogInfo);
		fclose(pfW); pfW = NULL;
	}
}

CString FunGetFileFolder(CString strFullPath)
{
	CString strFolder = strFullPath.Left(strFullPath.ReverseFind('\\'));
	return strFolder;
}

CString FunGetFileName(CString strFullPath, bool bAddExt/* = false*/)
{
	CString strName; int nPos1 = 0;
	nPos1 = strFullPath.ReverseFind('\\');
	if (bAddExt)
	{
		strName = strFullPath.Right(strFullPath.GetLength() - nPos1 - 1);
	}
	else
	{
		int nPos2 = strFullPath.ReverseFind('.');
		if (nPos2 == -1) nPos2 = strFullPath.GetLength();
		strName = strFullPath.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
	}
	return strName;
}

CString FunGetFileExt(CString strFullPath)
{
	CString strExt = strFullPath.Right(strFullPath.GetLength() - strFullPath.ReverseFind('.') - 1);
	return strExt;
}

void    FunSearchFile(CString strFolder, CString strExt, vector<CString>& vecFilePath)
{
	if (!PathFileExists(strFolder))
	{
		FunOutPutLog2File("文件检索失败，目录路径错误。");
		return;
	}

	CString path = strFolder;
	CString  strFileName, strFileExt;
	if (path.Right(path.GetLength() - 1) != "\\");//路径要以/结尾 
	path += _T("\\");
	path += _T("*.*");//CFileFind 类查找路径/*.gz时，指查找路径下所有文件 
					  //path += _T("*.tif");//CFileFind 类查找路径/*.tiff时，指查找路径下所有文件 
	CFileFind ff;
	BOOL res = ff.FindFile(path);
	while (res)
	{
		res = ff.FindNextFile();
		//如果不是. ..（.表示当前目录，..表示上一级目录，也是以文件形式存在的)
		//也不是路径/文件夹，即如果是文件，就删除，你的操作就是获取文件名然后和要查找的文件进行比较
		if (!ff.IsDots() && !ff.IsDirectory())
		{
			if (ff.IsReadOnly())
			{
				::SetFileAttributes(ff.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
			}
			//strFilePath是当前搜索到的文件的完整路径，含文件名和后缀
			CString strFilePath;
			strFilePath = ff.GetFilePath();

			//取得当前文件路径及文件名
			for (int j = strFilePath.GetLength(); j > 0; j--)
			{

				if (strFilePath.GetAt(j) == '\\')
				{
					strFileName = strFilePath.Right(strFilePath.GetLength() - j - 1);
					strFileExt = strFilePath.Right(strExt.GetLength());
					if (strFileExt == strExt)
					{
						vecFilePath.push_back(strFilePath);//文件名依次存入到文件名数组中
					}
					break;
				}
			}
		}
		else   if (ff.IsDots())   //如果是. ..，就继续。    
			continue;
		else    if (ff.IsDirectory())///*如果是文件夹，就递归进入此方法继续删除，你的操作就是递归进入此方法继续对比文件名，并缓存起来。    */
		{
			path = ff.GetFilePath();
			FunSearchFile(path, strExt, vecFilePath);
		}
	}
}

void FunCreateDir4Path(CString strFullPath)
{
	if (PathFileExists(strFullPath)) return;

	char tmpPath[MAX_SIZE_FILE_PATH];
	strFullPath = FunGetFileFolder(strFullPath);
	const char* pCur = strFullPath;
	memset(tmpPath, 0, sizeof(tmpPath));

	int pos = 0;
	while (*pCur++ != '\0')
	{
		tmpPath[pos++] = *(pCur - 1);
		if (*pCur == '/' || *pCur == '\\' || *pCur == '\0')
		{

			if (!PathFileExists(tmpPath) && strlen(tmpPath) > 0)
			{
				CreateDirectory(tmpPath, NULL);
			}
		}
	}
}

vector<CString> FunStringTok(CString strFull, CString strDot)
{
	vector<CString> vPart;
	char strInfo[MAX_SIZE_FILE_PATH]; strcpy(strInfo, strFull);
	char* p = strtok(strInfo, strDot);
	while (p != NULL)
	{
		vPart.push_back(p);
		p = strtok(NULL, strDot);
	}
	return vPart;
}

void FunSaveShpFile(CString strSavePath, void * pGeo)
{
	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
	if (!poDriver)
	{
		return;
	}
	poDriver->Delete(strSavePath);
	GDALDataset* poDS = poDriver->Create(strSavePath, 0, 0, 0, GDT_Unknown, NULL);
	if (!poDS)
	{
		return;
	}
	OGRLayer* poLayer = poDS->CreateLayer("polygon");
	if (!poLayer)
	{
		return;
	}
	OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
	if (!poFeature)
	{
		return;
	}
	poFeature->SetGeometry((OGRGeometry*)pGeo);
	poLayer->CreateFeature(poFeature);
	OGRFeature::DestroyFeature(poFeature);
	GDALClose(poDS);
}

void CreateValidPolygon(CMImage4Check img, int nPixelsExpand, OGRPolygon* pPolygon)
{
	if (pPolygon == NULL) pPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
	OGRLinearRing *pRing = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
	OGRPoint pt;
	pt.setX(img.GetGeoInfo().vecValidVertex[0].x - img.GetResolutionX()*nPixelsExpand);
	pt.setY(img.GetGeoInfo().vecValidVertex[0].y - img.GetResolutionY()*nPixelsExpand);
	pRing->addPoint(&pt);
	pt.setX(img.GetGeoInfo().vecValidVertex[1].x - img.GetResolutionX()*nPixelsExpand);
	pt.setY(img.GetGeoInfo().vecValidVertex[1].y + img.GetResolutionY()*nPixelsExpand);
	pRing->addPoint(&pt);
	pt.setX(img.GetGeoInfo().vecValidVertex[2].x + img.GetResolutionX()*nPixelsExpand);
	pt.setY(img.GetGeoInfo().vecValidVertex[2].y + img.GetResolutionY()*nPixelsExpand);
	pRing->addPoint(&pt);
	pt.setX(img.GetGeoInfo().vecValidVertex[3].x + img.GetResolutionX()*nPixelsExpand);
	pt.setY(img.GetGeoInfo().vecValidVertex[3].y - img.GetResolutionY()*nPixelsExpand);
	pRing->addPoint(&pt);
	pRing->closeRings();
	pPolygon->addRing(pRing);
	OGRGeometryFactory::destroyGeometry(pRing);
}

/**********************************************************************************************/
static bool XmlNodeJudge(pugi::xml_node& Node)
{
	if (Node == NULL)
	{
		FunOutPutLog2File("工程文件解析错误！");
		return false;
	}
	return true;
}
CMDomQuaChk::CMDomQuaChk()
{
	m_pTieMatchMatrix = NULL;
	m_pCtlMatchMatrix = NULL;
	m_nThreodCir = 20;
	m_nThreodPix = 30;
	m_nThreodPtNumTie = 3000;
	m_nThreodPtNumControl = 10000;
	ClearPrjInfo();
}

CMDomQuaChk::~CMDomQuaChk()
{
	ClearPrjInfo();
}

bool CMDomQuaChk::SendData2Prj(vector<CMImage4Check> &vecDomImgs, vector<CMImage4Check> &vecRefImgs)
{
	ClearPrjInfo();
	m_vecDomImgs = vecDomImgs;
	m_vecRefImgs = vecRefImgs;
	if (m_pCtlMatchMatrix) delete[] m_pCtlMatchMatrix;
	if (m_pTieMatchMatrix) delete[] m_pTieMatchMatrix;
	m_pCtlMatchMatrix = NULL;
	m_pTieMatchMatrix = NULL;
	if (m_vecDomImgs.size() > 0)
	{
		m_pTieMatchMatrix = new BYTE[m_vecDomImgs.size()*m_vecDomImgs.size()];
		memset(m_pTieMatchMatrix, 0, m_vecDomImgs.size()*m_vecDomImgs.size());
		if (m_vecRefImgs.size() > 0)
		{
			m_pCtlMatchMatrix = new BYTE[m_vecDomImgs.size()*m_vecRefImgs.size()];
			memset(m_pCtlMatchMatrix, 0, m_vecDomImgs.size()*m_vecRefImgs.size());
		}
	}
	return true;
}

bool CMDomQuaChk::SavePrj2File(const char * strPrjPath)
{
	m_strPrjPath = strPrjPath;
	if (!WritePrjXml())
	{
		FunOutPutLog2File("工程保存失败：" + m_strPrjPath);
		return false;
	}
	m_strMatchFolder = FunGetFileFolder(m_strPrjPath) + "\\ImageMatch";
	FunCreateDir4Path(m_strMatchFolder);
	FunCreateDir4Path(m_strMatchFolder + "\\Tsk");
	FunCreateDir4Path(m_strMatchFolder + "\\Model");
	return true;
}

bool CMDomQuaChk::LoadPrj4File(const char * strPrjPath)
{
	CString strTmp = m_strPrjPath;
	m_strPrjPath = strPrjPath;
	if (!ReadPrjXml())
	{
		m_strPrjPath = strTmp;
		FunOutPutLog2File("工程读取失败：" + m_strPrjPath);
		return false;
	}
	return true;
}

void CMDomQuaChk::BulidPyramids(bool bReBuild /* = false */, pCallBackProFun pFun /* = NULL */, void *pPara3OfpFun /*= NULL*/)
{
	if (pFun)
	{
		pFun(1, "Building pryamid", pPara3OfpFun);
	}
	if (m_vecDomImgs.size() + m_vecRefImgs.size() == 0) return;
	vector<CMImage4Check> vecImgs;
	vecImgs.insert(vecImgs.end(), m_vecDomImgs.begin(), m_vecDomImgs.end());
	vecImgs.insert(vecImgs.end(), m_vecRefImgs.begin(), m_vecRefImgs.end());
	int nCount = 0;
// 	CRITICAL_SECTION cs;
// 	InitializeCriticalSection(&cs);
// #pragma omp parallel for
	for (int i = 0; i < vecImgs.size(); i++)
	{	
//		EnterCriticalSection(&cs);
		nCount++;	
		if (pFun)
		{
			pFun(nCount*100.0 / vecImgs.size(), "Building pryamid", pPara3OfpFun);
		}
	//	LeaveCriticalSection(&cs);
		vecImgs[i].OpenImage(vecImgs[i].GetImgPath());
		vecImgs[i].BuildPryamid(bReBuild, RESMODE_NEARST, 64, NULL);
		vecImgs[i].CloseImg();
	}
//	DeleteCriticalSection(&cs);
	vector<CMImage4Check>().swap(vecImgs);
	if (pFun)
	{
		pFun(100, "", pPara3OfpFun);
	}
}

void CMDomQuaChk::CatchTfwInfo(bool bSave2File /* = true */, pCallBackProFun pFun /* = NULL */, void* pPara3OfPFun /*= NULL*/)
{
	FunOutPutLog2File("计算影像有效范围...");
	if (m_vecDomImgs.size() + m_vecRefImgs.size() == 0) return;
	int nCount = 0;
// 	CRITICAL_SECTION cs;
// 	InitializeCriticalSection(&cs);
// 	#pragma omp parallel for
	for (int i = 0; i < m_vecDomImgs.size(); i++)
	{
		m_vecDomImgs[i].ReadTfwInfo(nCount, m_vecDomImgs.size() + m_vecRefImgs.size(), bSave2File, pFun, pPara3OfPFun);
// 			EnterCriticalSection(&cs);
 		nCount++;
// 			LeaveCriticalSection(&cs);
	}
	for (int i = 0; i < m_vecRefImgs.size(); i++)
	{
		m_vecRefImgs[i].ReadTfwInfo(nCount, m_vecDomImgs.size() + m_vecRefImgs.size(), bSave2File, pFun, pPara3OfPFun);
		//	EnterCriticalSection(&cs);
		nCount++;
		//	LeaveCriticalSection(&cs);
	}
	//DeleteCriticalSection(&cs);
	FunOutPutLog2File("影像有效范围计算完毕！");
}
void CMDomQuaChk::BuildColorMap(bool bReBuild, bool bFast, pCallBackProFun pFun, void *pPara3OfpFun)
{
	if (pFun) pFun(1, "Building color map", pPara3OfpFun);
	int nCount = 0;
// 	CRITICAL_SECTION cs;
// 	InitializeCriticalSection(&cs);
// 	#pragma omp parallel for
	for (int i = 0; i < m_vecDomImgs.size(); i++)	
	{
//		EnterCriticalSection(&cs);
		nCount++;
		if (pFun) 
			pFun((nCount /*+ 1*/)*100.0 / (m_vecDomImgs.size() + m_vecRefImgs.size()), "Building color map", pPara3OfpFun);
//		LeaveCriticalSection(&cs);
		m_vecDomImgs[i].OpenImage(m_vecDomImgs[i].GetImgPath());
		m_vecDomImgs[i].GetColorMap16To8(bFast);	
		m_vecDomImgs[i].CloseImg();
	}
	for (int i = 0; i < m_vecRefImgs.size(); i++)
	{
//		EnterCriticalSection(&cs);
		nCount++;
		if (pFun)	
			pFun((nCount/* + 1*/)*100.0 / (m_vecDomImgs.size() + m_vecRefImgs.size()), "Building color map", pPara3OfpFun);
//		LeaveCriticalSection(&cs);
		m_vecRefImgs[i].OpenImage(m_vecRefImgs[i].GetImgPath());
		m_vecRefImgs[i].GetColorMap16To8(bFast);	
		m_vecRefImgs[i].CloseImg();
	}
//	DeleteCriticalSection(&cs);
	if (pFun) pFun(100, "", pPara3OfpFun);
}
void CMDomQuaChk::ImageMatch()
{
	FunOutPutLog2File("执行影像匹配质量检查...");
	if (m_vecDomImgs.size() == 0) return;
	CalMatchMatrix();
	CreateMatchTsk();
	//	FunOutPutLog2File("匹配质检计算完毕！");
}
void CMDomQuaChk::MatchCheck(pCallBackProFun pFun, void *pPara3)
{
	ClearCheckInfo();
	CheckTieMchFile(pFun, pPara3);
	CheckCtlMchFile(pFun, pPara3);
	CheckGcpFile(pFun, pPara3);
}

void CMDomQuaChk::ClearPrjInfo()
{
	ClearMatchInfo();
	ClearCheckInfo();
	vector<CMImage4Check>().swap(m_vecDomImgs);
	vector<CMImage4Check>().swap(m_vecRefImgs);
	if (m_pTieMatchMatrix) delete[]m_pTieMatchMatrix;
	if (m_pCtlMatchMatrix) delete[]m_pCtlMatchMatrix;
	m_pTieMatchMatrix = NULL;
	m_pCtlMatchMatrix = NULL;
	m_strPrjPath.Empty();
	m_strMatchFolder.Empty();
	vector<vector<int>>().swap(m_vvImgRefIdx);
}

int CMDomQuaChk::GetDomNum()
{
	return m_vecDomImgs.size();
}

int CMDomQuaChk::GetRefNum()
{
	return m_vecRefImgs.size();
}

vector<CMImage4Check> CMDomQuaChk::GetAllDomInfo()
{
	return m_vecDomImgs;
}

vector<CMImage4Check> CMDomQuaChk::GetAllRefInfo()
{
	return m_vecRefImgs;
}

CMImage4Check CMDomQuaChk::GetDomInfo(int nIdx)
{
	if (nIdx < 0 || nIdx >= m_vecDomImgs.size()) return CMImage4Check();
	return m_vecDomImgs[nIdx];
}
CMImage4Check CMDomQuaChk::GetRefInfo(int nIdx)
{
	if (nIdx < 0 || nIdx >= m_vecRefImgs.size()) return CMImage4Check();
	return m_vecRefImgs[nIdx];
}
CString CMDomQuaChk::GetMatchFolder()
{
	return m_strMatchFolder;
}
CString CMDomQuaChk::GetGTskFilePath()
{
	return m_strGTskFilePath;
}
CString CMDomQuaChk::GetTieChkMdlPath(int nIdx)
{
	if (nIdx >= m_vecTieChkMdlPath.size()) return "false";
	return m_vecTieChkMdlPath[nIdx];
}
CString CMDomQuaChk::GetGcpChkMdlPath(int nIdx)
{
	if (nIdx >= m_vecGcpChkFilePath.size()) return "false";
	return m_vecGcpChkFilePath[nIdx];
}
int CMDomQuaChk::GetTskNum()
{
	return m_vecTskPath.size();
}

vector<MstuGcpChkModel> CMDomQuaChk::GetGcpModelInfo()
{
	return m_vecGcpChkModelInfo;
}

vector<MstuTieChkModel> CMDomQuaChk::GetTieModelInfo()
{
	return m_vecTieChkModelInfo;
}

vector<vector<Point2D>> CMDomQuaChk::GetGcpModelArea()
{
	return m_vvAreaPtGcpModel;
}

vector<vector<Point2D>> CMDomQuaChk::GetTieModelArea()
{
	return m_vvAreaPtTieModel;
}

vector<int> CMDomQuaChk::GetModelImgIdx(int nModelIdx, MenmModelType eModelType)
{
	vector<int> vecImgIdx;
	switch (eModelType)
	{
	case MODEL_TIE:
		if (nModelIdx >= 0 && nModelIdx <= m_vecTieChkMdlPath.size())
		{
			CString strModelName = FunGetFileName(m_vecTieChkMdlPath[nModelIdx]);
			vector<CString> vParts = FunStringTok(strModelName, "_");
			vecImgIdx.push_back(atoi(vParts[1]));
			vecImgIdx.push_back(atoi(vParts[2]));
			vector<CString>().swap(vParts);
		}
		break;
	case MODEL_CTL:
		if (nModelIdx >= 0 && nModelIdx <= m_vecGcpChkFilePath.size())
		{
			CString strModelName = FunGetFileName(m_vecGcpChkFilePath[nModelIdx]);
			vector<CString> vParts = FunStringTok(strModelName, "_");
			if(vParts.size()==0) break;
			vecImgIdx.push_back(atoi(vParts[0]));
			vector<CString>().swap(vParts);
			if (m_vvImgRefIdx[nModelIdx].size() > 0)
				vecImgIdx.push_back(m_vvImgRefIdx[nModelIdx][0]);
		}
		break;
	default:
		break;
	}
	return vecImgIdx;
}

MstuMchModel CMDomQuaChk::GetCheckModelPoint(int nModelIdx, MenmModelType eType)
{
	MstuMchModel model;
	switch (eType)
	{
	case MODEL_TIE:
		if (nModelIdx >= 0 && nModelIdx < m_vecTieChkMdlPath.size())
		{
			ReadTieCheckFile(nModelIdx, model);
		}
		break;
	case MODEL_CTL:
		if (nModelIdx >= 0 && nModelIdx < m_vecGcpChkFilePath.size())
		{
			ReadGcpCheckFile(nModelIdx, model);
		}
		break;
	default:
		break;
	}
	model.eType = eType;
	return model;
}

bool CMDomQuaChk::WritePrjXml()
{
	if (m_vecDomImgs.size() + m_vecRefImgs.size() == 0) return false;
	vector<CMImage4Check> vecImgs;
	vecImgs.insert(vecImgs.end(), m_vecDomImgs.begin(), m_vecDomImgs.end());
	vecImgs.insert(vecImgs.end(), m_vecRefImgs.begin(), m_vecRefImgs.end());
	pugi::xml_document IXml; char strValue[MAX_SIZE_FILE_LINE];
	pugi::xml_node decl = IXml.prepend_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "gb2312";
	pugi::xml_node RootNode = IXml.append_child("ImageList");
	pugi::xml_node ImgNode, ChildNode, ChildNode2;
	for (int i = 0; i < vecImgs.size(); i++)
	{
		ImgNode = RootNode.append_child("Image");
		ChildNode = ImgNode.append_child("ID");
		sprintf(strValue, "%d", i);
		ChildNode.append_child(pugi::node_pcdata).set_value(strValue);
		ChildNode = ImgNode.append_child("Path");
		sprintf(strValue, "%s", vecImgs[i].GetImgPath());
		ChildNode.append_child(pugi::node_pcdata).set_value(strValue);
		ChildNode = ImgNode.append_child("WorkType");
		if (vecImgs[i].GetImgType() == IMGTYPE_DOM)
			sprintf(strValue, "%s", "DOM"); else
			if (vecImgs[i].GetImgType() == IMGTYPE_REF)
				sprintf(strValue, "%s", "REF");
		ChildNode.append_child(pugi::node_pcdata).set_value(strValue);
		ChildNode = ImgNode.append_child("TFWInfo");
		vector<Point2D> pPt2d = vecImgs[i].GetGeoInfo().vecValidVertex;
		ChildNode2 = ChildNode.append_child("LeftTop");
		sprintf(strValue, "%.6lf %.6lf", pPt2d[0].x, pPt2d[0].y);
		ChildNode2.append_child(pugi::node_pcdata).set_value(strValue);
		ChildNode2 = ChildNode.append_child("LeftBottom");
		sprintf(strValue, "%.6lf %.6lf", pPt2d[1].x, pPt2d[1].y);
		ChildNode2.append_child(pugi::node_pcdata).set_value(strValue);
		ChildNode2 = ChildNode.append_child("RightBottom");
		sprintf(strValue, "%.6lf %.6lf", pPt2d[2].x, pPt2d[2].y);
		ChildNode2.append_child(pugi::node_pcdata).set_value(strValue);
		ChildNode2 = ChildNode.append_child("RightTop");
		sprintf(strValue, "%.6lf %.6lf", pPt2d[3].x, pPt2d[3].y);
		ChildNode2.append_child(pugi::node_pcdata).set_value(strValue);
		vector<Point2D>().swap(pPt2d);
	}
	IXml.save_file(m_strPrjPath);
	vector<CMImage4Check>().swap(vecImgs);
	return true;
}

bool CMDomQuaChk::ReadPrjXml()
{
	pugi::xml_document PrjXmlDoc;
	pugi::xml_parse_result ret = PrjXmlDoc.load_file(m_strPrjPath, pugi::parse_full, pugi::encoding_utf8);
	if (ret.status != pugi::status_ok)
	{
		FunOutPutLog2File("工程xml打开失败！");
		return false;
	}
	vector<CMImage4Check> vecDomImage, vecRefImage;
	pugi::xml_node NodeRoot = PrjXmlDoc.child("ImageList");
	if (!XmlNodeJudge(NodeRoot)) return false;
	for (pugi::xml_node Node1 = NodeRoot.first_child(); Node1 != NULL; Node1 = Node1.next_sibling())
	{
		if (!XmlNodeJudge(NodeRoot)) continue;
		CString NodeLabel = Node1.name();
		CMImage4Check image;
		if (NodeLabel == "Image")
		{
			for (pugi::xml_node NodeImgChid = Node1.first_child(); NodeImgChid != NULL; NodeImgChid = NodeImgChid.next_sibling())
			{
				CString ChidLabel = NodeImgChid.name();
				if (ChidLabel == "Path") image.SetImgPath(NodeImgChid.text().as_string()); else
					if (ChidLabel == "WorkType")
					{
						CString strLabel2 = NodeImgChid.text().as_string();
						if (strLabel2 == "DOM")
							image.SetImgType(IMGTYPE_DOM); else
							image.SetImgType(IMGTYPE_REF);
					}
					else if (ChidLabel == "TFWInfo")
					{
						vector<Point2D> vecTfwPt(4);
						for (pugi::xml_node NodeTFWChid = NodeImgChid.first_child(); NodeTFWChid != NULL; NodeTFWChid = NodeTFWChid.next_sibling())
						{
							CString ChidLabel2 = NodeTFWChid.name();
							if (ChidLabel2 == "LeftTop")
							{
								CString strTfw = NodeTFWChid.text().as_string();
								double px, py;
								sscanf(strTfw, "%lf %lf", &px, &py);
								vecTfwPt[0] = Point2D(px, py);
							}
							else
								if (ChidLabel2 == "LeftBottom")
								{
									CString strTfw = NodeTFWChid.text().as_string();
									double px, py;
									sscanf(strTfw, "%lf %lf", &px, &py);
									vecTfwPt[1] = Point2D(px, py);
								}
								else
									if (ChidLabel2 == "RightBottom")
									{
										CString strTfw = NodeTFWChid.text().as_string();
										double px, py;
										sscanf(strTfw, "%lf %lf", &px, &py);
										vecTfwPt[2] = Point2D(px, py);
									}
									else
										if (ChidLabel2 == "RightTop")
										{
											CString strTfw = NodeTFWChid.text().as_string();
											double px, py;
											sscanf(strTfw, "%lf %lf", &px, &py);
											vecTfwPt[3] = Point2D(px, py);
										}

						}
						image.SetTfwInfo(vecTfwPt);
						vector<Point2D>().swap(vecTfwPt);
					}
			}
		}
		if (image.GetImgType() == IMGTYPE_DOM) vecDomImage.push_back(image);
		else vecRefImage.push_back(image);
	}
	SendData2Prj(vecDomImage, vecRefImage);
	vector<CMImage4Check>().swap(vecDomImage);
	vector<CMImage4Check>().swap(vecRefImage);
	return true;
}

void CMDomQuaChk::CalMatchMatrix()
{
	FunOutPutLog2File("计算影像匹配矩阵...");
	if (m_vecDomImgs.size() == 0) return;
	vector<OGRPolygon*> vecDomPolygon(m_vecDomImgs.size());
	for (int i = 0; i < m_vecDomImgs.size(); i++)
	{
		vecDomPolygon[i] = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
		//		CreateValidPolygon(m_vecDomImgs[i], 10, vecDomPolygon[i]);
		CMImage4Check img = m_vecDomImgs[i]; int nPixelsExpand = 10;
		OGRLinearRing *pRing = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
		OGRPoint pt;
		pt.setX(img.GetGeoInfo().vecValidVertex[0].x - img.GetResolutionX()*nPixelsExpand);
		pt.setY(img.GetGeoInfo().vecValidVertex[0].y - img.GetResolutionY()*nPixelsExpand);
		pRing->addPoint(&pt);
		pt.setX(img.GetGeoInfo().vecValidVertex[1].x - img.GetResolutionX()*nPixelsExpand);
		pt.setY(img.GetGeoInfo().vecValidVertex[1].y + img.GetResolutionY()*nPixelsExpand);
		pRing->addPoint(&pt);
		pt.setX(img.GetGeoInfo().vecValidVertex[2].x + img.GetResolutionX()*nPixelsExpand);
		pt.setY(img.GetGeoInfo().vecValidVertex[2].y + img.GetResolutionY()*nPixelsExpand);
		pRing->addPoint(&pt);
		pt.setX(img.GetGeoInfo().vecValidVertex[3].x + img.GetResolutionX()*nPixelsExpand);
		pt.setY(img.GetGeoInfo().vecValidVertex[3].y - img.GetResolutionY()*nPixelsExpand);
		pRing->addPoint(&pt);
		pRing->closeRings();
		vecDomPolygon[i]->addRing(pRing);
		OGRGeometryFactory::destroyGeometry(pRing);
//		CString str; str.Format("%d.shp", i);
//		FunSaveShpFile(str, vecDomPolygon[i]);
	}
	if (m_pTieMatchMatrix == NULL)
		m_pTieMatchMatrix = new BYTE[m_vecDomImgs.size()*m_vecDomImgs.size()];
	memset(m_pTieMatchMatrix, 0, m_vecDomImgs.size()*m_vecDomImgs.size());

	vector<vector<Point2D>>().swap(m_vvAreaPtTieModel);
	for (int i = 0; i < m_vecDomImgs.size() - 1; i++)
	{
		for (int j = i + 1; j < m_vecDomImgs.size(); j++)
		{
			OGRGeometry *pinterGeo = vecDomPolygon[i]->Intersection(vecDomPolygon[j]);

			if (pinterGeo->getGeometryType() == wkbPolygon)
			{
				vector<Point2D> vecPt;
				m_pTieMatchMatrix[i*m_vecDomImgs.size() + j] = 1;
				m_pTieMatchMatrix[j*m_vecDomImgs.size() + i] = 1;
				OGRLinearRing *pRing = ((OGRPolygon*)pinterGeo)->getExteriorRing();
				for (int i = 0; i < pRing->getNumPoints(); i++)
				{
					OGRPoint pt; pRing->getPoint(i, &pt);
					vecPt.push_back(Point2D(pt.getX(), pt.getY()));
				}
				m_vvAreaPtTieModel.push_back(vecPt);
				vector<Point2D>().swap(vecPt);
			}
		}

	}
	FILE* pfW = fopen(m_strMatchFolder + "\\TieMatchMatrix.txt", "w");
	for (int i = 0; i < m_vecDomImgs.size(); i++)
	{
		for (int j = 0; j < m_vecDomImgs.size(); j++)
		{
			fprintf(pfW, "%d ", m_pTieMatchMatrix[i*m_vecDomImgs.size() + j]);
		}
		fprintf(pfW, "\n");
	}
	fclose(pfW);
	if (m_vecRefImgs.size() > 0)
	{

		vector<OGRPolygon*> vecRefPolygon(m_vecRefImgs.size());
		for (int i = 0; i < m_vecRefImgs.size(); i++)
		{
			vecRefPolygon[i] = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
			//	CreateValidPolygon(m_vecRefImgs[i], 10, vecRefPolygon[i]);
			CMImage4Check img = m_vecRefImgs[i]; int nPixelsExpand = 10;
			OGRLinearRing *pRing = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
			OGRPoint pt;
			pt.setX(img.GetGeoInfo().vecValidVertex[0].x - img.GetResolutionX()*nPixelsExpand);
			pt.setY(img.GetGeoInfo().vecValidVertex[0].y - img.GetResolutionY()*nPixelsExpand);
			pRing->addPoint(&pt);
			pt.setX(img.GetGeoInfo().vecValidVertex[1].x - img.GetResolutionX()*nPixelsExpand);
			pt.setY(img.GetGeoInfo().vecValidVertex[1].y + img.GetResolutionY()*nPixelsExpand);
			pRing->addPoint(&pt);
			pt.setX(img.GetGeoInfo().vecValidVertex[2].x + img.GetResolutionX()*nPixelsExpand);
			pt.setY(img.GetGeoInfo().vecValidVertex[2].y + img.GetResolutionY()*nPixelsExpand);
			pRing->addPoint(&pt);
			pt.setX(img.GetGeoInfo().vecValidVertex[3].x + img.GetResolutionX()*nPixelsExpand);
			pt.setY(img.GetGeoInfo().vecValidVertex[3].y - img.GetResolutionY()*nPixelsExpand);
			pRing->addPoint(&pt);
			pRing->closeRings();
			vecRefPolygon[i]->addRing(pRing);
		}
		if (m_pCtlMatchMatrix == NULL)
			m_pCtlMatchMatrix = new BYTE[m_vecDomImgs.size()*m_vecRefImgs.size()];
		memset(m_pCtlMatchMatrix, 0, m_vecDomImgs.size()*m_vecRefImgs.size());
		vector<vector<int>>().swap(m_vvImgRefIdx);
		m_vvImgRefIdx.resize(m_vecDomImgs.size());
		vector<vector<Point2D>>().swap(m_vvAreaPtGcpModel);
		for (int i = 0; i < m_vecDomImgs.size(); i++)
		{
			OGRGeometry *pUnionGeo = NULL;
			for (int j = 0; j < m_vecRefImgs.size(); j++)
			{
				OGRGeometry *pinterGeo = vecDomPolygon[i]->Intersection(vecRefPolygon[j]);

				if (pinterGeo->getGeometryType() == wkbPolygon)
				{
					m_pCtlMatchMatrix[i*m_vecRefImgs.size() + j] = 1;
					m_vvImgRefIdx[i].push_back(j);
					if (pUnionGeo == NULL) pUnionGeo = pinterGeo;
					else pUnionGeo = pUnionGeo->Union(pinterGeo);
				}
			}

			if (pUnionGeo->getGeometryType() == wkbPolygon)
			{
				vector<Point2D> vecPt;
				OGRLinearRing *pRing = ((OGRPolygon*)pUnionGeo)->getExteriorRing();
				for (int i = 0; i < pRing->getNumPoints(); i++)
				{
					OGRPoint pt; pRing->getPoint(i, &pt);
					vecPt.push_back(Point2D(pt.getX(), pt.getY()));
				}
				m_vvAreaPtGcpModel.push_back(vecPt);
				vector<Point2D>().swap(vecPt);
			}
			else
			{
				m_vvAreaPtGcpModel.push_back(GetDomInfo(i).GetGeoInfo().vecValidVertex);
			}

		}
		pfW = fopen(m_strMatchFolder + "\\CtlMatchMatrix.txt", "w");
		for (int i = 0; i < m_vecDomImgs.size(); i++)
		{
			for (int j = 0; j < m_vecRefImgs.size(); j++)
			{
				fprintf(pfW, "%d ", m_pCtlMatchMatrix[i*m_vecRefImgs.size() + j]);
			}
			fprintf(pfW, "\n");
		}
		fclose(pfW);
		for (int i = 0; i < vecRefPolygon.size(); i++)
		{
			OGRGeometryFactory::destroyGeometry((OGRGeometry*)vecRefPolygon[i]);
		}
		vector<OGRPolygon*>().swap(vecRefPolygon);
	}
	for (int i = 0; i < vecDomPolygon.size(); i++)
	{
		OGRGeometryFactory::destroyGeometry((OGRGeometry*)vecDomPolygon[i]);
	}
	vector<OGRPolygon*>().swap(vecDomPolygon);
	FunOutPutLog2File("影像匹配矩阵计算完毕！");
}

void CMDomQuaChk::CreateMatchTsk(bool bCheckExe /* = true */, MenmMatchExe eMatchExe /* = MATCH_HARRIS */)
{
	ClearMatchInfo();
	for (int i = 0; i < m_vecDomImgs.size() - 1; i++)
	{
		for (int j = i + 1; j < m_vecDomImgs.size(); j++)
		{
			if (m_pTieMatchMatrix[i*m_vecDomImgs.size() + j] == 1)
			{
				CString strIdx; strIdx.Format("_%d_%d_", i, j);
				CString strTskFile = m_strMatchFolder + "\\Tsk\\tie" + strIdx + FunGetFileName(m_vecDomImgs[i].GetImgPath())
					+ "_" + FunGetFileName(m_vecDomImgs[j].GetImgPath()) + ".tsk";
				CString strModelFile = m_strMatchFolder + "\\Model\\" + FunGetFileName(strTskFile) + ".mch";
				FunCreateDir4Path(strTskFile);
				FILE* pfW = fopen(strTskFile, "w"); if (pfW == NULL) continue;
				fprintf(pfW, "%s\n", m_vecDomImgs[i].GetImgPath());
				fprintf(pfW, "%s\n", m_vecDomImgs[j].GetImgPath());
				fprintf(pfW, "%s\n", strModelFile);
				if (eMatchExe == MATCH_SIFT) fprintf(pfW, "Image match by sift\n");
				if (eMatchExe == MATCH_HARRIS) fprintf(pfW, "Image match by harris\n");
				if (bCheckExe) fprintf(pfW, "1\n");
				else fprintf(pfW, "0\n");
				fclose(pfW);
				m_vecTskPath.push_back(strTskFile);
				m_vecTieModelPath.push_back(strModelFile);
			}
		}
	}
	for (int i = 0; i < m_vecDomImgs.size(); i++)
	{
		for (int j = 0; j < m_vecRefImgs.size(); j++)
		{
			if (m_pCtlMatchMatrix[i*m_vecRefImgs.size() + j] == 1)
			{
				CString strIdx; strIdx.Format("_%d_%d_", i, j);
				CString strTskFile = m_strMatchFolder + "\\Tsk\\ctl" + strIdx + FunGetFileName(m_vecDomImgs[i].GetImgPath())
					+ "_" + FunGetFileName(m_vecRefImgs[j].GetImgPath()) + ".tsk";
				CString strModelFile = m_strMatchFolder + "\\Model\\" + FunGetFileName(strTskFile) + ".mch";
				FunCreateDir4Path(strTskFile);
				FILE* pfW = fopen(strTskFile, "w"); if (pfW == NULL) continue;
				fprintf(pfW, "%s\n", m_vecDomImgs[i].GetImgPath());
				fprintf(pfW, "%s\n", m_vecRefImgs[j].GetImgPath());
				fprintf(pfW, "%s\n", strModelFile);
				if (eMatchExe == MATCH_SIFT) fprintf(pfW, "Image match by sift\n");
				if (eMatchExe == MATCH_HARRIS) fprintf(pfW, "Image match by harris\n");
				if (bCheckExe) fprintf(pfW, "1\n");
				else fprintf(pfW, "0\n");
				fclose(pfW);
				m_vecTskPath.push_back(strTskFile);
				m_vecCtlModelPath.push_back(strModelFile);
			}
		}
	}
	if (m_vecTskPath.size() > 0)
	{
		m_strGTskFilePath = FunGetFileFolder(m_vecTskPath[0]) + "\\ImageMatch_gtsk.gtsk";
		CString strMatchExe = FunGetFileFolder(FunGetSysExePath()) + "\\MDomQuaChkImgMch.exe";   //MODIFIED BY TK \\ImageMatch
		FILE*pfW = fopen(m_strGTskFilePath, "w"); if (pfW == NULL) return;
		fprintf(pfW, "%d\n", m_vecTskPath.size());
		for (int i = 0; i < m_vecTskPath.size(); i++)
		{
			fprintf(pfW, "%s %s\n", strMatchExe, m_vecTskPath[i]);
		}
		fclose(pfW);
	}

}

bool CMDomQuaChk::CheckTieMchFile(pCallBackProFun pFun /* = NULL */, void *pPara3 /* = NULL */)
{
	if (pFun) pFun(1, "Check tie match model", pPara3);
	FunOutPutLog2File("开始整理连接点匹配模型...");
	for (int i = 0; i < m_vecTieModelPath.size(); i++)
	{
		CString strMchPath = m_vecTieModelPath[i];
		MstuMchModel stuModel;
		FILE *pfR = fopen(strMchPath, "r");
		if (pfR == NULL)
		{
			if (pFun) pFun((i*2.0 + 2) * 100 / (m_vecTieModelPath.size() * 2), "Check tie match model", pPara3);
			stuModel.Clear();
			continue;
		}
		CString strModelName = FunGetFileName(strMchPath);
		if (strModelName.Left(3) == "tie") stuModel.eType = MODEL_TIE;
		else stuModel.eType = MODEL_CTL;
		if (stuModel.eType == MODEL_CTL)
		{
			if (pFun) pFun((i*2.0 + 2) * 100 / (m_vecTieModelPath.size() * 2), "Check tie match model", pPara3);
			stuModel.Clear();
			continue;
		}
		vector<CString> vPart = FunStringTok(strModelName, "_");
		if (vPart.size() < 2) continue;
		stuModel.nImgIdx1 = atoi(vPart[1]);
		stuModel.nImgIdx2 = atoi(vPart[2]);
		vector<CString>().swap(vPart);
		if(stuModel.nImgIdx1>=m_vecDomImgs.size()||stuModel.nImgIdx2>=m_vecDomImgs.size()) continue;
		MstuTieChkModel TieModel;
		strcpy(TieModel.strDomPath1, m_vecDomImgs[stuModel.nImgIdx1].GetImgPath());
		strcpy(TieModel.strDomPath2, m_vecDomImgs[stuModel.nImgIdx2].GetImgPath());
		TieModel.nIdxLeft = stuModel.nImgIdx1;
		TieModel.nIdxRight = stuModel.nImgIdx2;
		char line[MAX_SIZE_FILE_LINE]; memset(line, 0, MAX_SIZE_FILE_LINE);
		CString strLeftImg, strRightImg;
		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0'; strLeftImg = line;
		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0'; strRightImg = line;
		vPart = FunStringTok(strLeftImg, "= "); if (vPart.size() > 0) strLeftImg = vPart[vPart.size() - 1];
		vPart = FunStringTok(strRightImg, "= "); if (vPart.size() > 0) strRightImg = vPart[vPart.size() - 1];
		vector<CString>().swap(vPart);
		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0';
		int nPtNum = atoi(line);
		if (nPtNum == 0)
		{
			CString strChkMdlPath = FunGetFileFolder(m_strPrjPath) + "\\QuaChk\\Model\\" + FunGetFileName(strMchPath, false) + ".model";
			FunCreateDir4Path(strChkMdlPath);
			FILE*pfW = fopen(strChkMdlPath, "w"); if (pfW == NULL)
			{
				if (pFun) pFun((i*2.0 + 2) * 100 / (m_vecTieModelPath.size() * 2), "Check tie match model", pPara3);
				stuModel.Clear();
				continue;
			}
			fprintf(pfW, "%d\n", 0);
			fclose(pfW);
			m_vecTieChkMdlPath.push_back(strChkMdlPath);
			TieModel.nPtNum = nPtNum;
			m_vecTieChkModelInfo.push_back(TieModel);
			if (pFun) pFun((i*2.0 + 2) * 100 / (m_vecTieModelPath.size() * 2), "Check tie match model", pPara3);
			stuModel.Clear();
			continue;
		}
		stuModel.vecPtIdx.resize(nPtNum);
		stuModel.vecObjPtXY.resize(nPtNum);
		stuModel.vecImgPt1.resize(nPtNum);
		stuModel.vecImgPt2.resize(nPtNum);
		stuModel.vecPtResXY.resize(nPtNum);
		stuModel.vecPtResxy.resize(nPtNum);
		stuModel.vecPtValid.resize(nPtNum);
		int nIdx1, nIdx2; bool bOrder = true;
		nIdx1 = stuModel.nImgIdx1;
		nIdx2 = stuModel.nImgIdx2;
		if (strLeftImg == m_vecDomImgs[nIdx1].GetImgPath()&&strRightImg== m_vecDomImgs[nIdx2].GetImgPath()) 
			bOrder = true;
		else if (strLeftImg == m_vecDomImgs[nIdx2].GetImgPath()&&strRightImg== m_vecDomImgs[nIdx1].GetImgPath()) 
			bOrder = false;
		else continue;
		for (int i = 0; i < nPtNum; i++)
		{
			fgets(line, MAX_SIZE_FILE_LINE, pfR);
			line[strlen(line) - 1] = '\0';
			char strIdx[MAX_SIZE_FILE_LINE]; memset(strIdx, 0, MAX_SIZE_FILE_LINE);
			int nValid = 1; double lx, ly, rx, ry;
			if (bOrder)
				sscanf(line, "%s %lf %lf %lf %lf %s %d", strIdx, &lx, &ly, &rx, &ry, strIdx, &nValid);
			else
				sscanf(line, "%s %lf %lf %lf %lf %s %d", strIdx, &rx, &ry, &lx, &ly, strIdx, &nValid);
			stuModel.vecPtIdx[i] = strIdx;
			stuModel.vecImgPt1[i] = Point2D(lx, ly);
			stuModel.vecImgPt2[i] = Point2D(rx, ry);
		}
		stuModel.nPtValid = nPtNum;
		for (int i = 0; i < nPtNum; i++)
		{
			stuModel.vecPtResXY[i] = Point2D(0, 0);
			stuModel.vecPtResxy[i] = Point2D(0, 0);
			stuModel.vecPtValid[i] = true;
			Point2D ptGeo1, ptGeo2;
			ptGeo1 = m_vecDomImgs[nIdx1].ImgPt2Geo(stuModel.vecImgPt1[i]);
			ptGeo2 = m_vecDomImgs[nIdx2].ImgPt2Geo(stuModel.vecImgPt2[i]);
			stuModel.vecObjPtXY[i] = ptGeo1;
			stuModel.vecPtResXY[i].x = ptGeo1.x - ptGeo2.x;
			stuModel.vecPtResXY[i].y = ptGeo1.y - ptGeo2.y;
			Point2D ptImg2_1 = m_vecDomImgs[nIdx1].GeoPt2Img(ptGeo2);
			stuModel.vecPtResxy[i].x = stuModel.vecImgPt1[i].x - ptImg2_1.x;
			stuModel.vecPtResxy[i].y = stuModel.vecImgPt1[i].y - ptImg2_1.y;
		}
		if (pFun) pFun((i*2.0 + 1) * 100 / (m_vecTieModelPath.size() * 2), "Check tie match model", pPara3);

		DeleteCloudPoints(stuModel);
		DeleteMatchErr(stuModel);
		MchModelThining(stuModel, m_nThreodPtNumTie);
		CString strChkMdlPath = FunGetFileFolder(m_strPrjPath) + "\\QuaChk\\Model\\" + FunGetFileName(strMchPath, false) + ".model";
		FunCreateDir4Path(strChkMdlPath);
		FILE*pfW = fopen(strChkMdlPath, "w");
		if (pfW == NULL)
		{
			if (pFun) pFun((i*2.0 + 2) * 100 / (m_vecTieModelPath.size() * 2), "Check tie match model", pPara3);
			stuModel.Clear();
			continue;
		}
		fprintf(pfW, "%d\n", stuModel.nPtValid);
		int nCount = 0;
		for (int i = 0; i < stuModel.vecPtValid.size(); i++)
		{
			if (!stuModel.vecPtValid[i]) continue;
			int nValid = 1;
			CString strPtLabel;
			strPtLabel.Format("Tie_%04d_%04d_%04d", stuModel.nImgIdx1, stuModel.nImgIdx2, nCount);
			fprintf(pfW, "%s %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %10.6lf %10.6lf %10.6lf %10.6lf %d\n", strPtLabel, stuModel.vecImgPt1[i].x,
				stuModel.vecImgPt1[i].y, stuModel.vecImgPt2[i].x, stuModel.vecImgPt2[i].y, stuModel.vecPtResxy[i].x, stuModel.vecPtResxy[i].y,
				stuModel.vecObjPtXY[i].x, stuModel.vecObjPtXY[i].y, stuModel.vecPtResXY[i].x, stuModel.vecPtResXY[i].y, nValid);
			nCount++;
		}
		fclose(pfW);
		m_vecTieChkMdlPath.push_back(strChkMdlPath);
		MchModel2TieModel(stuModel, TieModel);
		m_vecTieChkModelInfo.push_back(TieModel);
		if (pFun) pFun((i*2.0 + 2) * 100 / (m_vecTieModelPath.size() * 2), "Check tie match model", pPara3);
		stuModel.Clear();
		FunOutPutLog2File(m_vecTieModelPath[i] + "整理完毕！");
	}
	FunOutPutLog2File("连接点匹配模型整理完毕！");
	return true;
}

bool CMDomQuaChk::CheckCtlMchFile(pCallBackProFun pFun /* = NULL */, void *pPara3 /* = NULL */)
{
	/**************内存释放***********/
	FunOutPutLog2File("开始整理控制点匹配模型...");
	if (pFun) pFun(1, "Check ctl match model", pPara3);
	double dRmsX, dRmsY, dSumX = 0, dSumY = 0, dMeanX, dMeanY;
	int nValidPtNum = 0;
	for (int j = 0; j < m_vecCtlModelPath.size(); j++)
	{
		MstuMchModel stuModel;
		CString strMchPath = m_vecCtlModelPath[j];

		CString strModelName = FunGetFileName(strMchPath);

		if (strModelName.Left(3) == "tie") stuModel.eType = MODEL_TIE;
		else stuModel.eType = MODEL_CTL;
		if (stuModel.eType == MODEL_TIE)
		{
			stuModel.Clear();
			if (pFun) pFun((j*2+2)*100.0/m_vecCtlModelPath.size(), "Check ctl match model", pPara3);
			continue;
		}
		vector<CString> vPart = FunStringTok(strModelName, "_");

		stuModel.nImgIdx1 = atoi(vPart[1]);
		stuModel.nImgIdx2 = atoi(vPart[2]);
		vector<CString>().swap(vPart);

		char line[MAX_SIZE_FILE_LINE]; memset(line, 0, MAX_SIZE_FILE_LINE);
		CString strLeftImg, strRightImg;
		FILE *pfR = fopen(strMchPath, "r");	if (pfR == NULL)
		{
			stuModel.Clear();
			if (pFun) pFun((j * 2 + 2)*100.0 / m_vecCtlModelPath.size(), "Check ctl match model", pPara3);
			continue;
		}

		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0'; strLeftImg = line;
		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0'; strRightImg = line;
		vPart = FunStringTok(strLeftImg, "= "); if (vPart.size() > 0) strLeftImg = vPart[vPart.size() - 1];
		vPart = FunStringTok(strRightImg, "= "); if (vPart.size() > 0) strRightImg = vPart[vPart.size() - 1];
		vector<CString>().swap(vPart);
		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0';
		int nPtNum = atoi(line);
		if (nPtNum == 0)
		{
			CString strChkMdlPath = FunGetFileFolder(m_strPrjPath) + "\\QuaChk\\Model\\" + FunGetFileName(strMchPath, false) + ".model";
			FunCreateDir4Path(strChkMdlPath);
			FILE*pfW = fopen(strChkMdlPath, "w");
			if (pfW == NULL)
			{
				stuModel.Clear();
				continue;
			}
			fprintf(pfW, "%d\n", 0);
			fclose(pfW);
			m_vecCtlChkMdlPath.push_back(strChkMdlPath);
			stuModel.Clear();
			if (pFun) pFun((j * 2 + 2)*100.0 / m_vecCtlModelPath.size(), "Check ctl match model", pPara3);
			continue;
		}
		stuModel.vecPtIdx.resize(nPtNum);
		stuModel.vecObjPtXY.resize(nPtNum);
		stuModel.vecImgPt1.resize(nPtNum);
		stuModel.vecImgPt2.resize(nPtNum);
		stuModel.vecPtResXY.resize(nPtNum);
		stuModel.vecPtResxy.resize(nPtNum);
		stuModel.vecPtValid.resize(nPtNum);
		int nIdx1, nIdx2; bool bOrder = true;
		nIdx1 = stuModel.nImgIdx1;
		nIdx2 = stuModel.nImgIdx2;
		if (strLeftImg != m_vecDomImgs[stuModel.nImgIdx1].GetImgPath())
			bOrder = false;
		for (int i = 0; i < nPtNum; i++)
		{
			fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0';
			char strIdx[MAX_SIZE_FILE_LINE]; memset(strIdx, 0, MAX_SIZE_FILE_LINE);
			int nValid; double lx, ly, rx, ry;
			if (bOrder)
				sscanf(line, "%s %lf %lf %lf %lf %s %d", strIdx, &lx, &ly, &rx, &ry, strIdx, &nValid);
			else
				sscanf(line, "%s %lf %lf %lf %lf %s %d", strIdx, &rx, &ry, &lx, &ly, strIdx, &nValid);
			stuModel.vecPtIdx[i] = strIdx;
			stuModel.vecImgPt1[i] = Point2D(lx, ly);
			stuModel.vecImgPt2[i] = Point2D(rx, ry);
		}
		stuModel.nPtValid = nPtNum;
		if (pFun) pFun((j * 2 + 1)*100.0 / m_vecCtlModelPath.size(), "Check ctl match model", pPara3);
		fclose(pfR);

		for (int i = 0; i < nPtNum; i++)
		{
			stuModel.vecPtResXY[i] = Point2D(0, 0);
			stuModel.vecPtResxy[i] = Point2D(0, 0);
			stuModel.vecPtValid[i] = true;
			Point2D ptGeo1, ptGeo2;
			ptGeo1 = m_vecDomImgs[nIdx1].ImgPt2Geo(stuModel.vecImgPt1[i]);
			ptGeo2 = m_vecRefImgs[nIdx2].ImgPt2Geo(stuModel.vecImgPt2[i]);
			stuModel.vecObjPtXY[i] = ptGeo1;
			stuModel.vecPtResXY[i].x = ptGeo1.x - ptGeo2.x;
			stuModel.vecPtResXY[i].y = ptGeo1.y - ptGeo2.y;
			Point2D ptImg2_1 = m_vecDomImgs[nIdx1].GeoPt2Img(ptGeo2);
			stuModel.vecPtResxy[i].x = stuModel.vecImgPt1[i].x - ptImg2_1.x;
			stuModel.vecPtResxy[i].y = stuModel.vecImgPt1[i].y - ptImg2_1.y;
		}

		DeleteCloudPoints(stuModel);
		DeleteMatchErr(stuModel);

		CString strChkMdlPath = FunGetFileFolder(m_strPrjPath) + "\\QuaChk\\Model\\" + FunGetFileName(strMchPath, false) + ".model";
		FunCreateDir4Path(strChkMdlPath);
		FILE*pfW = fopen(strChkMdlPath, "w"); if (pfW == NULL)
		{
			stuModel.Clear();
			if (pFun) pFun((j * 2 + 2)*100.0 / m_vecCtlModelPath.size(), "Check ctl match model", pPara3);
			continue;
		}
		fprintf(pfW, "%d\n", stuModel.nPtValid);
		int nCount = 0;
		for (int i = 0; i < nPtNum; i++)
		{
			int nValidType = (stuModel.vecPtValid[i] ? 1 : 0);
			if (nValidType == 0)continue;
			CString strPtLabel; strPtLabel.Format("Ctl_%04d_%04d_%04d", stuModel.nImgIdx1, stuModel.nImgIdx2, i);
			fprintf(pfW, "%s %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %10.6lf %10.6lf %10.6lf %10.6lf %d\n", strPtLabel, stuModel.vecImgPt1[i].x,
				stuModel.vecImgPt1[i].y, stuModel.vecImgPt2[i].x, stuModel.vecImgPt2[i].y, stuModel.vecPtResxy[i].x, stuModel.vecPtResxy[i].y,
				stuModel.vecObjPtXY[i].x, stuModel.vecObjPtXY[i].y, stuModel.vecPtResXY[i].x, stuModel.vecPtResXY[i].y, nValidType);
		}
		fclose(pfW);
		m_vecCtlChkMdlPath.push_back(strChkMdlPath);
		stuModel.Clear();
		if (pFun) pFun((j * 2 + 2)*100.0 / m_vecCtlModelPath.size(), "Check ctl match model", pPara3);
		FunOutPutLog2File(m_vecCtlModelPath[j] + "整理完毕！");
	}
	FunOutPutLog2File("控制点匹配模型整理完毕！");
	return true;
}

//创建 check model 以及 gcp文件 的保存路径
void CMDomQuaChk::CreateChkModlePath()
{
	FunOutPutLog2File("正在创建check模型的保存路径...");

	//tie check model path
	CString strMchPath, strChkMdlPath;
	for (int i = 0; i < m_vecTieModelPath.size(); i++)
	{
		strMchPath = m_vecTieModelPath[i];
		strChkMdlPath = FunGetFileFolder(m_strPrjPath) + "\\QuaChk\\Model\\" + FunGetFileName(strMchPath, false) + ".model";
		m_vecTieChkMdlPath.push_back(strChkMdlPath);
	}

	//gcp path
	vector<CString>().swap(m_vecGcpChkFilePath);
	m_vecGcpChkFilePath.resize(m_vecDomImgs.size());
	for (int i = 0; i < m_vecDomImgs.size(); i++)
	{
		m_vecGcpChkFilePath[i].Format("%s\\QuaChk\\Gcp\\%d_%s.gcp", FunGetFileFolder(m_strPrjPath), i,
			FunGetFileName(m_vecDomImgs[i].GetImgPath(), false));
	}
}

bool CMDomQuaChk::LoadModelAndGcpFile(pCallBackProFun pFun /* = NULL */, void *pPara3 /* = NULL */)
{
	FunOutPutLog2File("正在载入连接点匹配模型...");
	if (pFun) pFun(1, "Loading tie match model", pPara3);
	for (int ciri = 0; ciri < m_vecTieChkMdlPath.size(); ++ciri)
	{
		CString strMchPath = m_vecTieChkMdlPath[ciri];
		MstuMchModel stuModel;

		CString strModelName = FunGetFileName(strMchPath);
		if (strModelName.Left(3) == "tie") stuModel.eType = MODEL_TIE;
		else stuModel.eType = MODEL_CTL;
		if (stuModel.eType == MODEL_CTL)
		{
			stuModel.Clear();
			continue;
		}
		vector<CString> vPart = FunStringTok(strModelName, "_");
		if (vPart.size() < 2) continue;
		stuModel.nImgIdx1 = atoi(vPart[1]);
		stuModel.nImgIdx2 = atoi(vPart[2]);
		vector<CString>().swap(vPart);

		if (stuModel.nImgIdx1 >= m_vecDomImgs.size() || stuModel.nImgIdx2 >= m_vecDomImgs.size()) continue;
		MstuTieChkModel TieModel;
		strcpy(TieModel.strDomPath1, m_vecDomImgs[stuModel.nImgIdx1].GetImgPath());
		strcpy(TieModel.strDomPath2, m_vecDomImgs[stuModel.nImgIdx2].GetImgPath());
		TieModel.nIdxLeft = stuModel.nImgIdx1;
		TieModel.nIdxRight = stuModel.nImgIdx2;
		
		FILE*pfW = fopen(m_vecTieChkMdlPath[ciri], "r"); 	
		fscanf(pfW, "%d\n", &stuModel.nPtValid);
		stuModel.vecPtIdx.resize(stuModel.nPtValid);
		stuModel.vecObjPtXY.resize(stuModel.nPtValid);
		stuModel.vecImgPt1.resize(stuModel.nPtValid);
		stuModel.vecImgPt2.resize(stuModel.nPtValid);
		stuModel.vecPtResXY.resize(stuModel.nPtValid);
		stuModel.vecPtResxy.resize(stuModel.nPtValid);
		stuModel.vecPtValid.resize(stuModel.nPtValid);

		int nCount = 0;
		int nValid = 1;
		for (int i = 0; i < stuModel.nPtValid; i++)
		{
			CString strPtLabel;
			fscanf(pfW, "%s", strPtLabel);
			fscanf(pfW, "%lf", &stuModel.vecImgPt1[i].x);
			fscanf(pfW, "%lf", &stuModel.vecImgPt1[i].y);
			fscanf(pfW, "%lf", &stuModel.vecImgPt2[i].x);
			fscanf(pfW, "%lf", &stuModel.vecImgPt2[i].y);
			fscanf(pfW, "%lf", &stuModel.vecPtResxy[i].x);
			fscanf(pfW, "%lf", &stuModel.vecPtResxy[i].y);
			fscanf(pfW, "%lf", &stuModel.vecObjPtXY[i].x);
			fscanf(pfW, "%lf", &stuModel.vecObjPtXY[i].y);
			fscanf(pfW, "%lf", &stuModel.vecPtResXY[i].x);
			fscanf(pfW, "%lf", &stuModel.vecPtResXY[i].y);
			fscanf(pfW, "%d", &nValid);

			stuModel.vecPtValid[i] = true;
			nCount++;
		}
		fclose(pfW);
		MchModel2TieModel(stuModel, TieModel);
		m_vecTieChkModelInfo.push_back(TieModel);
		stuModel.Clear();
		if (pFun) pFun((ciri*2.0 + 2) * 100 / (m_vecTieChkMdlPath.size() * 2), "Loading tie match model", pPara3);
	}
	
	FunOutPutLog2File("正在载入控制点匹配模型...");
	if (pFun) pFun(1, "Loading control match model", pPara3);
	vector<MstuGcpChkModel>().swap(m_vecGcpChkModelInfo);
	m_vecGcpChkModelInfo.resize(m_vecGcpChkFilePath.size());
	for (int ciri = 0; ciri < m_vecGcpChkFilePath.size(); ++ciri)
	{
		vector<CString> vPart = FunStringTok(FunGetFileName(m_vecGcpChkFilePath[ciri]), "_");
		if (vPart.size() < 1) continue;
		int nDomIdx = atoi(vPart[0]);
		vector<CString>().swap(vPart);

		MstuMchModel MchModel; MstuGcpChkModel GcpModel;
		GcpModel.nDomIdx = nDomIdx;
		strcpy(GcpModel.strDomPath, m_vecDomImgs[nDomIdx].GetImgPath());

		FILE* pfW = fopen(m_vecGcpChkFilePath[ciri], "r");  if (pfW == NULL) continue;
		fscanf(pfW, "%d\n", &MchModel.nPtValid);
		MchModel.eType = MODEL_CTL;
		MchModel.vecPtIdx.resize(MchModel.nPtValid);
		MchModel.vecObjPtXY.resize(MchModel.nPtValid);
		MchModel.vecImgPt1.resize(MchModel.nPtValid);
		MchModel.vecImgPt2.resize(MchModel.nPtValid);
		MchModel.vecPtResXY.resize(MchModel.nPtValid);
		MchModel.vecPtResxy.resize(MchModel.nPtValid);
		MchModel.vecPtValid.resize(MchModel.nPtValid);

		int nValid = 1;
		char strPtLabel[512];
		for (int j = 0; j < MchModel.nPtValid; j++)
		{
			fscanf(pfW, "%s", strPtLabel);
			fscanf(pfW, "%lf", &MchModel.vecImgPt1[j].x);
			fscanf(pfW, "%lf", &MchModel.vecImgPt1[j].y);
			fscanf(pfW, "%lf", &MchModel.vecImgPt2[j].x);
			fscanf(pfW, "%lf", &MchModel.vecImgPt2[j].y);
			fscanf(pfW, "%lf", &MchModel.vecPtResxy[j].x);
			fscanf(pfW, "%lf", &MchModel.vecPtResxy[j].y);
			fscanf(pfW, "%lf", &MchModel.vecObjPtXY[j].x);
			fscanf(pfW, "%lf", &MchModel.vecObjPtXY[j].y);
			fscanf(pfW, "%lf", &MchModel.vecPtResXY[j].x);
			fscanf(pfW, "%lf", &MchModel.vecPtResXY[j].y);
			fscanf(pfW, "%d", &nValid);

			MchModel.vecPtValid[j] = true;
			MchModel.nImgIdx1 = 0;
			MchModel.nImgIdx2 = 0;
			
			MchModel.vecPtIdx[j] = strPtLabel;
		}
		fclose(pfW);

		MchModel2GcpModel(MchModel, GcpModel);
		m_vecGcpChkModelInfo[ciri] = GcpModel;
		MchModel.Clear();
		if (pFun) pFun((ciri*2.0 + 2) * 100 / (m_vecGcpChkFilePath.size() * 2), "Loading control match model", pPara3);
	}
	if (pFun) pFun(100, "", pPara3);
	return false;
}

bool CMDomQuaChk::fIsTiemodelAndGcpExist()
{
	CreateChkModlePath();
	for (int ciri = 0; ciri < m_vecTieChkMdlPath.size(); ++ciri)
	{
		if (!IsExist(m_vecTieChkMdlPath[ciri])) return false;
	}
	for (int ciri = 0; ciri < m_vecGcpChkFilePath.size(); ++ciri)
	{
		if (!IsExist(m_vecGcpChkFilePath[ciri])) return false;
	}
	return true;
}

void CMDomQuaChk::CheckGcpFile(pCallBackProFun pFun /* = NULL */, void *pPara3 /* = NULL */)
{
	FunOutPutLog2File("开始整理单张影像控制点...");
	if (pFun) pFun(1, "Check gcp model", pPara3);
	vector<vector<CString>> vvImgGcpLine(m_vecDomImgs.size());
	for (int i = 0; i < m_vecCtlChkMdlPath.size(); i++)
	{
		if (pFun) pFun(i*50.0 / m_vecCtlChkMdlPath.size()+1, "Check gcp model", pPara3);
		vector<CString> vPart = FunStringTok(FunGetFileName(m_vecCtlChkMdlPath[i]), "_");
		if (vPart.size() < 2) continue;
		int nDomIdx = atoi(vPart[1]);
		vector<CString>().swap(vPart);
		char line[MAX_SIZE_FILE_LINE]; memset(line, 0, MAX_SIZE_FILE_LINE);
		FILE *pfR = fopen(m_vecCtlChkMdlPath[i], "r");
		fgets(line, MAX_SIZE_FILE_LINE, pfR);
		while (fgets(line, MAX_SIZE_FILE_LINE, pfR))
		{
			vvImgGcpLine[nDomIdx].push_back(line);
		}
		fclose(pfR);
	}
	if (pFun) pFun(50, "Check gcp model", pPara3);
	vector<CString>().swap(m_vecGcpChkFilePath);
	m_vecGcpChkFilePath.resize(m_vecDomImgs.size());
	vector<MstuGcpChkModel>().swap(m_vecGcpChkModelInfo);
	m_vecGcpChkModelInfo.resize(m_vecDomImgs.size());
	FunOutPutLog2File("控制点匹配模型读取完毕！");
	for (int i = 0; i < m_vecDomImgs.size(); i++)
	{
		if (pFun) pFun(50 + i*50.0 / m_vecDomImgs.size(), "Check gcp model", pPara3);

		MstuMchModel MchModel; MstuGcpChkModel GcpModel;
		int nPtNum = vvImgGcpLine[i].size();
		GcpModel.nDomIdx = i;
		strcpy(GcpModel.strDomPath, m_vecDomImgs[i].GetImgPath());
		if (nPtNum == 0)
		{
			m_vecGcpChkModelInfo[i]=GcpModel;
			m_vecGcpChkFilePath[i].Format("%s\\QuaChk\\Gcp\\%d_%s.gcp", FunGetFileFolder(m_strPrjPath), i,
				FunGetFileName(m_vecDomImgs[i].GetImgPath(), false));
			FunCreateDir4Path(m_vecGcpChkFilePath[i]);
			FILE *pfW = fopen(m_vecGcpChkFilePath[i], "w"); 
			if (pfW == NULL)
				continue;
			fprintf(pfW, "%d\n", 0);
			fclose(pfW);
			continue;
		}
		MchModel.eType = MODEL_CTL;
		MchModel.vecPtIdx.resize(nPtNum);
		MchModel.vecObjPtXY.resize(nPtNum);
		MchModel.vecImgPt1.resize(nPtNum);
		MchModel.vecImgPt2.resize(nPtNum);
		MchModel.vecPtResXY.resize(nPtNum);
		MchModel.vecPtResxy.resize(nPtNum);
		MchModel.vecPtValid.resize(nPtNum);
		for (int j = 0; j < nPtNum; j++)
		{
			char strIdx[MAX_SIZE_FILE_LINE]; memset(strIdx, 0, MAX_SIZE_FILE_LINE);
			int nValid; double lx, ly, rx, ry, dx, dy, X, Y, dX, dY;
			sscanf(vvImgGcpLine[i][j], "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d", strIdx, &lx, &ly, &rx, &ry, &dx, &dy, &X, &Y, &dX, &dY, &nValid);
			MchModel.vecPtIdx[j] = strIdx;
			MchModel.vecImgPt1[j] = Point2D(lx, ly);
			MchModel.vecImgPt2[j] = Point2D(rx, ry);
			MchModel.vecPtResxy[j] = Point2D(dx, dy);
			MchModel.vecObjPtXY[j] = Point2D(X, Y);
			MchModel.vecPtResXY[j] = Point2D(dX, dY);
			MchModel.vecPtValid[j] = (nValid == 0 ? false : true);
			vector<CString> vParts = FunStringTok(MchModel.vecPtIdx[j].c_str(), "_");
			int nIdx1 = atoi(vParts[1]);
			int nIdx2 = atoi(vParts[2]);
			MchModel.nImgIdx1 = nIdx1;
			MchModel.nImgIdx2 = nIdx2;
 		}
		MchModel.nPtValid = nPtNum;
		MchModelThining(MchModel, m_nThreodPtNumControl);
		MchModel2GcpModel(MchModel, GcpModel);
	
		m_vecGcpChkFilePath[i].Format("%s\\QuaChk\\Gcp\\%d_%s.gcp", FunGetFileFolder(m_strPrjPath), i,
			FunGetFileName(m_vecDomImgs[i].GetImgPath(), false));
		m_vecGcpChkModelInfo[i] = GcpModel;

/********************************************存储GCP文件***********************************************/
		FunCreateDir4Path(m_vecGcpChkFilePath[i]);
		FILE *pfW = fopen(m_vecGcpChkFilePath[i], "w"); if (pfW == NULL) continue;
		fprintf(pfW, "%d\n", MchModel.nPtValid);
		for (int j = 0; j < MchModel.nPtValid; j++)
		{
			fprintf(pfW, "%s %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.6lf %.6lf %.6lf %.6lf %d\n",
				MchModel.vecPtIdx[j].c_str(), MchModel.vecImgPt1[j].x, MchModel.vecImgPt1[j].y, MchModel.vecImgPt2[j].x,
				MchModel.vecImgPt2[j].y, MchModel.vecPtResxy[j].x, MchModel.vecPtResxy[j].y, MchModel.vecObjPtXY[j].x,
				MchModel.vecObjPtXY[j].y, MchModel.vecPtResXY[j].x, MchModel.vecPtResXY[j].y, 1);
		}
		fclose(pfW);
		MchModel.Clear();
		CString strInfo; strInfo.Format("%s 整理完毕！", m_vecDomImgs[i].GetImgPath());
		FunOutPutLog2File(strInfo);
/*********************************************************************************/
	}
	for (int i = 0; i < vvImgGcpLine.size(); i++)
	{
		vector<CString>().swap(vvImgGcpLine[i]);
	}
	vector<vector<CString>>().swap(vvImgGcpLine);

	if (pFun) pFun(100, "Check gcp model", pPara3);
	FunOutPutLog2File("单张影像控制点整理完毕！");
}

void CMDomQuaChk::DeleteMatchErr(MstuMchModel &model)
{
	int nOriPtValid = model.vecPtValid.size();

	// delete position error points  //add by tk
	for (int ciri = 0; ciri < model.vecPtValid.size(); ciri++)
	{
		if (!model.vecPtValid[ciri]) continue;
		if (abs(model.vecPtResxy[ciri].x)>m_nThreodPix || abs(model.vecPtResxy[ciri].y) > m_nThreodPix)
		{
			model.nPtValid--;
			model.vecPtValid[ciri] = false;
		}
	}


	int nTimesCount = 0; double dRmsX = 100, dRmsY = 100;
	while (nTimesCount < m_nThreodCir && (dRmsX > 5 || dRmsY > 5))
	{
		vector<double> vecDx(model.nPtValid), vecDy(model.nPtValid);

		int nTempCount = 0;
		for (int i = 0; i < nOriPtValid; i++)
		{
			if (!model.vecPtValid[i]) continue;
			vecDx[nTempCount] = model.vecImgPt1[i].x - model.vecImgPt2[i].x;
			vecDy[nTempCount] = model.vecImgPt1[i].y - model.vecImgPt2[i].y;
			nTempCount++;
		}
		if (vecDy.size() == 0)
		{  
			nTimesCount++;  
			continue;
		}
		double dSumX = 0, dSumY = 0, dMeanX = 0, dMeanY = 0;
		for (int i = 0; i < vecDy.size(); i++)
		{
			dSumX += vecDx[i];
			dSumY += vecDy[i];
		}
		dMeanX = dSumX / vecDx.size();
		dMeanY = dSumY / vecDy.size();
		dSumX = 0; dSumY = 0;
		for (int i = 0; i < vecDy.size(); i++)
		{
			dSumX += (vecDx[i] - dMeanX)*(vecDx[i] - dMeanX);
			dSumY += (vecDy[i] - dMeanY)*(vecDy[i] - dMeanY);
		}
		dRmsX = sqrt(dSumX / vecDy.size());
		dRmsY = sqrt(dSumY / vecDy.size());

		double dTempDevdx, dTempDevdy;
		dTempDevdx = dTempDevdy = 0.0;
		for (int ciri = 0; ciri < nOriPtValid; ciri++)
		{
			if (!model.vecPtValid[ciri]) continue;
			dTempDevdx = model.vecImgPt1[ciri].x - model.vecImgPt2[ciri].x;
			dTempDevdy = model.vecImgPt1[ciri].y - model.vecImgPt2[ciri].y;
			if (fabs(dTempDevdx - dMeanX) > 2 * dRmsX)
			{
				model.nPtValid--;
				model.vecPtValid[ciri] = false;
			}
			else if (fabs(dTempDevdy - dMeanY) > 2 * dRmsY)
			{
				model.nPtValid--;
				model.vecPtValid[ciri] = false;
			}
		}
		/*for (int i = 0; i < vecDy.size(); i++)
		{
		if (fabs(vecDx[i] - dMeanX) > 2 * dRmsX)
		{
		model.nPtValid--;
		model.vecPtValid[i] = false;
		}
		else if (fabs(vecDy[i] - dMeanY) > 2 * dRmsY)
		{
		model.nPtValid--;
		model.vecPtValid[i] = false;
		}
		}*/
		nTimesCount ++ ; 

		vector<double>().swap(vecDx);
		vector<double>().swap(vecDy);
	}


	nTimesCount = 0;  dRmsX = dRmsY = 100;
	while (nTimesCount < m_nThreodCir && (dRmsX > 5 || dRmsY > 5))
	{
		vector<double> vecDx(model.nPtValid), vecDy(model.nPtValid);

		int nTempCount = 0;
		for (int i = 0; i < nOriPtValid; i++)
		{
			if (!model.vecPtValid[i]) continue;
			vecDx[nTempCount] = model.vecPtResxy[i].x;
			vecDy[nTempCount] = model.vecPtResxy[i].y;
			nTempCount++;
		}
		if (vecDy.size() == 0)
		{
			nTimesCount++;
			continue;
		}
		double dSumX = 0, dSumY = 0, dMeanX = 0, dMeanY = 0;
		for (int i = 0; i < vecDy.size(); i++)
		{
			dSumX += vecDx[i];
			dSumY += vecDy[i];
		}
		dMeanX = dSumX / vecDx.size();
		dMeanY = dSumY / vecDy.size();
		dSumX = 0; dSumY = 0;
		for (int i = 0; i < vecDy.size(); i++)
		{
			dSumX += (vecDx[i] - dMeanX)*(vecDx[i] - dMeanX);
			dSumY += (vecDy[i] - dMeanY)*(vecDy[i] - dMeanY);
		}
		dRmsX = sqrt(dSumX / vecDy.size());
		dRmsY = sqrt(dSumY / vecDy.size());
 
		double dTempDevdx, dTempDevdy;
		dTempDevdx = dTempDevdy = 0.0;
		for (int ciri = 0; ciri < nOriPtValid; ciri++)
		{
			if (!model.vecPtValid[ciri]) continue;
			dTempDevdx = model.vecPtResxy[ciri].x;;
			dTempDevdy = model.vecPtResxy[ciri].y;;
			if (fabs(dTempDevdx - dMeanX) > 2 * dRmsX)
			{
				model.nPtValid--;
				model.vecPtValid[ciri] = false;
			}
			else if (fabs(dTempDevdy - dMeanY) > 2 * dRmsY)
			{
				model.nPtValid--;
				model.vecPtValid[ciri] = false;
			}
		}
		/*for (int i = 0; i < vecDy.size(); i++)
		{
			if (fabs(vecDx[i] - dMeanX) > 2 * dRmsX)
			{
				model.nPtValid--;
				model.vecPtValid[i] = false;
			}
			else if (fabs(vecDy[i] - dMeanY) > 2 * dRmsY)
			{
				model.nPtValid--;
				model.vecPtValid[i] = false;
			}
		}*/
		nTimesCount++; 

		vector<double>().swap(vecDx);
		vector<double>().swap(vecDy);
	}  
}

void CMDomQuaChk::DeleteCloudPoints(MstuMchModel &model)
{
	int nValidNum = 0;
	bool IsCloudPoint = false;
	int nImgLx, nImgLy, nImgRx, nImgRy;

	CMImage4Check ImgCheckL, ImgCheckR;
	ImgCheckL = m_vecDomImgs[model.nImgIdx1];
	if (model.eType == MODEL_CTL) ImgCheckR = m_vecRefImgs[model.nImgIdx2];
	else if (model.eType == MODEL_TIE) ImgCheckR = m_vecDomImgs[model.nImgIdx2];
	else return;

	char *pCloudPathL = new char[1024]; strcpy(pCloudPathL, ImgCheckL.GetImgPath());   //search cloud image path
	char *pt = strrchr(pCloudPathL, '.'); *pt = 0; strcat(pCloudPathL, "_AOIB.tif");
	char *pCloudPathR = new char[1024]; strcpy(pCloudPathR, ImgCheckR.GetImgPath());
	pt = strrchr(pCloudPathR, '.'); *pt = 0; strcat(pCloudPathR, "_AOIB.tif");

	bool IsImgLExist = IsExist(pCloudPathL);
	bool IsImgRExist = IsExist(pCloudPathR);
	if (IsImgLExist || IsImgRExist)
	{
		bool bImgLOpened, bImgROpened;
		bImgLOpened = bImgROpened = false;
		if (IsImgLExist) bImgLOpened = ImgCheckL.OpenImage(pCloudPathL);
		if (IsImgRExist) bImgROpened = ImgCheckR.OpenImage(pCloudPathR);
		if (!bImgLOpened&&!bImgROpened) return;

		int nImgLBands, nImgRBands;
		nImgLBands = nImgRBands = 1;
		if (bImgLOpened) nImgLBands = ImgCheckL.GetBands();  //get image data type
		if (bImgROpened) nImgRBands = ImgCheckR.GetBands();
		
		BYTE *pImgLPixel = new BYTE[nImgLBands]; 
		BYTE *pImgRPixel = new BYTE[nImgRBands]; 

		for (int ciri = 0; ciri < model.vecPtValid.size(); ++ciri)
		{
			if (!model.vecPtValid[ciri]) continue;
			IsCloudPoint = false;

			nImgLx = (int)(model.vecImgPt1[ciri].x + 0.5);
			nImgLy = (int)(model.vecImgPt1[ciri].y + 0.5);
			nImgRx = (int)(model.vecImgPt2[ciri].x + 0.5);
			nImgRy = (int)(model.vecImgPt2[ciri].y + 0.5);

			memset(pImgLPixel, 0, nImgLBands);   //initial array
			memset(pImgRPixel, 0, nImgRBands);
			if (bImgLOpened)
			{
				if (ImgCheckL.ReadImg(nImgLx, nImgLy, 1, 1, 1, 1, pImgLPixel))
				{
					if (pImgLPixel[0]>10) IsCloudPoint = true;
				}
			}
			if (!IsCloudPoint && bImgROpened)
			{
				if (ImgCheckR.ReadImg(nImgRx, nImgRy, 1, 1, 1, 1, pImgRPixel))
				{
					if (pImgRPixel[0] > 10) IsCloudPoint = true;
				}
			}

			if (IsCloudPoint)
			{
				model.nPtValid--;
				model.vecPtValid[ciri] = false;
			}
		}
		delete[] pImgLPixel, pImgRPixel;
		ImgCheckL.CloseImg();
		ImgCheckR.CloseImg();
	}
	delete[] pCloudPathL, pCloudPathR;
}

void CMDomQuaChk::MchModel2TieModel(MstuMchModel stuModel, MstuTieChkModel & TieModel)
{
	double dRmsX = 100, dRmsY = 100;
	double dSumX = 0, dSumY = 0, dMeanX, dMeanY;
	int nValid = 0; 
	for (int i = 0; i < stuModel.vecPtValid.size(); i++)   //stuModel.nPtValid  modified by tk
	{
		if (!stuModel.vecPtValid[i]) continue;
		dSumX += fabs(stuModel.vecPtResxy[i].x);
		dSumY += fabs(stuModel.vecPtResxy[i].y);
		nValid++;
	}
	if (nValid == 0)return;
	dMeanX = dSumX / nValid;
	dMeanY = dSumY / nValid;
	for (int i = 0; i < stuModel.nPtValid; i++)
	{
		if (!stuModel.vecPtValid[i]) continue;
		dSumX += (fabs(stuModel.vecPtResxy[i].x) - dMeanX)*(fabs(stuModel.vecPtResxy[i].x) - dMeanX);
		dSumY += (fabs(stuModel.vecPtResxy[i].y) - dMeanY)*(fabs(stuModel.vecPtResxy[i].y) - dMeanY);
	}
	dRmsX = sqrt(dSumX / nValid);
	dRmsY = sqrt(dSumY / nValid);

	TieModel.nPtNum = stuModel.nPtValid;
	TieModel.dMeanDx = dMeanX;
	TieModel.dMeanDy = dMeanY;
	TieModel.dMeanDxy = sqrt(dMeanX*dMeanX + dMeanY*dMeanY);
	TieModel.dMseDx = dRmsX/* + dMeanX*/;
	TieModel.dMseDy = dRmsY/* + dMeanY*/;
	TieModel.dMseDxy = sqrt((dRmsX/* + dMeanX*/)*(dRmsX/* + dMeanX*/)+(dRmsY/* + dMeanY*/)*(dRmsY/* + dMeanY*/));
}

void CMDomQuaChk::MchModel2GcpModel(MstuMchModel stuModel, MstuGcpChkModel & GcpModel)
{
	double dRmsX = 100, dRmsY = 100;
	double dSumX = 0, dSumY = 0, dMeanX, dMeanY;
	int nValid = 0;
	for (int i = 0; i < stuModel.nPtValid; i++)
	{
		if (!stuModel.vecPtValid[i]) continue;
		dSumX += fabs(stuModel.vecPtResxy[i].x);
		dSumY += fabs(stuModel.vecPtResxy[i].y);
		nValid++;
	}
	if (nValid == 0)return;
	dMeanX = dSumX / nValid;
	dMeanY = dSumY / nValid;
	for (int i = 0; i < stuModel.nPtValid; i++)
	{
		if (!stuModel.vecPtValid[i]) continue;
		dSumX += (fabs(stuModel.vecPtResxy[i].x) - dMeanX)*(fabs(stuModel.vecPtResxy[i].x) - dMeanX);
		dSumY += (fabs(stuModel.vecPtResxy[i].y) - dMeanY)*(fabs(stuModel.vecPtResxy[i].y) - dMeanY);
	}
	dRmsX = sqrt(dSumX / nValid);
	dRmsY = sqrt(dSumY / nValid);

	GcpModel.nPtNum = stuModel.nPtValid;
	GcpModel.dMeanDx = dMeanX;
	GcpModel.dMeanDy = dMeanY;
	GcpModel.dMeanDxy = sqrt(dMeanX*dMeanX + dMeanY*dMeanY);
	GcpModel.dMseDx = dRmsX/* + dMeanX*/;
	GcpModel.dMseDy = dRmsY/* + dMeanY*/;
	GcpModel.dMseDxy = sqrt((dRmsX/* + dMeanX*/)*(dRmsX/* + dMeanX*/)+(dRmsY/* + dMeanY*/)*(dRmsY/* + dMeanY*/));
}

void CMDomQuaChk::MchModelThining(MstuMchModel & MchModel, int nPtNumStay)
{
	if (nPtNumStay >= MchModel.vecPtValid.size()) return;
	srand((unsigned)time(NULL));
	MstuMchModel stuModel = MchModel;
	MchModel.Clear();
	MchModel.eType = stuModel.eType;
	MchModel.nImgIdx1 = stuModel.nImgIdx1;
	MchModel.nImgIdx2 = stuModel.nImgIdx2;
	MchModel.eType = stuModel.eType;
	
	MchModel.vecImgPt1.resize(nPtNumStay);
	MchModel.vecImgPt2.resize(nPtNumStay);
	MchModel.vecPtIdx.resize(nPtNumStay);
	MchModel.vecPtResxy.resize(nPtNumStay);
	MchModel.vecPtResXY.resize(nPtNumStay);
	MchModel.vecObjPtXY.resize(nPtNumStay);
	MchModel.vecPtValid.resize(nPtNumStay);

	double nGap = 1.0*stuModel.vecPtValid.size() / nPtNumStay;
	int nTempVild = 0;
	for (int k = 0; k < nPtNumStay; k++)
	{
		int idx = min(k*nGap, stuModel.vecPtValid.size());
		MchModel.vecPtIdx[k] = stuModel.vecPtIdx[idx];
		MchModel.vecImgPt1[k] = stuModel.vecImgPt1[idx];
		MchModel.vecImgPt2[k] = stuModel.vecImgPt2[idx];
		MchModel.vecPtResxy[k] = stuModel.vecPtResxy[idx];
		MchModel.vecPtResXY[k] = stuModel.vecPtResXY[idx];
		MchModel.vecObjPtXY[k] = stuModel.vecObjPtXY[idx];
		MchModel.vecPtValid[k] = stuModel.vecPtValid[idx];
		if (MchModel.vecPtValid[k]) nTempVild++;
	}
	MchModel.nPtValid = nTempVild;
	stuModel.Clear();
}

void CMDomQuaChk::ClearMatchInfo()
{
	vector<CString>().swap(m_vecTskPath);
	vector<CString>().swap(m_vecTieModelPath);
	vector<CString>().swap(m_vecCtlModelPath);
	vector<MstuTieChkModel>().swap(m_vecTieChkModelInfo);
	vector<MstuGcpChkModel>().swap(m_vecGcpChkModelInfo);
	m_strGTskFilePath.Empty();
	//	vector<vector<int>>().swap(m_vvImgRefIdx);
}

void CMDomQuaChk::ClearCheckInfo()
{
	vector<CString>().swap(m_vecTieChkMdlPath);
	vector<CString>().swap(m_vecCtlChkMdlPath);
	vector<CString>().swap(m_vecGcpChkFilePath);
}

bool CMDomQuaChk::ReadTieCheckFile(int nIdx, MstuMchModel & model)
{
	FILE *pfR = fopen(m_vecTieChkMdlPath[nIdx], "r");
	if (pfR == NULL) return false;
	CString strModelName = FunGetFileName(m_vecTieChkMdlPath[nIdx]);
	vector<CString> vPart = FunStringTok(strModelName, "_");
	model.nImgIdx1 = atoi(vPart[1]);
	model.nImgIdx2 = atoi(vPart[2]);
	char line[MAX_SIZE_FILE_LINE]; memset(line, 0, MAX_SIZE_FILE_LINE);
	fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0';
	int nPtNum = atoi(line);
	if (nPtNum == 0)return true;
	model.eType = MODEL_TIE;
	model.nPtValid = nPtNum;
	model.vecPtIdx.resize(nPtNum);
	model.vecObjPtXY.resize(nPtNum);
	model.vecImgPt1.resize(nPtNum);
	model.vecImgPt2.resize(nPtNum);
	model.vecPtResxy.resize(nPtNum);
	model.vecPtResXY.resize(nPtNum);
	model.vecPtValid.resize(nPtNum);
	for (int i = 0; i < nPtNum; i++)
	{
		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0';
		char strIdx[MAX_SIZE_FILE_LINE]; memset(strIdx, 0, MAX_SIZE_FILE_LINE);
		int nValid; 	double lx, ly, rx, ry, dx, dy, X, Y, dX, dY;
		sscanf(line, "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d", strIdx, &lx, &ly, &rx, &ry, &dx, &dy, &X, &Y, &dX, &dY, &nValid);
		//	model.vecPtIdx[i] = new char[strlen(strIdx) + 1];
		//	memset(model.vecPtIdx[i], 0, strlen(strIdx) + 1);
		//	strcpy((char*)model.vecPtIdx[i], strIdx);
		model.vecPtIdx[i] = strIdx;
		model.vecObjPtXY[i] = Point2D(X, Y);
		model.vecImgPt1[i] = Point2D(lx, ly);
		model.vecImgPt2[i] = Point2D(rx, ry);
		model.vecPtResxy[i] = Point2D(dx, dy);
		model.vecPtResXY[i] = Point2D(dX, dY);
		model.vecPtValid[i] = nValid;
		//		strIdx.ReleaseBuffer();
	}
	fclose(pfR);
	return true;
}

bool CMDomQuaChk::ReadGcpCheckFile(int nIdx, MstuMchModel & model)
{
	FILE *pfR = fopen(m_vecGcpChkFilePath[nIdx], "r");
	if (pfR == NULL) return false;
	char line[MAX_SIZE_FILE_LINE]; memset(line, 0, MAX_SIZE_FILE_LINE);
	CString strModelName = FunGetFileName(m_vecGcpChkFilePath[nIdx]);
	vector<CString> vPart = FunStringTok(strModelName, "_");
	model.nImgIdx1 = atoi(vPart[0]);
	model.nImgIdx2 = -1;
	fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0';
	int nPtNum = atoi(line);
	if (nPtNum == 0)return true;
	model.eType = MODEL_CTL;

	model.nPtValid = nPtNum;
	model.vecObjPtXY.resize(nPtNum);
	model.vecPtIdx.resize(nPtNum);
	model.vecImgPt1.resize(nPtNum);
	model.vecImgPt2.resize(nPtNum);
	model.vecPtResxy.resize(nPtNum);
	model.vecPtResXY.resize(nPtNum);
	model.vecPtValid.resize(nPtNum);
	for (int i = 0; i < nPtNum; i++)
	{
		fgets(line, MAX_SIZE_FILE_LINE, pfR); line[strlen(line) - 1] = '\0';
		char strIdx[MAX_SIZE_FILE_LINE]; memset(strIdx, 0, MAX_SIZE_FILE_LINE);
		int nValid; 	double lx, ly, rx, ry, dx, dy, X, Y, dX, dY;
		sscanf(line, "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d", strIdx, &lx, &ly, &rx, &ry, &dx, &dy, &X, &Y, &dX, &dY, &nValid);
		// 		model.vecPtIdx[i] = new char[strlen(strIdx) + 1];
		// 		memset(model.vecPtIdx[i], 0, strlen(strIdx) + 1);
		//		strcpy((char*)model.vecPtIdx[i], strIdx);
		model.vecPtIdx[i] = strIdx;
		model.vecObjPtXY[i] = Point2D(X, Y);
		model.vecImgPt1[i] = Point2D(lx, ly);
		model.vecImgPt2[i] = Point2D(rx, ry);
		model.vecPtResxy[i] = Point2D(dx, dy);
		model.vecPtResXY[i] = Point2D(dX, dY);
		model.vecPtValid[i] = nValid;
		//		strIdx.ReleaseBuffer();
	}
	fclose(pfR);
	return true;
}
