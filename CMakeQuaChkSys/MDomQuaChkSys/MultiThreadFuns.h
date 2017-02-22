#pragma once

struct MatchCheckPara
{
	LPVOID p;
	bool bMatchCheck;
	MatchCheckPara(LPVOID pDa, bool bmatch)
	{
		p = pDa;
		bMatchCheck = bmatch;
	}
};

void WINAPI MultiThreadImgPrePro(LPVOID p);
void WINAPI MultiThreadSavePointShpFiles(LPVOID p);
void WINAPI MultiThreadMatchCheck(LPVOID p);
