#include "Directory.h"

Directory::Directory() {

}

Directory::Directory(const char* path)
{
	strcat(directoryPath, path);
	listFile = settingFile();
}

char* Directory::getPath()
{
	return directoryPath;
}

vector<FileInfomation> Directory::settingFile()
{
	// tạo handle trỏ tới thư mục cần theo dõi
	vector<FileInfomation> files ;
	WIN32_FIND_DATAA FindData;
	char fullPath[1024];
	memset(fullPath, 0, sizeof(fullPath));
	sprintf(fullPath, "%s\\*.*", directoryPath);
	HANDLE hFile = FindFirstFileA(fullPath, &FindData);
	if (hFile != INVALID_HANDLE_VALUE) {
		int num = 0;
		// duyệt lần lượt các file trong thư mục
		while (FindNextFileA(hFile, &FindData)) {
			char filePath[1024]{ 0 };
			if (num > 0) {// bỏ qua file đầu tiên vì nó không có ý nghĩa
				sprintf(filePath, "%s\\%s", directoryPath, FindData.cFileName);
				FileInfomation f(filePath);
				files.push_back(f);
			}
			num++;
		}
	}
	return files;
}
void Directory::traceFile()
{
	// kiểm tra lần lượt các file xem có file nào bị sửa nội dung hay không
	for (int i = 0; i < listFile.size(); i++) {
		if (listFile[i].fileStatus == FILE_NOR) {
			if (listFile[i].updateFile()) {
				logFile(listFile[i], FILE_EDIT);
			}
		}
	}
}

void Directory::checkFile(vector<FileInfomation> delList, vector<FileInfomation> addList)
{
	int i = 0;
	// kiểm tra các file trùng nhau trong 2 danh sách và loại bỏ chúng
	while (i < delList.size() && !addList.empty() && !delList.empty()){
		bool isEq = false;
		for (int j = 0; j < addList.size(); j++) {
			if (delList[i] == addList[j]) {
				delList.erase(delList.begin() + i);
				addList.erase(addList.begin() + j);
				isEq = true;
				break;
			}
		}
		if (!isEq) {
			i++;
		}
	}
	// các file còn lại ở danh sách cũ là các file đã bị xóa hoặc sửa tên
	for (int i = 0; i < delList.size(); i++) {
		logFile(delList[i], FILE_DELETE);
		delFile(delList[i]);
	}
	// các file còn lại ở danh sách mới là các file được thêm hoặc đổi tên
	for (int i = 0; i < addList.size(); i++) {
		logFile(addList[i], FILE_ADD);
		listFile.push_back(addList[i]);
	}
}
void Directory::traceDirectory()
{
	vector<FileInfomation> oldList = listFile;
	vector<FileInfomation> newList = settingFile();
	checkFile(oldList, newList);
}
void Directory::logFile(FileInfomation f, FileStatus fstatus)
{
	listFileChange.push_back(make_pair(f, fstatus));
	/*switch (fstatus)
	{
	case FILE_DELETE: 
	{
		
		break;
	}
	case FILE_ADD:
	{
		printf("file %s has been added\n", f.getFileName());
		break;
	}
	case FILE_EDIT:
	{
		printf("file %s has changed\n", f.getFileName());
		break;
	}
	default:
		break;
	}*/
}
void Directory::delFile(FileInfomation fileDeleted)
{
	for (int i = 0; i < listFile.size(); i++) {
		if (listFile[i] == fileDeleted) {
			char filePath[1024]{ 0 };
			sprintf(filePath, "%s\\%s", directoryPath, fileDeleted.getFileName());
			remove(filePath);
			listFile.erase(listFile.begin() + i);
		}
	}
}
void Directory::addFile(FileInfomation fileAdded)
{
	delFile(fileAdded);
	listFile.push_back(fileAdded);
}
void Directory::clear()
{
	for (int i = 0; i < listFile.size(); i++) {
		char filePath[1024]{ 0 };
		sprintf(filePath, "%s\\%s", directoryPath, listFile[i].getFileName());
		remove(filePath);
	}
	listFile.clear();
}
//#include<iostream>
//int main() {
//	Directory d("Debug");
//	while (true) {
//		d.traceDirectory();
//		d.traceFile();
//		Sleep(50);
//	}
//	return 0;
//}
﻿#include "Directory.h"

