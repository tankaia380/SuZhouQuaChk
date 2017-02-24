#ifndef M_Data_Type_Base_h
#define M_Data_Type_Base_h
#pragma once
#include <windows.h>
#include <vector>
using namespace std;
#define INVALID_VALUE          -99999
#define MAX_SIZE_FILE_PATH     1024
#define MAX_SIZE_FILE_LINE     1024
#define MAX_SIZE_BUF8          256
#define MAX_SIZE_BUF16         65536
#define CUT_PERCENT_16To8      0.003

template <class DataType>
class Point2_
{
public:
	typedef Point2_<DataType> MiType;
	Point2_() { x = INVALID_VALUE;y = INVALID_VALUE; }
	//		: Point2_(DataType(INVALID_VALUE), DataType(INVALID_VALUE)){}
	Point2_(const DataType _x, const DataType _y)
		: x(_x), y(_y) {}
	Point2_(const MiType &p)
		: x(p.x), y(p.y) {}
	~Point2_() {}

private:

public:
	DataType x;
	DataType y;

public:
	// define MiType operators

	MiType operator+(MiType pt) {
		return Point2_(x + pt.x, y + pt.y);
	}
	MiType operator-(MiType pt) {
		return Point2_(x - pt.x, y - pt.y);
	}
	MiType operator-() {
		return Point2_(-x, -y);
	}
	MiType operator*(MiType pt) {
		return Point2_(x *= pt.x, y *= pt.y);
	}
	MiType operator=(MiType pt) {
		return Point2_(x = pt.x, y = pt.y);
	}
	bool operator!=(MiType pt) {
		return (x != pt.x || y != pt.y);
	}
	bool operator==(MiType pt) {
		return (x == pt.x && y == pt.y);
	}
	MiType operator+=(MiType pt) {
		return (*this = *this + pt);
	}
	MiType operator-=(MiType pt) {
		return (*this = *this - pt);
	}
	MiType operator*(DataType k) {
		return Point2_(x *= k, y *= k);
	}
	MiType operator/(DataType k) {
		if (k != 0) return Point2_(x /= k, y /= k);
		else return Point2_(x, y);
	}
};

template <class DataType>
class Point3_
{
public:
	typedef Point3_<DataType> MiType;

	Point3_() { X = INVALID_VALUE;Y = INVALID_VALUE;Z = INVALID_VALUE; }
	//		: Point3_(DataType(INVALID_VALUE), DataType(INVALID_VALUE), DataType(INVALID_VALUE)){}
	Point3_(const DataType _x, const DataType _y, const DataType _z)
		: X(_x), Y(_y), Z(_z) {}
	~Point3_() {}


private:

public:
	DataType X;
	DataType Y;
	DataType Z;

public:

	MiType operator+(MiType gt) {
		return Point3_(X + gt.X, Y + gt.Y, Z + gt.Z);
	}
	MiType operator-(MiType gt) {
		return Point3_(X - gt.X, Y - gt.Y, Z - gt.Z);
	}
	MiType operator-() {
		return Point3_(-X, -Y, -Z);
	}
	MiType operator*(MiType gt) {
		return Point3_(X *= gt.X, Y *= gt.Y, Z *= gt.Z);
	}
	MiType operator/(DataType a) {
		if (a != DataType(0))
			return Point3_(X /= a, Y /= a, Z /= a);
		else return Point3_(999999999999999, 999999999999999, 9999999999999999);
	}
	MiType operator=(MiType gt) {
		return Point3_(X = gt.X, Y = gt.Y, Z = gt.Z);
	}
	bool operator==(MiType gt) {
		return (X == gt.X && Y == gt.Y && Z = gt.Z);
	}
	MiType operator+=(MiType gt) {
		return (*this = *this + gt);
	}
	MiType operator-=(MiType gt) {
		return (*this = *this - gt);
	}
	MiType operator/=(DataType a) {
		if (a != DataType(0))
			return (*this = *this / a);
		else return Point3_(999999999999999, 999999999999999, 9999999999999999);
	}
	bool operator!=(MiType pt) {
		return (X != pt.X || Y != pt.Y || Z != pt.Z);
	}
};

