#include "LogFile.h"

LogFile::LogFile(string str_file_path)
{
	ZeroMemory(&sysTime, sizeof(sysTime));
	n_precision_ = 8;
    fp = fopen(str_file_path.c_str(),"at");
}

LogFile::LogFile()
{
	ZeroMemory(&sysTime, sizeof(sysTime));
	n_precision_ = 8;
	char str_path[512] = "\0";
	GetModuleFileName(NULL, str_path, 512);
	char* pt = strrchr(str_path, '.'); *pt = 0; strcpy(pt, ".log");
	fp = fopen(str_path, "at");
}

LogFile::~LogFile()
{
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
}

LogFile LogFile::operator<<(char* str_out)
{
	if (fp == NULL) return NULL;
	fprintf(fp, "%s",str_out);
	return *this;
}

LogFile LogFile::operator<<(int n_out)
{
	if (fp == NULL) return NULL;

	if (n_out == TKTIME)
	{
		GetLocalTime(&sysTime);
		fprintf(fp, "%d/%d/%d/%d/%d/%d: ", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		return *this;
	}
	fprintf(fp, "%d", n_out);
	return *this;
}

LogFile LogFile::operator<<(double n_out)
{
	if (fp == NULL) return NULL;
	fprintf(fp, "%.*lf", n_precision_, n_out);
	return *this;
}

void LogFile::SetPrecision(int n_precision)
{
	n_precision_ = n_precision;
}

void LogFile::close()
{
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
}

