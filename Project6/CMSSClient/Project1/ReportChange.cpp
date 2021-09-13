#include "ReportChange.h"
#include <iostream>
#include"TLVPackage.h"
void ReportChange::operator()(Directory* d, SOCKET s, HANDLE* hMutex, int* Id)
{
	c = s;
	while (1) {
		if (*Id != -1) {
			id = *Id;
			int send = 0;
			WaitForSingleObject(hMutex, INFINITE);
			while (!d->listFileChange.empty()) {
				send = 0;
				pair<FileInfomation, FileStatus> file = d->listFileChange.back();
				d->listFileChange.pop_back();
				// Kiểm tra có phải rename file hay không
				if (file.second == FILE_ADD) {
					if (!d->listFileChange.empty()) {
						if (d->listFileChange.back().second == FILE_DELETE) {
							pair<FileInfomation, FileStatus> file2 = d->listFileChange.back();
							
							send = SendReport(file2.first, file.first, FILE_RENAME);
							goto endloop;
						}
					}
				}
				send = SendReport(file.first, file.first, file.second);
			endloop:;
			}
			ReleaseMutex(hMutex);
			Sleep(1000);
		}
	}
}

int ReportChange::SendReport(FileInfomation fOld, FileInfomation fNew, FileStatus s)
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
	TLVPackage p(CONTROL_MESSAGE, id, strlen(request) + 8, request);
	int byteSend = send(c, p.packageValue(), p.getLength(), 0);
	return byteSend;
}
