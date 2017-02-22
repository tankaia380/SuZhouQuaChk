// MImage.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "MImage/MImage.h"
#include "gdal201/gdal.h"
#include "gdal201/gdal_priv.h"
#include "gdal201/ogrsf_frmts.h"
#include "gdal201/ogr_spatialref.h"

#include "xml/pugixml.hpp"
#include <io.h>
#include <algorithm>
#include <windows.h>
#include <omp.h>
#include <vector>
#include <conio.h>
#include "CoorConv.hpp"
using namespace std;

bool IsPathExist(const char *pStrPath)
{
	if (_access(pStrPath, 0) == 0) return true;
	else return false;
}

bool WriteImgColorMapXml(const char *strXmlPath, int* pColorMap, bool bFast)
{
	pugi::xml_document ColorMapXmlDoc;
	pugi::xml_parse_result ret = ColorMapXmlDoc.load_file(strXmlPath);
	if (ret.status != pugi::status_ok)
	{
		pugi::xml_node decl = ColorMapXmlDoc.prepend_child(pugi::node_declaration);
		decl.append_attribute("version") = "1.0";
		decl.append_attribute("encoding") = "gb2312";
		char strValue[MAX_SIZE_FILE_LINE];
		sprintf(strValue, "ColorMapTo8");
		pugi::xml_node RootNode = ColorMapXmlDoc.append_child(strValue);
		pugi::xml_node Node;
		Node = RootNode.append_child("Approximate");
		sprintf(strValue, "%d", bFast);
		Node.append_child(pugi::node_pcdata).set_value(strValue);
		int *pGray8UpLimitTo16 = new int[MAX_SIZE_BUF8];
		memset(pGray8UpLimitTo16, 0, sizeof(int)*MAX_SIZE_BUF8);
		int nStart = 0;
		for (int i = 0; i < MAX_SIZE_BUF8; i++)
		{
			for (int j = nStart; j < MAX_SIZE_BUF16; j++)
			{
				if (i == *(pColorMap + j))
				{
					(*(pGray8UpLimitTo16 + i))++;
					nStart++;
				}
			}
			sprintf(strValue, "Gray8_%d_CutValue", i);
			Node = RootNode.append_child(strValue);
			sprintf(strValue, "%d", nStart - 1);
			Node.append_child(pugi::node_pcdata).set_value(strValue);
		}
	}
	else
	{
		char strValue[MAX_SIZE_FILE_LINE];
		for (pugi::xml_node RootNode = ColorMapXmlDoc.first_child(); RootNode != NULL; RootNode = RootNode.next_sibling())
		{
			pugi::xml_node Node = RootNode.first_child();
			if (strcmp(Node.name(), "Approximate") == 0)
			{
				if (Node.text().as_bool() == bFast)
				{
					ColorMapXmlDoc.remove_child(RootNode);
					break;
				}

			}
		}
		sprintf(strValue, "ColorMapTo8");
		pugi::xml_node RootNode = ColorMapXmlDoc.append_child(strValue);
		pugi::xml_node Node;
		Node = RootNode.append_child("Approximate");
		sprintf(strValue, "%d", bFast);
		Node.append_child(pugi::node_pcdata).set_value(strValue);
		int *pGray8UpLimitTo16 = new int[MAX_SIZE_BUF8];
		memset(pGray8UpLimitTo16, 0, sizeof(int)*MAX_SIZE_BUF8);
		int nStart = 0;
		for (int i = 0; i < MAX_SIZE_BUF8; i++)
		{
			for (int j = nStart; j < MAX_SIZE_BUF16; j++)
			{
				if (i == *(pColorMap + j))
				{
					(*(pGray8UpLimitTo16 + i))++;
					nStart++;
				}
			}
			sprintf(strValue, "Gray8_%d_CutValue", i);
			Node = RootNode.append_child(strValue);
			sprintf(strValue, "%d", nStart - 1);
			Node.append_child(pugi::node_pcdata).set_value(strValue);
		}
	}
	ColorMapXmlDoc.save_file(strXmlPath);
	return true;
}
bool ReadImgColorMapXml(const char *strXmlPath, int *pColorMap, bool bFast)
{
	if (!IsPathExist(strXmlPath)) return false;
	pugi::xml_document ColorMapXmlDoc;
	pugi::xml_parse_result ret = ColorMapXmlDoc.load_file(strXmlPath);
	if (ret.status != pugi::status_ok) return false;
	bool bReadOk = false;
	for (pugi::xml_node RootNode = ColorMapXmlDoc.first_child(); RootNode != NULL; RootNode = RootNode.next_sibling())
	{
		pugi::xml_node Node;
		Node = RootNode.first_child();
		if (strcmp(Node.name(), "Approximate") != 0 || Node.text().as_bool() != bFast)
		{
			continue;
		}
		else
		{
			int nCount = 0;
			int nGray8 = 0;
			for (Node = Node.next_sibling(); Node != NULL; Node = Node.next_sibling())
			{
				int nCutGray = Node.text().as_int();
				for (int i = nCount; i <= nCutGray; i++)
				{
					*(pColorMap + i) = nGray8;
				}
				nGray8++;
				nCount = nCutGray;
			}
			if (nGray8 == MAX_SIZE_BUF8) bReadOk = true;
			break;
		}
	}
	if (bReadOk) return true;
	return false;
}

char* findImageTypeGDAL(const char* strDetImgName)
{
	char* ps = strrchr((char*)strDetImgName, '.');
	string strExt = ps;
	transform(strExt.begin(), strExt.end(), strExt.begin(), tolower);
	if (strExt == ".bmp") return "BMP";
	else if (strExt == ".jpg" || strExt == "jpeg") return "JPEG";
	else if (strExt == ".png") return "PNG";
	else if (strExt == ".tif" || strExt == "tiff")  return "GTiff";
	else if (strExt == ".gif") return "GIF";
	else return"";
}
vector<char*> StrTok(const char* str, const char* strTok)
{
	vector<char*> vPart;
	char strInfo[MAX_SIZE_FILE_PATH]; strcpy(strInfo, str);
	char* p = strtok(strInfo, strTok);
	while (p != NULL)
	{
		vPart.push_back(p);
		p = strtok(NULL, strTok);
	}
	return vPart;
}

