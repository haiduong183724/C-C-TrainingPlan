#pragma once
#ifndef FILEINFOMATION_H
#define FILEINFOMATION_H
#include"DateTime.h"
#include<fileapi.h>
class FileInfomation
{
private:
	char fileName[1024]{0};
	char path[1024]{ 0 };
	DateTime createdDate;
	DateTime modifiedDate;
public:
	FileInfomation(const char* filePath);
	bool updateFile();
	char* getFileName();
	bool operator == (FileInfomation f);
};
#endif // !FILEINFOMATION_H



