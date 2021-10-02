#pragma once
#ifndef REPORTCHANGE_H
#define REPORTCHANGE_H
#include"ClientHandleRequest.h"
class ReportChange
{
private:
	SOCKET c;
	int id = 0;
public:
	void operator ()(ClientHandleRequest* hRequest, HANDLE* hMutex);
	int SendReport(char* fOld,char* fNew , FileStatus s);
};
#endif // !REPORTCHANGE_H



