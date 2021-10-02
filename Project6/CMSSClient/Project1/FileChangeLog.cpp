#include "FileChangeLog.h"

FileChangeLog::FileChangeLog(char* FileName, FileStatus s)
{
	strcat(fileName, FileName);
	status = s;
	timeChange = DateTime::Now();

}

FileChangeLog FileChangeLog::getOb(char* fileName, FileStatus s)
{
	return FileChangeLog(fileName, s);
}
