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

BOOL GetImgPathFromTsk(char* tsk_path, vector<char* > &vec)
{
	FILE *fp;
	fp = fopen(tsk_path, "r");
	if (fp == NULL) return FALSE;
	while (!feof(fp))
	{
		char* str_temp = new char[MAXSTRSIZE];
		fscanf(fp, "%s", str_temp);
		vec.push_back(str_temp);
	}
	fclose(fp);
	return TRUE;
}

BOOL FromMux2Gray(USINT* &p_data, int n_cols, int n_rows, int n_bands)
{
	USINT* p_gary = new USINT[n_cols*n_rows];
	for (int n_ciri = 0; n_ciri < n_rows; n_ciri++)
	{
		for (int n_cirj = 0; n_cirj < n_cols; n_cirj++)
		{
			p_gary[n_ciri*n_cols + n_cirj] =
				p_data[(n_ciri*n_cols + n_cirj)*n_bands + 0] * 0.299 +
				p_data[(n_ciri*n_cols + n_cirj)*n_bands + 1] * 0.587 +
				p_data[(n_ciri*n_cols + n_cirj)*n_bands + 2] * 0.144;
		}
	}
	delete[] p_data;
	p_data = p_gary;
	return TRUE;
}

BYTE* GetSuperpixelLaber(USINT* p_data, int* p_color_table, int n_cols, int n_rows)
{
	cout << "Getting superpixels..." << endl;
	BYTE* p_laber = new BYTE[n_cols*n_rows];
	memset(p_laber, 0, n_cols*n_rows);
	for (int n_ciri = 0; n_ciri < n_cols*n_rows; ++n_ciri)
	{
		p_laber[n_ciri] = p_color_table[p_data[n_ciri]];
	}

	SLIC slic;
	USINT n_superpixel_num = int(1.0*n_cols / SUPERPIXELSIZE + 0.5)*int(1.0*n_rows / SUPERPIXELSIZE + 0.5);
	slic.GenerateSuperpixels(p_laber, n_cols, n_rows, n_superpixel_num);
	slic.Close();
	cout << "Superpixels number:" << n_superpixel_num << endl;
	return p_laber;
}

BOOL CheckDomImage(const vector<char*> vec_image_path)
{
	CMImage4Check image_check;
	if (!image_check.OpenImage(vec_image_path[0]))
	{
		log_file << TKTIME << "Image:" << vec_image_path[0] << " open failed!" << "\n";
		return FALSE;
	}

	const int n_image_check_cols = image_check.GetCols();
	const int n_image_check_rows = image_check.GetRows();
	const int n_image_check_bands = image_check.GetBands();

	int n_image_block_start_col = 0;
	int n_image_block_start_row = 0;
	int n_image_block_cols = BLOCKCOLSSIZE;
	int n_image_block_rows = BLOCKROWSSIZE;
	const int n_steps_x = int(n_image_check_cols * 1.0 / n_image_block_cols + 0.5);
	const int n_steps_y = int(n_image_check_rows * 1.0 / n_image_block_rows + 0.5);
	
	int* p_color_table = new int[65536]; memset(p_color_table, 0, 65535 * sizeof(int));
	p_color_table = image_check.GetColorMap16To8();

	for (int n_ciri = 0; n_ciri < n_steps_y; ++n_ciri)
	{
		for (int n_cirj = 0; n_cirj < n_steps_x; ++n_cirj)
		{
			n_image_block_start_col = n_cirj * n_image_block_cols;
			n_image_block_start_row = n_ciri * n_image_block_rows;
			if ((n_image_block_start_row + n_image_block_rows)>n_image_check_rows) n_image_block_rows = n_image_check_rows - n_image_block_start_row;
			else n_image_block_rows = BLOCKROWSSIZE;
			if ((n_image_block_start_col + n_image_block_cols)>n_image_check_cols) n_image_block_cols = n_image_check_cols - n_image_block_start_col;
			else n_image_block_cols = BLOCKCOLSSIZE;

			USINT* p_image_block_data = new USINT[n_image_block_cols*n_image_block_rows*n_image_check_bands];
			memset(p_image_block_data, 0, n_image_block_cols*n_image_block_rows*n_image_check_bands*sizeof(USINT));

			image_check.ReadImg(n_image_block_start_col, n_image_block_start_row, n_image_block_cols, n_image_block_rows, n_image_block_cols, n_image_block_rows, p_image_block_data);
			if (n_image_check_bands >= 3) FromMux2Gray(p_image_block_data, n_image_block_cols, n_image_block_rows, n_image_check_bands);

			BYTE* p_superpixel_laber_data = GetSuperpixelLaber(p_image_block_data, p_color_table, n_image_block_cols, n_image_block_rows);


			/*if (n_cirj == 0)
			{
				cout << "Saving superpixel..." << endl;
				image_check.SaveImg16("C:\\Users\\Administrator\\Desktop\\新建文件夹\\11.tif", n_image_block_cols, n_image_block_rows, 1, p_image_block_data);
				image_check.SaveImg("C:\\Users\\Administrator\\Desktop\\新建文件夹\\22.tif", n_image_block_cols, n_image_block_rows, 1, p_superpixel_laber_data);
				image_check.CloseImg();
				if (p_image_block_data != NULL)
				{
					delete[] p_image_block_data, p_superpixel_laber_data;
					p_image_block_data = NULL;
					p_superpixel_laber_data = NULL;
				}
				return TRUE;
			}
			if (p_image_block_data != NULL)
			{
				delete[] p_image_block_data;
				p_image_block_data = NULL;
			}*/
		}
	}
	delete[] p_color_table;
	return TRUE;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		log_file << TKTIME << "Process can't run! Because the parameters don't match: EXE+TSK!" << "\n";
		return 0;
	}

	vector<char* > vec_image_path;
	if (!GetImgPathFromTsk(argv[1], vec_image_path))
	{
		log_file << TKTIME << "Can't load image path from tsk file" << "\n";
		return 0;
	}
	if (vec_image_path.size() < 3)
	{
		log_file << TKTIME << "Images in the tsk:" << argv[1] << " is less than 3, porcess can't run!" << "\n";
		return 0;
	}

	CheckDomImage(vec_image_path);
	
	return 0;
}

