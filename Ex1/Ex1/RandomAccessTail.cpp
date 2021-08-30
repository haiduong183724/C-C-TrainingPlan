#include "RandomAccessTail.h"
#include<string>
#include<fstream>
#include<iostream>
using namespace std;
//void RandomAccessTail::setFileName(char* filename)
//{
//	strcpy_s(file_name, filename);
//}
void RandomAccessTail::tailFile(int numRow)
{
	if (!checkFile()) {
		return;
	}
	fstream fio(file_name, ios::in || ios::out);
	fio.seekg(0, ios::end);
	char content[256];
	// số byte của một file
	int pos = fio.tellg();
	// Số dòng cần đọc
	int row = numRow;
	int i = 1;
	do {
		// chuyển con trỏ đến vị trí xuống dòng
		fio.seekg(pos - i);
		char ch;
		fio.get(ch);
		// vì kí tự xuống dòng tốn 2 byte nên giảm con trỏ đi thêm 1 byte so vs kí tự thường
		if (ch == '\n'){
			row--;
			i++;
		}
		i++;
	} while (row > 0 && i <= pos);
	if (i>pos) {
		fio.seekg(0, ios::beg);
	}
	char* buffer = new char[i];
	memset(buffer, 0, i);
	fio.read(buffer, i - 1);
	cout << buffer << endl;
	//// đọc file từ vị trí pos đến cuối file.
	//while (!fio.eof())
	//{
	//	char line[256];
	//	memset(line, 0, sizeof(line));
	//	fio.getline(line, sizeof(line));
	//	cout << line << endl;
	//}
	fio.close();
}