template <class DataType>
class CRect_
{
public:  //构造初始化
	typedef CRect_<DataType> MiType;
	CRect_() {}
	//		: CRect_(DataType(INVALID_VALUE), DataType(INVALID_VALUE), DataType(INVALID_VALUE), DataType(INVALID_VALUE)){}
	CRect_(const DataType _l, const DataType _t, const DataType _r, const DataType _b)
		: left(_l), top(_t), right(_r), bottom(_b) {}
	CRect_(Point2_<DataType> p1, Point2_<DataType> p2)
		: left(p1.x), top(p1.y), right(p2.x), bottom(p2.y) {}
	CRect_(const MiType &p)
		: left(p.left), top(p.top), right(p.right), bottom(p.bottom) {}
public:
	DataType left;
	DataType right;
	DataType top;
	DataType bottom;

	Point2_<DataType> RightTop() { return Point2_<DataType>(right, top); };
	Point2_<DataType> LeftBottom() { return Point2_<DataType>(left, bottom); };
	Point2_<DataType> LeftTop() { return Point2_<DataType>(left, top); };
	Point2_<DataType> RightBottom() { return Point2_<DataType>(right, bottom); };
	Point2_<DataType> CenterPoint() { return Point2_<DataType>((right + left) / 2, (top + bottom) / 2); };

	DataType Width() { return fabs(right - left); };
	DataType Height() { return fabs(top - bottom); };
	DataType Area() { return Width()*Height(); };

	bool operator==(MiType drect) { //矩形赋值
		return (left == drect.left&& top == drect.top&& right == drect.right&& bottom == drect.bottom);
	}
	MiType operator=(MiType drect) { //矩形赋值
		return CRect_(left = drect.left, top = drect.top, right = drect.right, bottom = drect.bottom);
	}

	MiType operator+(MiType drect) {//矩形求并
		return CRect_(left = min(left, drect.left), top = max(top, drect.top),
			right = max(right, drect.right), bottom = min(bottom, drect.bottom));
	}

public:
	bool Intersect_Rect(MiType dr) {
		if (left > dr.right || right < dr.left ||
			top<dr.bottom || bottom>dr.top)
			return false;
		return true;
	}
	bool Intersect_Pt(Point2_<DataType> pt) {
		if ((pt.x - left)*(pt.x - right) < 0 && (pt.y - top)*(pt.y - bottom) < 0)
			return true;
		return false;
	}

private:

};

typedef Point2_<double> Point2D;
typedef Point3_<double> Point3D;
typedef CRect_<double> CRectD;

#define COORDINATE_HEAD_PRJ "PROJCS"
#define COORDINATE_HEAD_BLH "GEOGCS"
#define COORDINATE_WGS84    "WGS 84"
enum MenmImageBufType
{//影像数据类型
	BUFTYPE_BYTE = 0,
	BUFTYPE_UINT16 = 1,
	BUFTYPE_FLOAT32 = 2
};
enum MenmImageBandOrder
{
	BANDORDER_RGB_RGB_RGB = 0,
	BANDORDER_RRR_GGG_BBB = 1,
	BANDORDER_BBB_GGG_RRR = 2
};
enum MenmResampleMode
{
	RESMODE_NEARST = 0,
	RESMODE_GUASS = 1,
	RESMODE_CUBIC = 2,
	RESMODE_AVAERAGE = 3
};
enum MenmCoordinateSysInfo
{
	COORDINATE_SYS_WGS84 = 0
};
enum MenmProjectInfo
{
	PROJECT_BLH = 0,
	PROJECT_UTM = 1,
	PROJECT_NONE = 2
};

struct MstuGeoInfo
{
	double dResolutionX;
	double dResolutionY;
	CRectD dGeoRect;
	vector<Point2D> vecValidVertex;
	MenmCoordinateSysInfo eCoordinateSysInfo;
	MenmProjectInfo eProjInfo;
	MstuGeoInfo()
	{
		dResolutionX = 0;
		dResolutionY = 0;
		eCoordinateSysInfo = COORDINATE_SYS_WGS84;
		eProjInfo = PROJECT_BLH;
		
//		vector<Point2D>().swap(vecValidVertex);
		vecValidVertex.resize(4);
	}
};

