#include "Directory.h"
using namespace std;

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
				Connector->Edit(listFile[i]);
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
		delFile(delList[i]);
		logFile(delList[i], FILE_DELETE);
	}
	// các file còn lại ở danh sách mới là các file được thêm hoặc đổi tên
	for (int i = 0; i < addList.size(); i++) {
		listFile.push_back(addList[i]);
		Connector->AddFile(addList[i]);
		logFile(addList[i], FILE_ADD);
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
	fstream logFile;
	logFile.open("list_file.txt", ios::out);
	char logs[1024]{ 0 };
	strcat(logs, getLog());
	logFile << logs;
	logFile.close();
	if (fstatus == FILE_EDIT) {
		for (int i = 0; i < listFileChange.size(); i++) {
			if (strcmp(listFileChange[i].fileName, f.getFileName()) == 0) {
				if (listFileChange[i].status == FILE_ADD || listFileChange[i].status == FILE_EDIT) {
					listFileChange.erase(listFileChange.begin() + i);
				}
			}
		}
	}
	listFileChange.insert(listFileChange.begin(), FileChangeLog::getOb(f.getFileName(), fstatus));
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
			Connector->Delete(fileDeleted.getFileName());
		}
	}
}
void Directory::addFile(FileInfomation fileAdded)
{
	Connector->AddFile(fileAdded);
	delFile(fileAdded);
	listFile.push_back(fileAdded);
}
void Directory::clear()
{
	listFile = settingFile();
	for (int i = 0; i < listFile.size(); i++) {
		char filePath[1024]{ 0 };
		sprintf(filePath, "%s\\%s", directoryPath, listFile[i].getFileName());
		remove(filePath);
	}
	listFile.clear();
	Connector->ResetDb();
}
char* Directory::getLog()
{
	char logs[1024]{ 0 };
	for (int i = 0; i < listFile.size(); i++) {
		char log[1024]{ 0 };
		sprintf(log, "%s %s\n", listFile[i].getFileName(), listFile[i].getModifiedDate().dateStr());
		strcat(logs, log);
	}
	return logs;
}
void Directory::readFileListSave()
{
	listFile.clear();
	fstream logFile;
	logFile.open("list_file.txt", ios::in);
	while (!logFile.eof()) {
		char logs[1024]{ 0 }, fileName[1024]{ 0 }, dateStr[1024]{ 0 };
		logFile.getline(logs, sizeof(logs));
		sscanf(logs, "%s%s", fileName, dateStr);
		if (strlen(fileName) > 0 && strlen(dateStr) > 0) {
			DateTime modifiedDate(dateStr);
			char filePath[1024]{ 0 };
			sprintf(filePath, "%s\\%s", directoryPath, fileName);
			FileInfomation f(filePath,fileName, modifiedDate);
			listFile.push_back(f);
		}
	}
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

