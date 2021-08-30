#pragma once
#ifndef DIRECTORY_H
#define DIRECTORY_H
#include<string.h>
#include<vector>
#include<Windows.h>
#include<string>
class Directory
{
private:
	char path[1024];
public:
	Directory(const char* path);
	bool goIn(const char* directory);
	bool goBack();
	bool goTo(const char* directory);
	bool checkFileExist(const char* file);
	char* getPath();
	bool checkFolder(const char* folder);
	std::vector<std::string> scanFolder();
};
#endif // !DIRECTORY_H



