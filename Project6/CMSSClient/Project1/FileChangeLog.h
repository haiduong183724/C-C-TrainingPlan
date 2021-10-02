#pragma once
#ifndef FILECHANGELOG_H
#define FILECHANGELOG_H
#include"DateTime.h"
enum FileStatus
{
	FILE_DELETE = 1,// file xóa
	FILE_EDIT = 2,// file chỉnh sửa
	FILE_ADD = 3,// file thêm mới
	FILE_RENAME = 4// đổi tên file
};
class FileChangeLog
{
public:
	char fileName[1024]{ 0 };
	FileStatus status;
	DateTime timeChange;
	FileChangeLog(char* FileName, FileStatus s);
	static FileChangeLog getOb(char* fileName, FileStatus s);
};
#endif // !FILECHANGELOG_H



