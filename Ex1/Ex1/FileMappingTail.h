#pragma once
#include<Windows.h>
#ifndef FILEMAPPING_H
#include"TextFile.h"
class FileMappingTail : public TextFile
{
private:
	u_long fileSize;
public:
	// gán giá trị cho filename
	//void setFileName(char* fileName);
	// đọc n dòng từ cuối file
	void tailFile(int numRow);
	// mapping file
	HANDLE mappingFile();
	void printScreen(DWORD start, DWORD pos, HANDLE hFileMapping);
};

#define FILEMAPPING_H
#endif // !FILEMAPPING_H