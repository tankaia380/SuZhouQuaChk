/***************************************************************************************
 *                    Copyright		:	whudepgrid
 *                    Author		:	tankai
 *                    Tel.			:	13349890592
 *                    Email			:	1428308325@qq.com
 *					  Created Time	:	2017.02.23
 *				      Function		:	check the quality of dom in superpixel level
****************************************************************************************/

#include "Include.h"

static LogFile log_file;

inline BOOL IsExist(LPCSTR lpstrPathName)   //判断文件是否存在 added by tk
{
	WIN32_FIND_DATA fd; HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = ::FindFirstFile(lpstrPathName, &fd);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	::FindClose(hFind); return TRUE;
}

BOOL GetImgPath(char* tsk_path, vector<char* > &vec)
{
	FILE *fp;
	fp = fopen(tsk_path, "r");
	if (fp == NULL) return FALSE;
	while (feof(fp))
	{
		char* str_temp = new char[MAXSTRSIZE];
		fscanf(fp, "%s", str_temp);
		vec.push_back(str_temp);
	}
	return TRUE;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		log_file << TKTIME << "Process can't run! Because of the parameters don't match: EXE+TSK!" << "\n";
		return 0;
	}

	vector<char* > vec_image_path;
	if (!GetImgPath(argv[1], vec_image_path))
	{
		log_file << TKTIME << "Can't load image path from tsk file" << "\n";
	}



	return 0;
}

