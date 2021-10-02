#include "ReportChange.h"
#include <iostream>
#include"TLVPackage.h"
void ReportChange::operator()(ClientHandleRequest* hRequest, HANDLE* hMutex)
{
	while (1) {
		if (hRequest->isConnect) {
			c = hRequest->getSocket();
			id = hRequest->getId();
			int send = 0;
			WaitForSingleObject(hMutex, INFINITE);
			while (!hRequest->d->listFileChange.empty()) {
				send = 0;
				FileChangeLog file = hRequest->d->listFileChange.back();
				hRequest->d->listFileChange.pop_back();
				// Kiểm tra có phải rename file hay không
				if (file.status == FILE_ADD) {
					if (!hRequest->d->listFileChange.empty()) {
						if (hRequest->d->listFileChange.back().status == FILE_DELETE) {
							FileChangeLog file2 = hRequest->d->listFileChange.back();
							hRequest->d->listFileChange.pop_back();
							send = SendReport(file2.fileName, file.fileName, FILE_RENAME);
							if (send < 0) {// disconnect
								hRequest->d->listFileChange.push_back(file2);
								hRequest->d->listFileChange.push_back(file);
								hRequest->isConnect = false;
								break;
							}
							goto endloop;
						}
					}
				}
				if (DateTime::Now() - file.timeChange > 2) {
					send = SendReport(file.fileName, file.fileName, file.status);
					if (send < 0) {// disconnect
						hRequest->d->listFileChange.push_back(file);
						hRequest->isConnect = false;
						break;
					}
				}
				else {
					hRequest->d->listFileChange.push_back(file);
				}
			endloop:;
			}
			ReleaseMutex(hMutex);
			Sleep(1000);
		}
	}
}

int ReportChange::SendReport(char* fOld, char* fNew, FileStatus s)
{
	char request[1024]{ 0 };
	switch (s)
	{
	case FILE_DELETE:
	{
		sprintf(request, "%s %s", "DELETE", fOld);
		break;
	}
	case FILE_EDIT:
	{
		sprintf(request, "%s %s", "EDIT", fOld);
		break;
	}
	case FILE_ADD:
	{
		sprintf(request, "%s %s", "ADD", fOld);
		break;
	}
	case FILE_RENAME:
	{
		sprintf(request, "%s %s %s", "RENAME", fOld, fNew);
		break;
	}
	default:
		break;
	}
	cout << request << endl;
	TLVPackage p(CONTROL_MESSAGE, id, strlen(request) + 8, request);
	return send(c, p.packageValue(), p.getLength(), 0);
}