Directory::Directory() {

}

Directory::Directory(const char* path)
{
	strcat(directoryPath, path);
	listFile = settingFile();
}

char* Directory::getPath()
{
	return directoryPath;
}

vector<FileInfomation> Directory::settingFile()
{
	// tạo handle trỏ tới thư mục cần theo dõi
	vector<FileInfomation> files ;
	WIN32_FIND_DATAA FindData;
	char fullPath[1024];
	memset(fullPath, 0, sizeof(fullPath));
	sprintf(fullPath, "%s\\*.*", directoryPath);
	HANDLE hFile = FindFirstFileA(fullPath, &FindData);
	if (hFile != INVALID_HANDLE_VALUE) {
		int num = 0;
		// duyệt lần lượt các file trong thư mục
		while (FindNextFileA(hFile, &FindData)) {
			char filePath[1024]{ 0 };
			if (num > 0) {// bỏ qua file đầu tiên vì nó không có ý nghĩa
				sprintf(filePath, "%s\\%s", directoryPath, FindData.cFileName);
				FileInfomation f(filePath);
				files.push_back(f);
			}
			num++;
		}
	}
	return files;
}
void Directory::traceFile()
{
	// kiểm tra lần lượt các file xem có file nào bị sửa nội dung hay không
	for (int i = 0; i < listFile.size(); i++) {
		if (listFile[i].fileStatus == FILE_NOR) {
			if (listFile[i].updateFile()) {
				logFile(listFile[i], FILE_EDIT);
			}
		}
	}
}

void Directory::checkFile(vector<FileInfomation> delList, vector<FileInfomation> addList)
{
	int i = 0;
	// kiểm tra các file trùng nhau trong 2 danh sách và loại bỏ chúng
	while (i < delList.size() && !addList.empty() && !delList.empty()){
		bool isEq = false;
		for (int j = 0; j < addList.size(); j++) {
			if (delList[i] == addList[j]) {
				delList.erase(delList.begin() + i);
				addList.erase(addList.begin() + j);
				isEq = true;
				break;
			}
		}
		if (!isEq) {
			i++;
		}
	}
	// các file còn lại ở danh sách cũ là các file đã bị xóa hoặc sửa tên
	for (int i = 0; i < delList.size(); i++) {
		logFile(delList[i], FILE_DELETE);
		delFile(delList[i]);
	}
	// các file còn lại ở danh sách mới là các file được thêm hoặc đổi tên
	for (int i = 0; i < addList.size(); i++) {
		logFile(addList[i], FILE_ADD);
		listFile.push_back(addList[i]);
	}
}
void Directory::traceDirectory()
{
	vector<FileInfomation> oldList = listFile;
	vector<FileInfomation> newList = settingFile();
	checkFile(oldList, newList);
}
void Directory::logFile(FileInfomation f, FileStatus fstatus)
{
	listFileChange.push_back(make_pair(f, fstatus));
	/*switch (fstatus)
	{
	case FILE_DELETE: 
	{
		
		break;
	}
	case FILE_ADD:
	{
		printf("file %s has been added\n", f.getFileName());
		break;
	}
	case FILE_EDIT:
	{
		printf("file %s has changed\n", f.getFileName());
		break;
	}
	default:
		break;
	}*/
}
void Directory::delFile(FileInfomation fileDeleted)
{
	for (int i = 0; i < listFile.size(); i++) {
		if (listFile[i] == fileDeleted) {
			char filePath[1024]{ 0 };
			sprintf(filePath, "%s\\%s", directoryPath, fileDeleted.getFileName());
			remove(filePath);
			listFile.erase(listFile.begin() + i);
		}
	}
}
void Directory::addFile(FileInfomation fileAdded)
{
	delFile(fileAdded);
	listFile.push_back(fileAdded);
}
void Directory::clear()
{
	for (int i = 0; i < listFile.size(); i++) {
		char filePath[1024]{ 0 };
		sprintf(filePath, "%s\\%s", directoryPath, listFile[i].getFileName());
		remove(filePath);
	}
	listFile.clear();
}
//#include<iostream>
//int main() {
//	Directory d("Debug");
//	while (true) {
//		d.traceDirectory();
//		d.traceFile();
//		Sleep(50);
//	}
//	return 0;
//}