static bool g_bRegisterGdal = false;
CMImage::CMImage()
{
	if (!g_bRegisterGdal)
	{
		GDALAllRegister();         //利用GDAL读取图片，先要进行注册  
		OGRRegisterAll();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径

		g_bRegisterGdal = true;
	}
	InitImg();
}

CMImage::~CMImage()
{
	// 	memset(m_strImgFilePath, 0, MAX_SIZE_FILE_PATH);
	// 	memset(m_strStasticXmlPath, 0, MAX_SIZE_FILE_PATH);
	// 	memset(m_strWKTInfo, 0, MAX_SIZE_FILE_PATH);
}

void CMImage::InitImg()
{
	//	memset(m_strImgFilePath, 0, MAX_SIZE_FILE_PATH);
	m_pDataSet = NULL;
	m_nBands = 0;
	m_nCols = 0;
	m_nRows = 0;
	m_eBufType = BUFTYPE_BYTE;
}
bool CMImage::OpenImg(const char* strImgPath, bool bReadOnly /* = true */)
{
	if (!IsPathExist(strImgPath)) return false;
	GDALAccess eAcess = bReadOnly ? GA_ReadOnly : GA_Update;
	m_pDataSet = (void *)GDALOpen(strImgPath, eAcess);
	if (m_pDataSet == NULL) return false;
	strcpy(m_strImgFilePath, strImgPath);
	m_nBands = ((GDALDataset*)m_pDataSet)->GetRasterCount();
	m_nCols = ((GDALDataset*)m_pDataSet)->GetRasterXSize();
	m_nRows = ((GDALDataset*)m_pDataSet)->GetRasterYSize();
	GDALDataType eBufType = ((GDALDataset*)m_pDataSet)->GetRasterBand(1)->GetRasterDataType();
	switch (eBufType)
	{
	case GDT_Byte:
		m_eBufType = BUFTYPE_BYTE;
		break;
	case GDT_UInt16:
		m_eBufType = BUFTYPE_UINT16;
		break;
	case GDT_Float32:
		m_eBufType = BUFTYPE_FLOAT32;
		break;
	default:
		break;
	}
	return true;
}
void CMImage::CloseImg()
{
	if (m_pDataSet)
	{
		GDALClose((GDALDataset*)m_pDataSet);
		InitImg();
	}
}
bool CMImage::SaveImg(const char* strSavePath, int nCols, int nRows, int nBands, BYTE*pMem, const char *pszFormat /* = JPEG */)
{
	if (strcmp(pszFormat, "GTiff") == 0)
	{
		GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
		if (poDriver == NULL) return false;
		GDALDataset *WriteDataSet = poDriver->Create(strSavePath, nCols, nRows, nBands, GDT_Byte, NULL);
		int pBandMap[3] = { 1,2,3 };
		if (WriteDataSet->RasterIO(GF_Write, 0, 0, nCols, nRows, pMem, nCols, nRows, GDT_Byte, nBands, NULL, nBands * 1, nCols*nBands * 1, 1) == CE_Failure)
		{
			return false;
		}
		GDALClose(poDriver);
		GDALClose(WriteDataSet); WriteDataSet = NULL;
		return true;
	}
	else
	{
		if (strSavePath==NULL || pMem == NULL || nCols < 1 || nRows < 1 || nBands < 1)
		{
			return false;
		}

		char* GType = findImageTypeGDAL(strSavePath);
		if (GType=="") { return false; }
		GDALDriver *pMemDriver = NULL;
		pMemDriver = GetGDALDriverManager()->GetDriverByName("MEM");
		if (pMemDriver == NULL) { return false; }

		GDALDataset * pMemDataSet = pMemDriver->Create("", nCols, nRows, 3, GDT_Byte, NULL);
		GDALRasterBand *pBand = NULL;
		int nLineCount = nCols * 3;
		int pBandMap[3] = { 1,2,3 };
		CPLErr err = pMemDataSet->RasterIO(GF_Write, 0, 0, nCols, nRows, pMem, nCols, nRows, GDT_Byte, 3, pBandMap, nBands * 1, nLineCount, 1);
		if (err == CE_Failure) return false;
		GDALDriver *pDstDriver = NULL;
		pDstDriver = (GDALDriver *)GDALGetDriverByName(GType);
		if (pDstDriver == NULL) { return false; }

		pDstDriver->CreateCopy(strSavePath, pMemDataSet, FALSE, NULL, NULL, NULL);
		GDALClose(pMemDataSet);
		return true;
	}
}

bool CMImage::SavePointShp(const char* strSavePath, MstuMchModel modelInfo, const char *prj)
{
	//注册shape文件驱动
	const char* pszDriverName = "ESRI Shapefile";
	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	if (poDriver == NULL) return false;

	//创建数据源-shape文件;
	GDALDataset  *poDS;
	//创建一个叫myshapefile的目录，存放生成的文件;
	//如果名字有.shp后缀，则直接在当前目录下生成文件;
	poDS = poDriver->Create(strSavePath, 0, 0, 0, GDT_Unknown, NULL);   //创建数据源
	if (poDS == NULL) return false;

	OGRSpatialReference oSRS;
	oSRS.SetFromUserInput(prj);  //将获得的空间参考信息字符串做为文本一次性赋给矢量数据的OGRSpatialReference对象;

	OGRLayer *poLayer;
	poLayer = poDS->CreateLayer("point_out", &oSRS, wkbUnknown, NULL);
	if (poLayer == NULL) return false;

	//创建属性表
	OGRFieldDefn secondField("X", OFTReal);
	OGRFieldDefn thirdField("Y", OFTReal);
	OGRFieldDefn FourthField("Note", OFTInteger);
	secondField.SetWidth(32);
	secondField.SetPrecision(8);
	thirdField.SetWidth(32);
	thirdField.SetPrecision(8);
	FourthField.SetWidth(32);
	if (poLayer->CreateField(&secondField) != OGRERR_NONE || poLayer->CreateField(&thirdField) != OGRERR_NONE
		|| poLayer->CreateField(&FourthField) != OGRERR_NONE)
	{
		return false;
	}

	double x, y;
	for (int ciri = 0; ciri < modelInfo.vecObjPtXY.size(); ++ciri)
	{
		OGRPoint pt;
		x = modelInfo.vecObjPtXY[ciri].x;
		y = modelInfo.vecObjPtXY[ciri].y;
		pt.setX(x);
		pt.setY(y);

		Point2D ptResxy = modelInfo.vecPtResxy[ciri];
		int nPointType = int(sqrt(ptResxy.x*ptResxy.x + ptResxy.y*ptResxy.y) + 0.5);

		OGRFeature *poFeature;
		poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		poFeature->SetField("X", x);
		poFeature->SetField("Y", y);
		poFeature->SetField("Note", nPointType);

		poFeature->SetGeometry(&pt);
		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) return false;
		OGRFeature::DestroyFeature(poFeature);
	}
	GDALClose(poDS);
	return true;
}

