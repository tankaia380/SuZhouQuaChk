#pragma once

#include "opengl/glew.h"
#include "opengl/glut.h"
#include "MyFunctions.h"
#include <afxwin.h>
enum MyVectorType
{//ʸ������
	vLine=0,  //��
	vPoint=1, //��
	vPane = 2 //��
};

//opengl�������
//bool IsGlExtensionSupported(const char* extension); //Opengl VBO���

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
	//�����ڵ�VBO _nVertexNum�ڵ����,_eTypeVBO�ڵ�ͼ������ _bSupportVBO�Ƿ�֧��VBO
	void CreateVectorVBO(int _nVertexNum, MyVectorType _eType = vLine, bool _bSupportVBO = true);   //����ʸ������VBO
	void FillVectorVBO(GLdouble *_pV, GLdouble* _pColor, int _nVertexNum);//���VBO����
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