enum MenmImageType
{
IMGTYPE_DOM = 0,
IMGTYPE_REF = 1
};
enum MenmMatchExe
{
	MATCH_SIFT = 0,
	MATCH_HARRIS = 1
};
enum MenmModelType
{
	MODEL_TIE = 0,
	MODEL_CTL = 1,
	MODEL_NULL = 2
};
struct MstuTieChkModel
{
	int nIdxLeft;
	int nIdxRight;
	char strDomPath1[MAX_SIZE_FILE_PATH];
	char strDomPath2[MAX_SIZE_FILE_PATH];
	int nPtNum;
	double dMeanDx;
	double dMeanDy;
	double dMeanDxy;
	double dMseDx;
	double dMseDy;
	double dMseDxy;
	vector<Point2D> vecAreaPt;
	MstuTieChkModel()
	{
		nIdxLeft = -1;
		nIdxRight = -1;
		memset(strDomPath1, 0, MAX_SIZE_FILE_PATH);
		memset(strDomPath2, 0, MAX_SIZE_FILE_PATH);
		nPtNum = 0;
		dMeanDx = 0;
		dMeanDy = 0;
		dMeanDxy = 0;
		dMseDx = 0;
		dMseDy = 0;
		dMseDxy = 0;
	}
};
struct MstuGcpChkModel
{
	int nDomIdx;
	char strDomPath[MAX_SIZE_FILE_PATH];
	int nPtNum;
	double dMeanDx;
	double dMeanDy;
	double dMeanDxy;
	double dMseDx;
	double dMseDy;
	double dMseDxy;
	MstuGcpChkModel()
	{
		nDomIdx = -1;
		memset(strDomPath, 0, MAX_SIZE_FILE_PATH);
		nPtNum = 0;
		dMeanDx = 0;
		dMeanDy = 0;
		dMeanDxy = 0;
		dMseDx = 0;
		dMseDy = 0;
		dMseDxy = 0;
	}
};
struct MstuMchModel
{
	MenmModelType eType;
	int nImgIdx1;
	int nImgIdx2;
	vector<string> vecPtIdx;
	vector<Point2D> vecObjPtXY;
	vector<Point2D> vecImgPt1;
	vector<Point2D> vecImgPt2;
	vector<Point2D> vecPtResXY;
	vector<Point2D> vecPtResxy;
	vector<bool> vecPtValid;
	int nPtValid;

	MstuMchModel()
	{
		nImgIdx1 = -1;
		nImgIdx2 = -1;
		vector<string>().swap(vecPtIdx);
		vector<Point2D>().swap(vecImgPt1);
		vector<Point2D>().swap(vecImgPt2);
		vector<Point2D>().swap(vecPtResXY);
		vector<Point2D>().swap(vecPtResxy);
		vector<bool>().swap(vecPtValid);
		nPtValid = 0;

	}
	void Clear()
	{
		nImgIdx1 = -1;
		nImgIdx2 = -1;
		vector<Point2D>().swap(vecObjPtXY);
		vector<Point2D>().swap(vecImgPt1);
		vector<Point2D>().swap(vecImgPt2);
		vector<Point2D>().swap(vecPtResXY);
		vector<Point2D>().swap(vecPtResxy);
		vector<bool>().swap(vecPtValid);
		vector<string>().swap(vecPtIdx);
		nPtValid = 0;
	}
};
struct MstuPropOfImg
{
	char strFileName[MAX_SIZE_FILE_PATH];
	float dFileSizeMB;
	int nWidth;
	int nHeight;
	MenmCoordinateSysInfo eCoordinateSys;
	MenmProjectInfo eProjectInfo;
	double dResX;
	double dResY;
	MstuPropOfImg()
	{
		memset(strFileName, 0, MAX_SIZE_FILE_PATH);
		dFileSizeMB = 0;
		nWidth = 0;
		nHeight = 0;
		dResY = 0;
		dResX = 0;
	}

};
#endif
