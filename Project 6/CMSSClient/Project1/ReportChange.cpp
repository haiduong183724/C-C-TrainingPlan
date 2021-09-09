#include "ReportChange.h"
#include <iostream>
#include"TLVPackage.h"
void ReportChange::operator()(Directory* d, SOCKET s, HANDLE* hMutex, int* Id)
{
	c = s;
	while (1) {
		if (*Id != -1) {
			id = *Id;
			WaitForSingleObject(hMutex, INFINITE);
			while (!d->listFileChange.empty()) {
				pair<FileInfomation, FileStatus> file = d->listFileChange.back();
				d->listFileChange.pop_back();
				// Kiểm tra có phải rename file hay không
				if (file.second == FILE_ADD) {
					if (!d->listFileChange.empty()) {
						if (d->listFileChange.back().second == FILE_DELETE) {
							pair<FileInfomation, FileStatus> file2 = d->listFileChange.back();
							d->listFileChange.pop_back();
							SendReport(file2.first, file.first, FILE_RENAME);
							goto endloop;
						}
					}
				}
				SendReport(file.first, file.first, file.second);
			endloop:;
			}
			ReleaseMutex(hMutex);
			Sleep(1000);
		}
	}
}

void ReportChange::SendReport(FileInfomation fOld, FileInfomation fNew, FileStatus s)
{
	char request[1024]{ 0 };
	switch (s)
	{
	case FILE_DELETE:
	{
		sprintf(request, "%s %s", "DELETE", fOld.getFileName());
		break;
	}
	case FILE_EDIT:
	{
		sprintf(request, "%s %s", "EDIT", fOld.getFileName());
		break;
	}
	case FILE_ADD:
	{
		sprintf(request, "%s %s", "ADD", fOld.getFileName());
		break;
	}
	case FILE_RENAME:
	{
		sprintf(request, "%s %s %s", "RENAME", fOld.getFileName(), fNew.getFileName());
		break;
	}
	default:
		break;
	}
	cout << request << endl;
	TLVPackage p(100, id, strlen(request) + 8, request);
	send(c, p.packageValue(), p.getLength(), 0);
}
