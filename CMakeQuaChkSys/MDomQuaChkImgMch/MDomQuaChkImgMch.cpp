// MDomQuaChkImgMch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Shlwapi.h>
#include <vector>
using namespace std;
#define MAX_SIZE_FILE_PATH 2048



CString FunGetSysExePath()
{
	char strExePath[MAX_SIZE_FILE_PATH];
	memset(strExePath, 0, MAX_SIZE_FILE_PATH);
	GetModuleFileName(NULL, strExePath, sizeof(strExePath));
	return strExePath;
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

bool ReadTskFile(CString strTsk,vector<CString> &vecLines)
{
	FILE* pfW = fopen(strTsk, "r");
	if (pfW == NULL)return false;
	char line[MAX_SIZE_FILE_PATH]; memset(line, 0, MAX_SIZE_FILE_PATH);
	while (fgets(line, MAX_SIZE_FILE_PATH, pfW))
	{
		line[strlen(line)-1] = '\0';
		vecLines.push_back(line);
	}
	if (vecLines.size() != 5) return false;
	return true;
}
int main(int nPara, _TCHAR *Paras[])
{
	if (nPara != 2)
	{
		printf("参数错误，正确参数：tsk文件路径\n");
		return -1;
	}
	CString strTsk = Paras[1];
	if (!PathFileExists(strTsk))
	{
		printf("Tsk文件路径错误！\n");
		return -1;
	}
	vector<CString> vecLines;
	if (!ReadTskFile(strTsk, vecLines))
	{
		printf("Tsk文件解析错误！\n");
		return -1;
	}
	CString strImgPath1 = vecLines[0];
	CString strImgPath2 = vecLines[1];
	CString strModelPath = vecLines[2];
	CString strMatchExeType = vecLines[3];
	CString strCheckExist = vecLines[4];
	bool bCheckExist = false;
	if (strCheckExist == "1") bCheckExist = true;
	if (bCheckExist&&PathFileExists(strModelPath))
	{
		CString strTmpPath = FunGetFileFolder(vecLines[2]) + "\\temp\\" + FunGetFileName(vecLines[2], false)+".tmp";
		FunCreateDir4Path(strTmpPath);
		FILE *pfW = fopen(strTmpPath, "w"); fclose(pfW);
		return -1;
	}
	CString strRunMach, strMchExe;
	if (strMatchExeType == "Image match by sift") strMchExe = FunGetFileFolder(FunGetSysExePath()) + "\\ImageMatch\\SiftMatchEx.exe";
	if (strMatchExeType == "Image match by harris") strMchExe = FunGetFileFolder(FunGetSysExePath()) + "\\ImageMatch\\HarrisMatchEx.exe";
	strRunMach.Format("%s %s %s -geo -output %s", strMchExe, vecLines[0], vecLines[1], vecLines[2]);
	system(strRunMach);
	CString strTmpPath = FunGetFileFolder(vecLines[2]) + "\\temp\\" + FunGetFileName(vecLines[2], false) + ".tmp";
	FunCreateDir4Path(strTmpPath);
	FILE *pfW = fopen(strTmpPath, "w"); fclose(pfW);
    return 0;
}

