#ifndef LOGFILE_H_INCLOUDE
#define LOGFILE_H_INCLOUDE

#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

#define TKTIME 9999999

class LogFile
{
public:
	LogFile(string str_file_path);
	LogFile();
	~LogFile();
public:
	LogFile operator<<(char* str_out);
	LogFile operator<<(int n_out);
	LogFile operator<<(double n_out);
	void	SetPrecision(int n_precision);
	void	close();
	
private:
	SYSTEMTIME	sysTime;
	int			n_precision_;
	FILE		*fp;
};



#endif