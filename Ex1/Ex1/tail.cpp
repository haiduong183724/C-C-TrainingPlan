#include<iostream>
#include<fstream>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include"RandomAccessTail.h"
#include "FileMappingTail.h"
using namespace std;
int main(int argc, char* argv[]) {
	int n;

	// Sinh dữ liệu tự động
	//fstream fInput;
	//fInput.open("input.txt", ios::out);
	//for (int i = 0; i < 10000; i++) {
	//	int x = rand() % (126-10+1 ) + 10;
	// 	// số từ 0 đến 9
	//	if (x >= 48 && x <= 57) {
	//		fInput.put(char(x));
	//	}
	// 	// từ A đến Z
	//	if (x >= 65 && x <= 90) {
	//		fInput.put(char(x));
	//	}
	// 	// từ a đến z
	//	if (x >= 97 && x <= 122) {
	//		fInput.put(char(x));
	//	}
	//	// dấu space và xuống dòng
	//	if (x == 10 || x == 32) {
	//		fInput.put(char(x));
	//	}
	//}

	//fInput.close();
	// Tên file cần đọc
	//char fileName[1024];
	//memset(fileName, 0, sizeof(fileName));
	//strcpy_s(fileName, argv[1]);
	//sscanf_s(argv[2], "%d", &n);
	TextFile* file = new FileMappingTail() ;
	file->setFileName((char*)"input.txt");
	file->tailFile(3);
	return 0;
}