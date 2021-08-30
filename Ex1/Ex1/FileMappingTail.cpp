#include "FileMappingTail.h"
#include<string>
#include<iostream>
//void FileMappingTail::setFileName(char* fileName)
//{
//	strcpy_s(file_name, fileName);
//}
void FileMappingTail::tailFile(int numRow)
{
	if (!checkFile()) {
		return;
	}
	DWORD dwSysGran;      // chi tiết phân bổ tệp
	SYSTEM_INFO SysInfo;  // system information; used to get granularity
	GetSystemInfo(&SysInfo);
	dwSysGran = SysInfo.dwAllocationGranularity;
	HANDLE hFileMaping = mappingFile();
	int loop = fileSize / dwSysGran + fileSize%dwSysGran == 0 ? 0 : 1;// số vòng lặp để quét hết file;
	int mark = 0;// vị trí bắt đầu đọc
	// Map lần lượt các đoạn của file vào để đọc trong trường hợp file lớn và số n lớn.
	for (int i = 0; i < loop; i++) {
		DWORD dwFileMapStart = ((fileSize - i * dwSysGran)/ dwSysGran) * dwSysGran;// vị trí bắt đầu đọc
		DWORD byteRead = (i == 0) ? fileSize - dwFileMapStart : dwSysGran;// Số byte đọc
		// chuyển mapview thành mảng
		void* p = MapViewOfFile(hFileMaping, FILE_MAP_READ, 0, dwFileMapStart, fileSize - dwFileMapStart);
		// duyệt các p tử trong mảng, tìm kí tự xuống dòng.
		for(int i = byteRead - 1; i >= 0; i--) {
			char* c = (char*)p + i;
			if (c != NULL && (*c) == '\n') {
				numRow--;
				mark = dwFileMapStart + i;
				i--;
				// Khi đủ n dòng
				if (numRow == 0) {
					// in ra màn hình
					printScreen(dwFileMapStart, mark + 1, hFileMaping);
					return;
				}
			}
		}
	}
	// khi duyệt hết tất cả file mà chưa đủ n dòng => in tất cả nội dung file
	printScreen(0, 0, hFileMaping);
	CloseHandle(hFileMaping);
}

HANDLE FileMappingTail::mappingFile()
{
	HANDLE hFile = CreateFileA(file_name,
		GENERIC_READ,// quyền đọc
		0,// không chia sẻ 
		NULL,
		OPEN_EXISTING,// mở file đã tồn tại
		FILE_ATTRIBUTE_NORMAL,// file bình thường
		NULL);
	fileSize = GetFileSize(hFile, NULL);
	HANDLE hFileMapping = CreateFileMappingA(hFile,// map với file cần đọc
		NULL,
		PAGE_READONLY,// quyền đọc
		0,// tạo ánh xạ tệp là cả file.
		0,
		file_name
	);
	return hFileMapping;
}

void FileMappingTail::printScreen(DWORD start, DWORD pos, HANDLE hFileMapping)
{
	// đọc tất cả nội dung từ vị trí pos đến cuối file để in ra màn hình
	int byteRead = fileSize - start;
	void* p = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, start, byteRead);
	for (int i = pos; i < byteRead; i++) {
		char* c = (char*)p + i;
		if (c != NULL) {
			std::cout << *c;
		}
	}
}
