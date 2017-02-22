#pragma once

#include "opengl/glew.h"
#include "opengl/glut.h"
#include "MyFunctions.h"
#include <afxwin.h>
enum MyVectorType
{//矢量类型
	vLine=0,  //线
	vPoint=1, //点
	vPane = 2 //面
};

//opengl相关配置
//bool IsGlExtensionSupported(const char* extension); //Opengl VBO检测

class CMyViewerRender;
bool isGLExtensionSupported(const char *extension);  //VBO
void SetLogicalPalette(HPALETTE &hPalette);
bool SetupPixelFormat(CClientDC *pDC, HPALETTE &hPalett);
bool InitOpenGL(HGLRC &hRC, CClientDC*pDC, HPALETTE &hPalett);
void PrepareDraw(float r, float g, float b, float d);
void EndDraw(CClientDC *m_pDC);
void SetBackColor(CMyViewerRender& BkRender, CRectD rect, float *pR, float *pG, float *pB, bool bGradual  = true);
void DisplayString(const char* str);
class CMyViewerRender
{
public:
	CMyViewerRender();
	~CMyViewerRender();
public:
	//创建节点VBO _nVertexNum节点个数,_eTypeVBO节点图形类型 _bSupportVBO是否支持VBO
	void CreateVectorVBO(int _nVertexNum, MyVectorType _eType = vLine, bool _bSupportVBO = true);   //创建矢量绘制VBO
	void FillVectorVBO(GLdouble *_pV, GLdouble* _pColor, int _nVertexNum);//填充VBO数据
	void UpdateVectorVBO(int nStartPos, int nSize, GLdouble *pData);
	void Display(float nSize = 1,int nGap = 0);
	void DeleteVBO();
	bool IsFilled() { return m_nVertexNum > 0 ? true : false; }
	int GetVertexNum() { return m_nVertexNum; }
private:
	GLuint m_nVboId;
	GLuint m_nIdxVboId;
	int m_nVertexNum;
	bool m_bSupportVBO;
	MyVectorType m_eVertexType;
};

#define TEXTURE_BLOCK_SIZE  256
class CMyTextureRender
{
public:
	CMyTextureRender();
	~CMyTextureRender();
	void CreateTexture(int nWidth, int nHeight);
	void FillTexture(BYTE*pData);
	void DisplayTexture(CRect rect);
	void DeleteTexture();
	bool IsTextured() { return m_bFilled; }
protected:
	GLuint m_nIdTex;
	bool m_bTexted;
	bool m_bFilled;
	int m_nWidth;
	int m_nHeight;
};