bool CMImage::IsOpen()
{
	if (m_pDataSet == NULL) return false;
	else return true;
}
int CMImage::GetBands()
{
	return m_nBands;
}
int CMImage::GetCols()
{
	return m_nCols;
}
int CMImage::GetRows()
{
	return m_nRows;
}
int CMImage::GetBufSize()
{
	switch (m_eBufType)
	{
	case BUFTYPE_BYTE:
		return GDALGetDataTypeSize(GDT_Byte) / 8;
		break;
	case BUFTYPE_UINT16:
		return GDALGetDataTypeSize(GDT_UInt16) / 8;
		break;
	case BUFTYPE_FLOAT32:
		return GDALGetDataTypeSize(GDT_Float32) / 8;
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////
/************************************CMImage4Check********************************/

CMImage4Check::CMImage4Check()
{
	m_eImgType = IMGTYPE_DOM;
	for (int i = 0; i < MAX_SIZE_BUF16; i++)
	{
		m_pColorMap16To8[i] = INVALID_VALUE;
	}
}


CMImage4Check::~CMImage4Check()
{
}
bool CMImage4Check::OpenImage(const char * strPath, bool bReadOnly)
{
	if (CMImage::OpenImg(strPath, bReadOnly))
	{
		sprintf(m_strStasticXmlPath, "%s.aux.xml", m_strImgFilePath);
		sprintf(m_strColorMapPath, "%s.color.xml", m_strImgFilePath);
		sprintf(m_strTfwInfoPath, "%s.tfw", m_strImgFilePath);
		ReadGeoInfo();
		
		return true;
	}

	return false;
}
void CMImage4Check::ReadGeoInfo()
{
	double geo[6];
	((GDALDataset*)m_pDataSet)->GetGeoTransform(geo);
	m_stuGeoInfo.dResolutionX = geo[1];
	m_stuGeoInfo.dResolutionY = geo[5];
	m_stuGeoInfo.dGeoRect.left = geo[0];
	m_stuGeoInfo.dGeoRect.right = geo[0] + geo[1] * (m_nCols/* - 1*/);
	m_stuGeoInfo.dGeoRect.top = geo[3];
	m_stuGeoInfo.dGeoRect.bottom = geo[3] + geo[5] * (m_nRows/* - 1*/);
	strcpy(m_strWKTInfo, ((GDALDataset*)m_pDataSet)->GetProjectionRef());
	vector<char*> vstrWKTPart = StrTok(m_strWKTInfo, "[],");
	if (vstrWKTPart.size() == 0)
	{
		m_stuGeoInfo.eProjInfo = PROJECT_NONE;
	}
	else if (strcmp(vstrWKTPart[0], COORDINATE_HEAD_PRJ) == 0)
	{
		m_stuGeoInfo.eProjInfo = PROJECT_UTM;
// 		m_RectGeoXY = m_stuGeoInfo.dGeoRect;
// 		m_dResolutionX = m_stuGeoInfo.dResolutionX;
// 		m_dResolutionY = m_stuGeoInfo.dResolutionY;
	}
	else if (strcmp(vstrWKTPart[0], COORDINATE_HEAD_BLH) == 0)
	{
		m_stuGeoInfo.eProjInfo = PROJECT_BLH;
// 		UTMCoor xyLT, xyRB;
// 		int zone = (m_stuGeoInfo.dGeoRect.CenterPoint().x + 180.0) / 6 + 1;
// 		LatLonToUTMXY(DegToRad(m_stuGeoInfo.dGeoRect.top), DegToRad(m_stuGeoInfo.dGeoRect.left), zone, xyLT);
// 		LatLonToUTMXY(DegToRad(m_stuGeoInfo.dGeoRect.bottom), DegToRad(m_stuGeoInfo.dGeoRect.right), zone, xyRB);
// 		m_RectGeoXY = CRectD(xyLT.x, xyLT.y, xyRB.x, xyRB.y);
// 		m_dResolutionX = (xyRB.x - xyLT.x) / GetCols();
// 		m_dResolutionY = (xyRB.y - xyLT.y) / GetRows();
	}
	for (int i = 0; i < vstrWKTPart.size(); i++)
	{
		if (strcmp(vstrWKTPart[i], COORDINATE_HEAD_BLH) == 0)
		{
			if (strcmp(vstrWKTPart[i + 1], COORDINATE_WGS84) == 0)
			{
				m_stuGeoInfo.eCoordinateSysInfo = COORDINATE_SYS_WGS84;
				break;
			}
		}
	}
}

char *CMImage4Check::GetProjectionRef()
{
	return m_strWKTInfo;
}

bool CMImage4Check::ScanValidCorners(vector<Point2D>&pCornersPt, int nTskId, int nTskNum, int nBlockSize /* = 100 */, pCallBackProFun pFun /* = NULL */, void* pPara3OfpFun /* = NULL */)
{
	if (!IsOpen()) return false;
	int nBands = GetBands();
	int nRows = GetRows();
	int nCols = GetCols();
	int nBits = GetBufSize();
	double dResolutionX = GetResolutionX();
	double dResolutionY = GetResolutionY();
	Point2D ptLT = m_stuGeoInfo.dGeoRect.LeftTop();
	int nStepRow, nStepCol, nStepRom = 200;
	void *pRead = NULL;
	int nMark = 0, rMark = 0, cMark = 0;


	/*扫描左上角点*/
	cprintf("扫描左上角点...");
	nStepRow = nBlockSize;
	for (int r = 0, nMark = 0; r < nRows / 2, nMark == 0; r += nStepRow)
	{
		nStepRow = min(nBlockSize, nStepRom * 1024 * 1024 * 8 / nBits / nBands / nCols);
		nStepCol = nCols;
		int stCol = 0, nColsRead = nStepCol;
		int stRow = r, nRowsRead = min(nStepRow, nRows - stRow);
		if (nBits == 1)
		{
			BYTE*pMem = new BYTE[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(BYTE));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);
			for (int i = 0; i < nRowsRead; i++)
			{
				for (int j = 0; j < nColsRead; j++)
				{
					for (int b = 0; b < nBands; b++)
					{
						if (*(pMem + i*nColsRead*nBands + j*nBands + b) > 0)
						{
							rMark = stRow + i;
							cMark = stCol + j;
							nMark++;
							pCornersPt[0].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[0].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
		else if (nBits == 2)
		{
			unsigned short*pMem = new unsigned short[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(unsigned short));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);
			for (int i = 0; i < nRowsRead; i++)
			{
				for (int j = 0; j < nColsRead; j++)
				{
					for (int b = 0; b < nBands; b++)
					{
						if (*(pMem + i*nColsRead*nBands + j*nBands + b) > 0)
						{
							rMark = stRow + i;
							cMark = stCol + j;
							nMark++;
							pCornersPt[0].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[0].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
	}
	if (pFun) pFun((nTskId * 4 + 1)*100.0 / (nTskNum * 4), "Catching tfw info", pPara3OfpFun);
	cprintf("%.8lf(%d) %.8lf(%d)\n", pCornersPt[0].x, cMark, pCornersPt[0].y, rMark);
	/*扫描左下角点*/
	cprintf("扫描左下角点...");
	nMark = rMark = cMark = 0; nStepCol = nBlockSize;
	for (int c = 0, nMark = 0; c < nCols / 2, nMark == 0; c += nStepCol)
	{
		nStepCol = min(nBlockSize, nStepRom * 1024 * 1024 * 8 / nBits / nBands / nRows);
		//		nStepCol = 1;
		nStepRow = nRows;
		int stCol = c, nColsRead = min(nStepCol, nCols - stCol);;
		int stRow = 0, nRowsRead = nStepRow;
		if (nBits == 1)
		{
			BYTE*pMem = new BYTE[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(BYTE));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);

			for (int i = 0; i < nColsRead; i++)
			{
				for (int j = 0; j < nRowsRead; j++)
				{
					for (int b = 0; b < nBands; b++)
					{
						if (*(pMem + j*nColsRead*nBands + i*nBands + b) > 0)
						{
							rMark = stRow + j;
							cMark = stCol + i;
							nMark++;
							pCornersPt[1].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[1].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
		else if (nBits == 2)
		{
			unsigned short*pMem = new unsigned short[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(unsigned short));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);
			for (int i = 0; i < nColsRead; i++)
			{
				for (int b = 0; b < nBands; b++)
				{
					for (int j = 0; j < nRowsRead; j++)
					{
						if (*(pMem + j*nColsRead*nBands + i*nBands + b) > 0)
						{
							rMark = stRow + j;
							cMark = stCol + i;
							nMark++;
							pCornersPt[1].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[1].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
	}
	if (pFun) pFun((nTskId * 4 + 2)*100.0 / (nTskNum * 4), "Catching tfw info", pPara3OfpFun);
	cprintf("%.8lf(%d) %.8lf(%d)\n", pCornersPt[1].x, cMark, pCornersPt[1].y, rMark);
	/*扫描右下角点*/
	cprintf("扫描右下角点...");
	nMark = rMark = cMark = 0; nStepRow = nBlockSize;
	for (int r = nRows - 1, nMark = 0; r > nRows / 2, nMark == 0; r -= nStepRow)
	{
		nStepRow = min(nBlockSize, nStepRom * 1024 * 1024 * 8 / nBits / nBands / nCols);
		//	nStepRow = 1;
		nStepCol = nCols;
		int stCol = 0, nColsRead = nStepCol;
		int stRow = max(0, r - nStepRow), nRowsRead = min(nStepRow, nRows - stRow);
		if (nBits == 1)
		{
			BYTE*pMem = new BYTE[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(BYTE));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);
			for (int i = nRowsRead - 1; i >= 0; i--)
			{
				for (int j = nColsRead - 1; j >= 0; j--)
				{
					for (int b = 0; b < nBands; b++)
					{
						if (*(pMem + i*nColsRead*nBands + j*nBands + b) > 0)
						{
							rMark = stRow + i;
							cMark = stCol + j;
							nMark++;
							pCornersPt[2].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[2].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
		else if (nBits == 2)
		{
			unsigned short*pMem = new unsigned short[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(unsigned short));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);
			for (int i = nRowsRead - 1; i >= 0; i--)
			{
				for (int j = nColsRead - 1; j >= 0; j--)
				{
					for (int b = 0; b < nBands; b++)
					{
						if (*(pMem + i*nColsRead*nBands + j*nBands + b) > 0)
						{
							rMark = stRow + i;
							cMark = stCol + j;
							nMark++;
							pCornersPt[2].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[2].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
	}
	if (pFun) pFun((nTskId * 4 + 3)*100.0 / (nTskNum * 4), "Catching tfw info", pPara3OfpFun);
	cprintf("%.8lf(%d) %.8lf(%d)\n", pCornersPt[2].x, cMark, pCornersPt[2].y, rMark);
	/*扫描右上角点*/
	cprintf("扫描右上角点...");
	nMark = rMark = cMark = 0; nStepCol = nBlockSize;
	for (int c = nCols - 1, nMark = 0; c > nCols / 2, nMark == 0; c -= nStepCol)
	{
		nStepCol = min(nBlockSize, nStepRom * 1024 * 1024 * 8 / nBits / nBands / nRows);
		//		nStepCol = 1;
		nStepRow = nRows;
		int stCol = max(0, c - nStepCol), nColsRead = min(nStepCol, nCols - stCol);;
		int stRow = 0, nRowsRead = nStepRow;
		if (nBits == 1)
		{
			BYTE*pMem = new BYTE[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(BYTE));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);
			for (int j = nColsRead - 1; j >= 0; j--)
			{
				for (int b = 0; b < nBands; b++)
				{
					for (int i = 0; i < nRowsRead; i++)
					{

						if (*(pMem + i*nColsRead*nBands + j*nBands + b) > 0)
						{
							rMark = stRow + i;
							cMark = stCol + j;
							nMark++;
							pCornersPt[3].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[3].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
		else if (nBits == 2)
		{
			unsigned short*pMem = new unsigned short[nColsRead*nRowsRead*nBands];
			memset(pMem, 0, nColsRead*nRowsRead*nBands * sizeof(unsigned short));
			ReadImg(stCol, stRow, nColsRead, nRowsRead, nColsRead, nRowsRead, pMem);
			for (int j = nColsRead - 1; j >= 0; j--)
			{
				for (int b = 0; b < nBands; b++)
				{
					for (int i = 0; i < nRowsRead; i++)
					{

						if (*(pMem + i*nColsRead*nBands + j*nBands + b) > 0)
						{
							rMark = stRow + i;
							cMark = stCol + j;
							nMark++;
							pCornersPt[3].x = ptLT.x + dResolutionX*cMark;
							pCornersPt[3].y = ptLT.y + dResolutionY*rMark;
							break;
						}
					}
					if (nMark > 0)break;
				}
				if (nMark > 0)break;
			}
			delete[]pMem; pMem = NULL;
		}
	}
	if (pFun) pFun((nTskId * 4 + 4)*100.0 / (nTskNum * 4), "Catching tfw info", pPara3OfpFun);
	cprintf("%.8lf(%d) %.8lf(%d)\n", pCornersPt[3].x, cMark, pCornersPt[3].y, rMark);
	if (m_stuGeoInfo.eProjInfo == PROJECT_NONE)
	{
		for (int i = 0; i < 4; i++)
		{
			pCornersPt[i].y = GetRows() - pCornersPt[i].y;
		}
	}
	return true;
}
void CMImage4Check::ReadTfwInfo(int nTskId, int nTskNum, bool bSave2File /* = true */, pCallBackProFun pFun /* = NULL */, void* pPara3OfpFun/* = NULL*/)
{
	if (m_pDataSet == NULL) OpenImage(m_strImgFilePath);
	if (m_stuGeoInfo.vecValidVertex[0] == Point2D())
	{
//		vector<Point2D>().swap(m_stuGeoInfo.vecValidVertex);
		if (IsPathExist(m_strTfwInfoPath))
		{

			FILE*pfR = fopen(m_strTfwInfoPath, "r");
			if (pfR != NULL)
			{
				m_stuGeoInfo.vecValidVertex.resize(4);
				char strLine[MAX_SIZE_FILE_LINE]; memset(strLine, 0, MAX_SIZE_FILE_LINE);
				int nCount = 0;
				while (fgets(strLine, MAX_SIZE_FILE_LINE, pfR))
				{
					strLine[strlen(strLine)] = '\0';
					Point2D pt;
					sscanf(strLine, "%lf %lf", &pt.x, &pt.y);
					m_stuGeoInfo.vecValidVertex[nCount] = pt;
					nCount++;
					if (nCount == 4) break;
				}
			}
			if (pFun) pFun((nTskId * 4 + 4)*100.0 / (nTskNum * 4), "Catching tfw info", pPara3OfpFun);
			fclose(pfR);
		}
		else
		{
			m_stuGeoInfo.vecValidVertex.resize(4);
			if (ScanValidCorners(m_stuGeoInfo.vecValidVertex, nTskId, nTskNum, 100, pFun, pPara3OfpFun) && bSave2File)
			{
				FILE*pfR = fopen(m_strTfwInfoPath, "w");
				for (int i = 0; i < 4; i++)
				{
					fprintf(pfR, "%.8lf %.8lf\n", m_stuGeoInfo.vecValidVertex[i].x, m_stuGeoInfo.vecValidVertex[i].y);
				}
				fclose(pfR);
			}
		}
	}
	CloseImg();
}


double CMImage4Check::GetResolutionX()
{
	return m_stuGeoInfo.dResolutionX;
}
double CMImage4Check::GetResolutionY()
{
	return m_stuGeoInfo.dResolutionY;
}

MstuGeoInfo CMImage4Check::GetGeoInfo()
{
	return m_stuGeoInfo;
}
double CMImage4Check::GetBandMaxValue(int nBandIdx, bool bFast /* = false */)
{
	if (nBandIdx<1 || nBandIdx>m_nBands || m_pDataSet == NULL) return INVALID_VALUE;
	double dMaxMin[2]; int nRet;
	GDALDataset *pDataSet = (GDALDataset*)m_pDataSet;
	GDALRasterBand *pBand = pDataSet->GetRasterBand(nBandIdx);
	dMaxMin[1] = pBand->GetMaximum(&nRet);
	if (!nRet)
	{
		if (CE_None != pBand->ComputeRasterMinMax(bFast, dMaxMin))
			return INVALID_VALUE;
		/*dMaxMin[1] = pBand->GetMaximum(&nRet);*/
		//if (!nRet) return INVALID_VALUE;
	}
	return dMaxMin[1];
}
double CMImage4Check::GetBandMinValue(int nBandIdx, bool bFast /* = false */)
{
	if (nBandIdx<1 || nBandIdx>m_nBands || m_pDataSet == NULL) return INVALID_VALUE;
	double dMaxMin[2]; int nRet;
	GDALDataset *pDataSet = (GDALDataset*)m_pDataSet;
	GDALRasterBand *pBand = pDataSet->GetRasterBand(nBandIdx);
	dMaxMin[0] = pBand->GetMinimum(&nRet);
	if (!nRet)
	{
		if (CE_None != pBand->ComputeRasterMinMax(bFast, dMaxMin))
			return INVALID_VALUE;
		/*dMaxMin[0] = pBand->GetMinimum(&nRet);*/
		//if (!nRet) return INVALID_VALUE;
	}
	return dMaxMin[0];
}
double CMImage4Check::GetBandMeanValue(int nBandIdx, bool bFast /* = false */)
{
	if (nBandIdx<1 || nBandIdx>m_nBands || m_pDataSet == NULL) return INVALID_VALUE;
	GDALDataset *pDataSet = (GDALDataset*)m_pDataSet;
	double dMaxValueOfBand;
	double dMinValueOfband;
	double dMeanValueOfBand;
	double dDevValueOfBand;
	GDALRasterBand *pBand = pDataSet->GetRasterBand(nBandIdx);
	pBand->ComputeStatistics(bFast, &dMinValueOfband, &dMaxValueOfBand, &dMeanValueOfBand, &dDevValueOfBand, NULL, NULL);
	return dMeanValueOfBand;
}
double CMImage4Check::GetBandStaDevValue(int nBandIdx, bool bFast /* = false */)
{
	if (nBandIdx<1 || nBandIdx>m_nBands || m_pDataSet == NULL) return INVALID_VALUE;
	GDALDataset *pDataSet = (GDALDataset*)m_pDataSet;
	double dMaxValueOfBand;
	double dMinValueOfband;
	double dMeanValueOfBand;
	double dDevValueOfBand;
	/*统计波段灰度最值*/
	GDALRasterBand *pBand = pDataSet->GetRasterBand(nBandIdx);
	pBand->ComputeStatistics(bFast, &dMinValueOfband, &dMaxValueOfBand, &dMeanValueOfBand, &dDevValueOfBand, NULL, NULL);
	return dDevValueOfBand;
}
int * CMImage4Check::GetBandHistogram(int nBandIdx, double dMax, double dMin, bool bFast /* = false */)
{
	if (nBandIdx<1 || nBandIdx>m_nBands || m_pDataSet == NULL) return false;
	GDALDataset *pDataSet = (GDALDataset*)m_pDataSet;
	GDALRasterBand *pBand = pDataSet->GetRasterBand(nBandIdx);
	int nHistCount = dMax - dMin + 1;
	GUIntBig *pHist = new GUIntBig[nHistCount]; memset(pHist, 0, sizeof(GUIntBig)*nHistCount);
	pBand->GetHistogram(dMin - 0.5, dMax + 0.5, nHistCount, (GUIntBig*)pHist, false, bFast, NULL, NULL);
	return (int*)pHist;
}
int *CMImage4Check::GetColorMap16To8(bool bFast /* = false */)
{
	if (m_pDataSet == NULL) return NULL;
	if (m_pColorMap16To8[0] != INVALID_VALUE) return m_pColorMap16To8;
	if (!ReadImgColorMapXml(m_strColorMapPath, m_pColorMap16To8, bFast))
	{
		if (m_eBufType != BUFTYPE_BYTE)
		{
			int nBandCount2Stretch = min(m_nBands, 3);
		//	if (bFast) nBandCount2Stretch = 1;
			double *pdMax = new double[nBandCount2Stretch];
			double *pdMin = new double[nBandCount2Stretch];
			for (int i = 0; i < nBandCount2Stretch; i++)
			{
				pdMax[i] = -1e10;
				pdMin[i] = 1e10;
			}
			for (int i = 0; i < nBandCount2Stretch; i++)
			{
				pdMax[i] = GetBandMaxValue(i + 1, false);
				pdMin[i] = GetBandMinValue(i + 1, false);
			}
			double dMax = pdMax[0], dMin = pdMin[0];
			for (int i = 0; i < nBandCount2Stretch; i++)
			{
				dMax = max(dMax, pdMax[i]);
				dMin = min(dMin, pdMin[i]);
			}
			int nHist = dMax - dMin + 1;
			int *pHist = new int[nHist];
			int nSumAll = 0;
			memset(pHist, 0, nHist * sizeof(int));
			for (int i = 0; i < nBandCount2Stretch; i++)
			{
				int *pHistI = new int[nHist];
				pHistI = GetBandHistogram(i + 1, dMax, dMin, false);
				if (pHistI)
				{
					for (int i = 0; i < nHist; i++)
					{
						(*(pHist + i)) += (*(pHistI + i));
						nSumAll += (*(pHistI + i));
					}
					delete[] pHistI;
				}

			}
			/*计算拉伸截断灰度值*/
			int nCutNum = nSumAll*CUT_PERCENT_16To8;
			int nCutMin = dMin - 1, nCutMax = dMax + 1;
			int nSum = 0;
			for (int i = dMin; nSum < nCutNum; i++)
			{
				nSum += pHist[i];
				nCutMin = i;
			}
			for (int i = dMax; nSum < nCutNum; i--)
			{
				nSum += pHist[i];
				nCutMax = i;
			}
			/*16转8灰度线性拉伸*/
			for (int i = 0; i < MAX_SIZE_BUF16; i++)
			{
				if (i <= nCutMin) *(m_pColorMap16To8 + i) = 0; else
					if (i >= nCutMax) *(m_pColorMap16To8 + i) = MAX_SIZE_BUF8 - 1; else
						*(m_pColorMap16To8 + i) = int(MAX_SIZE_BUF8*(i - nCutMin) / (nCutMax - nCutMin));
				if (*(m_pColorMap16To8 + i) >= MAX_SIZE_BUF8) *(m_pColorMap16To8 + i) = MAX_SIZE_BUF8 - 1; else
					if (*(m_pColorMap16To8 + i) < 0) *(m_pColorMap16To8 + i) = 0;
			}
		}
		else
		{
			for (int i = 0; i < MAX_SIZE_BUF16; i++)
			{
				*(m_pColorMap16To8 + i) = min(i, MAX_SIZE_BUF8 - 1);
			}
		}
		WriteImgColorMapXml(m_strColorMapPath, m_pColorMap16To8, bFast);
	}
	return m_pColorMap16To8;
}

bool CMImage4Check::BuildPryamid(bool bReBuild /* = false */, MenmResampleMode eMode/* = RESMODE_NEARST */,
	int nBottomSize /* = 64 */, pCallBackProFun pFun /* = NULL */, void *pProgressCtrl /* = NULL */)
{
	if (m_pDataSet == NULL) return false;
	GDALDataset* pDataSet = (GDALDataset*)m_pDataSet;
	char strOvr[MAX_SIZE_FILE_PATH]; sprintf(strOvr, "%s.ovr", m_strImgFilePath);
	if (IsPathExist(strOvr))
	{
		if (bReBuild)
			DeleteFile(strOvr);
		else
		{
	//		pFun(1, NULL, pProgressCtrl);
			return true;
		}
	}
	GDALDriverH hDriver = GDALGetDatasetDriver(pDataSet);
	char strDriver[MAX_SIZE_FILE_LINE];
	strcpy(strDriver, GDALGetDriverShortName(hDriver));
	if (EQUAL(strDriver, "HFA") || EQUAL(strDriver, "PCIDSK"))
	{
		CloseImg();//img/pix创建内金字塔，其他格式创建外金字塔
		OpenImg(m_strImgFilePath, false);
	}
	//获取影像基本信息并生成金字塔
	if (nBottomSize < 0) nBottomSize = 32;
	int nWidth = m_nCols;
	int nHeight = m_nRows;
	int nPixelAll = nWidth*nHeight; //影像像素总数
	int nPixelTop = nBottomSize*nBottomSize;  //顶层金字塔像素数 64*64
	int NumCur = nPixelAll;
	int nGap = 4;
	int nTemp = 0;
	do
	{
		nTemp++;
		NumCur /= nGap;
	} while (NumCur > nPixelTop);
	int *anLevels = new int[nTemp]; memset(anLevels, 0, nTemp * sizeof(int));
	int nLevelCount = 0; //金字塔级数
	NumCur = nPixelAll / nGap;
	do //计算金字塔级数，2->top
	{
		anLevels[nLevelCount] = static_cast<int>(pow(2.0, nLevelCount + 2));
		nLevelCount++;
		NumCur /= nGap;
	} while (NumCur > nPixelTop);
	//CString strResample = "nearest";//最邻近采样方式
	char strResampleMode[MAX_SIZE_FILE_LINE]; memset(strResampleMode, 0, MAX_SIZE_FILE_LINE);
	switch (eMode)
	{
	case RESMODE_NEARST:
		strcpy(strResampleMode, "NEARST");
		break;
	case RESMODE_GUASS:
		strcpy(strResampleMode, "GUASS");
		break;
	case RESMODE_CUBIC:
		strcpy(strResampleMode, "CUBIC");
		break;
	case RESMODE_AVAERAGE:
		strcpy(strResampleMode, "AVERAGE");
		break;
	default:
		break;
	}
	if (pDataSet->BuildOverviews(strResampleMode, nLevelCount, anLevels, 0, NULL, (GDALProgressFunc)pFun, pProgressCtrl) != CE_None) /*(nLevelCount > 0 && GDALBuildOverviews(hDataSet, strResample, nLevelCount, anLevels, 0, NULL, NULL, NULL) != CE_None)*/
	{
		return false;
	}
	delete[]anLevels; anLevels = NULL;
	return true;
}
bool CMImage4Check::ReadImg8(int stCol, int stRow, int nCols, int nRows,
	int nMemSizeX, int nMemSizeY, BYTE *pMem)
{
	if (m_pDataSet == NULL) return false;
	GDALDataset* pDataset = (GDALDataset*)m_pDataSet;
	stCol = max(stCol, 0); nCols = min(GetCols() - stCol, nCols);
	stRow = max(stRow, 0); nRows = min(GetRows() - stRow, nRows);
	int nBandRead = min(m_nBands, 4);
	int *pBandMap = new int[nBandRead];
	for (int i = 0; i < nBandRead; i++)
	{
		*(pBandMap + i) = i + 1;;
	}
	int nBitSize = GetBufSize();
	CPLErr eRet;
	if (m_eBufType == BUFTYPE_BYTE)
	{
		eRet = pDataset->RasterIO(GF_Read, stCol, stRow, nCols, nRows, pMem, nMemSizeX, nMemSizeY, GDT_Byte,
			nBandRead, pBandMap, nBandRead * nBitSize, nBandRead * nBitSize * nMemSizeX, nBitSize);
		if (CE_None != eRet)
		{
			delete[]pBandMap;
			return false;
		}
	}
	else if (m_eBufType == BUFTYPE_UINT16)
	{
		unsigned short *pRead = new unsigned short[nBandRead*nMemSizeX*nMemSizeY];
		memset(pRead, 0, sizeof(unsigned short)*nBandRead*nMemSizeX*nMemSizeY);
		eRet = pDataset->RasterIO(GF_Read, stCol, stRow, nCols, nRows, pRead, nMemSizeX, nMemSizeY, GDT_UInt16,
			nBandRead, pBandMap, nBandRead * nBitSize, nBandRead * nBitSize * nMemSizeX, nBitSize);

		if (CE_None != eRet)
		{
			delete[]pBandMap;
			return false;
		}
		int *pMap16To8 = GetColorMap16To8(false);
		for (int i = 0; i < nBandRead*nMemSizeX*nMemSizeY; i++)
		{
			*(pMem + i) = *(BYTE*)(pMap16To8 + (*(pRead + i)));
		}
		delete[]pRead; pRead = NULL;
	}
	delete[]pBandMap;
	return true;
}
void CMImage4Check::ReadData4Texture(float stCol, float stRow, int nShowSizX, int nShowSizY, float fScale, int *pShowBandOrder, BYTE * pData4Tex)
{
	int nCols = min(int(nShowSizX / fScale), GetCols());
	int nRows = min(int(nShowSizY / fScale), GetRows());
	BYTE *data = NULL;
	if (fScale <= 1) {
		data = new BYTE[nShowSizX*nShowSizY*GetBands()];
		ReadImg8(stCol, stRow, nCols, nRows, nShowSizX, nShowSizY, data);

		BYTE* pRed = data + pShowBandOrder[0];
		BYTE* pGreen = data + pShowBandOrder[1];
		BYTE* pBlue = data + pShowBandOrder[2];
		BYTE* pPixel = pData4Tex;
		for (int i = 0; i < nShowSizX*nShowSizY; i++)
		{
			*pPixel++ = *(pRed + GetBands() * i);
			*pPixel++ = *(pGreen + GetBands() * i);
			*pPixel++ = *(pBlue + GetBands() * i);
			if (*(pRed + GetBands() * i) == 0 &&
				*(pGreen + GetBands() * i) == 0 &&
				*(pBlue + GetBands() * i) == 0)
			{
				*pPixel++ = 0;
			}
			else
			{
				*pPixel++ = 1;
			}

		}
	}
	else
	{
		nCols += 8; nRows += 8;
		nCols = min(nCols, GetCols() - stCol);
		nRows = min(nRows, GetRows() - stRow);
		data = new BYTE[nCols*GetBands()*nRows];
		ReadImg8(stCol, stRow, nCols, nRows, nCols, nRows, data);
		fScale = 1 / fScale;
		BYTE *pRowRed = data + pShowBandOrder[0];
		BYTE *pRowGreen = data + pShowBandOrder[1];
		BYTE *pRowBlue = data + pShowBandOrder[2];
		BYTE *pPixel = pData4Tex;
		float dy = stRow - int(stRow);
		for (int r = 0; r < nShowSizY; r++) {
			float dx = stCol - int(stCol);
			BYTE* pRed = pRowRed;
			BYTE* pGreen = pRowGreen;
			BYTE* pBlue = pRowBlue;
			for (int c = 0; c < nShowSizX; c++) {
				*pPixel++ = *pRed;
				*pPixel++ = *pGreen;
				*pPixel++ = *pBlue;
				if (*pRed == 0 && *pGreen == 0 && *pBlue == 0)
					*pPixel++ = 0;
				else *pPixel++ = 1;
				dx += fScale;
				if (dx >= 1) {
					dx -= 1;
					pRed += GetBands();
					pGreen += GetBands();
					pBlue += GetBands();
				}
			}
			dy += fScale;
			if (dy >= 1) {
				dy -= 1;
				pRowRed += nCols*GetBands();
				pRowGreen += nCols*GetBands();
				pRowBlue += nCols*GetBands();
			}
		}
	}
	delete[] data; data = NULL;
}
bool CMImage4Check::ReadImg(int stCol, int stRow, int nCols, int nRows,
	int nMemSizeX, int nMemSizeY, void *pMem)
{
	if (m_pDataSet == NULL) return false;
	GDALDataset* pDataset = (GDALDataset*)m_pDataSet;
	stCol = max(stCol, 0); nCols = min(GetCols() - stCol, nCols);
	stRow = max(stRow, 0); nRows = min(GetRows() - stRow, nRows);
	int nBandRead = min(m_nBands, 4);
	int *pBandMap = new int[nBandRead];
	for (int i = 0; i < nBandRead; i++)
	{
		*(pBandMap + i) = i + 1;;
	}
	int nBitSize = GetBufSize();
	if (m_eBufType == BUFTYPE_BYTE)
	{
		BYTE*pRead = (BYTE*)pMem;
		if (CE_None != pDataset->RasterIO(GF_Read, stCol, stRow, nCols, nRows, pRead, nMemSizeX, nMemSizeY, GDT_Byte,
			nBandRead, pBandMap, nBandRead * nBitSize, nBandRead * nBitSize * nMemSizeX, nBitSize))
			return false;
	}
	else if (m_eBufType == BUFTYPE_UINT16)
	{
		unsigned short *pRead = (unsigned short*)pMem;
		if (CE_None != pDataset->RasterIO(GF_Read, stCol, stRow, nCols, nRows, pRead, nMemSizeX, nMemSizeY, GDT_UInt16,
			nBandRead, pBandMap, nBandRead * nBitSize, nBandRead * nBitSize * nMemSizeX, nBitSize))
			return false;
	}
	if (pBandMap) delete[]pBandMap;
	return true;
}


void CMImage4Check::SetImgType(MenmImageType eType)
{
	m_eImgType = eType;
}
void CMImage4Check::SetImgPath(const char * strImgPath)
{
	strcpy(m_strImgFilePath, strImgPath);
	sprintf(m_strStasticXmlPath, "%s.aux.xml", m_strImgFilePath);
	sprintf(m_strColorMapPath, "%s.color.xml", m_strImgFilePath);
	sprintf(m_strTfwInfoPath, "%s.tfw", m_strImgFilePath);
}

void CMImage4Check::SetTfwInfo(vector<Point2D> vecVertexValid)
{
	if (vecVertexValid.size() != 4)return;
	m_stuGeoInfo.vecValidVertex = vecVertexValid;
}

MenmImageType CMImage4Check::GetImgType()
{
	return m_eImgType;
}

char * CMImage4Check::GetImgPath()
{
	return m_strImgFilePath;
}

Point2D CMImage4Check::ImgPt2Geo(Point2D ptImg)
{
	Point2D ptGeo;
	ptGeo.x = m_stuGeoInfo.dGeoRect.left + ptImg.x*m_stuGeoInfo.dResolutionX;
	ptGeo.y = m_stuGeoInfo.dGeoRect.top + ptImg.y*m_stuGeoInfo.dResolutionY;
	return ptGeo;
}

Point2D CMImage4Check::GeoPt2Img(Point2D ptGeo)
{
	Point2D ptImg;
	ptImg.x = (ptGeo.x - m_stuGeoInfo.dGeoRect.left) / m_stuGeoInfo.dResolutionX;
	ptImg.y = (ptGeo.y - m_stuGeoInfo.dGeoRect.top) / m_stuGeoInfo.dResolutionY;
	return ptImg;
}

