#include "TextFile.h"
#include<string.h>
#include<fstream>
#include <iostream>
using namespace std;
void TextFile::setFileName(char* filename)
{
	strcpy_s(file_name, filename);
}

bool TextFile::checkFile()
{
	fstream fio(file_name, ios::in | ios::out|ios::app);
	// Kiểm tra file có tồn tại chưa
	if (!fio.is_open()) {
		cout << "File is'nt exist" << endl;
		return false;
	}
	// Kiểm tra nội dung của file
	fio.seekg(0, ios::end);
	int pos = fio.tellg();
	if (pos == 0) {
		cout << "File is no content" << endl;
		return false;
	}
	return true;
}
