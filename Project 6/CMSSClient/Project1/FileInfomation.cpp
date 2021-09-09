#include "FileInfomation.h"

FileInfomation::FileInfomation(const char* filePath)
{
    WIN32_FIND_DATAA FindData;
    HANDLE hFile = FindFirstFileA(filePath, &FindData);
    if (hFile != INVALID_HANDLE_VALUE) {
        // copy fileName
        strcat(path, filePath);
        strcat(fileName, FindData.cFileName);
        // update file info
        SYSTEMTIME st;
        FileTimeToSystemTime(&(FindData.ftLastWriteTime), &st);
        modifiedDate.setDate(st);
        FileTimeToSystemTime(&(FindData.ftCreationTime), &st);
        createdDate.setDate(st);
    }
}

bool FileInfomation::updateFile()
{
    WIN32_FIND_DATAA FindData;
    HANDLE hFile = FindFirstFileA(path, &FindData);
    if (hFile != INVALID_HANDLE_VALUE) {
        SYSTEMTIME st;
        DateTime newDate;
        FileTimeToSystemTime(&(FindData.ftLastWriteTime), &st);
        newDate.setDate(st);
        if (newDate == modifiedDate) {
            return false;
        }
        return true;
        modifiedDate.setDate(st);
    }
    return false;
}
char* FileInfomation::getFileName()
{
    return fileName;
}
bool FileInfomation::operator==(FileInfomation f)
{
    if (strcmp(f.path, path) == 0) {
        return true;
    }
    return false;
}
#include<iostream>
using namespace std;
//int main() {
//    FileInfomation f("Debug");
//    while (1) {
//        if (f.updateFile()) {
//            cout << "file change" << endl;
//        }
//        Sleep(100);
//    }
//    return 0;
//}