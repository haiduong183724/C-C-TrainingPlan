#pragma once
#ifndef DIRECTORY_H
#define DIRECTORY_H
#include<fstream>
#include<vector>
#include "FileInfomation.h"
using namespace std;
enum FileStatus
{
FILE_DELETE = 1,
FILE_EDIT = 2,
FILE_ADD = 3,
FILE_RENAME = 4
};

class Directory
{
private: 
	char directoryPath[1024]{0};
	vector<FileInfomation> listFile;
	void checkFile(vector<FileInfomation> delList, vector<FileInfomation> addList);
public:
	vector<pair<FileInfomation, FileStatus>> listFileChange;
	Directory(const char* path);
	char* getPath();
	void settingFile();
	void traceFile();
	void traceDirectory();
	void logFile(FileInfomation f, FileStatus);
};

#endif // !DIRECTORY_H



