#include "Directory.h"

Directory::Directory(const char* Path)
{
    memset(path, 0, sizeof(path));
    memcpy(path, Path, strlen(Path));
}

bool Directory::goIn(const char* directory)
{
    if (checkFolder(directory)) {
        return true;
    }
    return false;
}

bool Directory::goBack()
{
    if (strlen(path) == 0) {
        return false;
    }
    for (int i = strlen(path) - 1; i >= 0; i--) {
        if (path[i] == '\\') {
            memset(path + i , 0, strlen(path) - i);
            return true;
        }
    }
    return false;
}

bool Directory::goTo(const char* directory)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFile = FindFirstFileA(directory, &findData);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    memset(path, 0, sizeof(path));
    strcat_s(path, directory);
    CloseHandle(hFile);
    hFile = NULL;
    return true;
}

bool Directory::checkFileExist(const char* file)
{
    strcat_s(path, "\\");
    strcat_s(path, file);
    WIN32_FIND_DATAA findData;
    HANDLE hFile = FindFirstFileA(path, &findData);
    goBack();
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    hFile = NULL;
    return true;
}

char* Directory::getPath()
{
    return path;
}

bool Directory::checkFolder(const char* folder)
{
    strcat_s(path, "\\");
    strcat_s(path, folder);
    WIN32_FIND_DATAA findData;
    HANDLE hFile = FindFirstFileA(path, &findData);
    if (hFile == INVALID_HANDLE_VALUE) {
        goBack();
        return false;
    }
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        hFile = NULL;
        return true;
    }
    goBack();
    hFile = NULL;
    return false;
}

std::vector<std::string> Directory::scanFolder()
{
     std::vector<std::string> files;
     WIN32_FIND_DATAA findData;
     char fullPath[1024];
     memset(fullPath, 0, sizeof(fullPath));
     sprintf(fullPath, "%s\\*.*", path);
     HANDLE hFile = FindFirstFileA(fullPath, &findData);
     if (hFile != INVALID_HANDLE_VALUE) {
         while (FindNextFileA(hFile, &findData)) {
             files.push_back(findData.cFileName);
         }
     }
     return files;
}
#include<iostream>
void print(Directory d) {
    for (std::string x : d.scanFolder()) {
        std::cout << x << std::endl;
    }
}
using namespace std;
//int main() {
//    Directory d((char*)"C:\\Users\\Admin\\source");
//    d.goIn("repos");
//    print(d);
//    if (d.checkFileExist("al.txt")) {
//        cout << "exist" << endl;
//    }
//    else {
//        cout << "doesnt exist" << endl;
//    }
//    std::cin.get();
//}