#pragma once
#ifndef FILEINFOMATION_H
#define FILEINFOMATION_H
#include"DateTime.h"
#include<fileapi.h>
enum FileState {
	FILE_SYNC = 1,
	FILE_NOR = 0,
};
class FileInfomation
{
private:
	char fileName[1024]{0};
	char path[1024]{ 0 };
	DateTime createdDate;
	DateTime modifiedDate;
public:
	int fileStatus = FILE_NOR;
	FileInfomation(const char* filePath);
	FileInfomation(const char* filePath, int state);
	FileInfomation(const char* filePath,const char* fileName, DateTime ModifiedDate);
	bool updateFile();
	char* getFileName();
	bool operator == (FileInfomation f);
		
	DateTime getModifiedDate();
};
#endif // !FILEINFOMATION_H



