#pragma once
#ifndef REPORTCHANGE_H
#define REPORTCHANGE_H
#include"Directory.h"
class ReportChange
{
private:
	SOCKET c;
	int id = 0;
public:
	void operator ()(Directory* d, SOCKET s, HANDLE* hMutex, int* Id);
	void SendReport(FileInfomation fOld,FileInfomation fNew , FileStatus s);
};
#endif // !REPORTCHANGE_H



