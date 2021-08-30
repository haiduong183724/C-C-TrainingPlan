#include<iostream>
#include<thread>
#include<fstream>
#include<Windows.h>
#include<vector>
#include <algorithm>
#include "Sort.h"
#include"SystemSort.h"
#include "FindValueInVector.h"
#include"FindValueEqual.h"
#include "Print.h"
#include "ConsolePrint.h"

using namespace std;
vector<int> arr;
vector<int> findValue;
int foundPosition = 0;
HANDLE hSorted, hFound, hPrint, hMutex;
void createEvent() {
	hSorted = CreateEventA(NULL, true, false, NULL);// Event sắp xếp xong
	hFound = CreateEventA(NULL, true, false, NULL);// Event đọc dữ liệu và tìm kiếm vị trí xong.
	hPrint = CreateEventA(NULL, true, true, NULL);// Event in giá trị xong.
	hMutex = CreateMutex(NULL, false, NULL);
}
//Hàm đọc dữ liệu đầu vào cần sắp xếp
bool readInputData() {
	fstream file("input.txt", ios::in | ios::out);
	// Đọc mảng các số cần sắp xếp
	int num = 0;
	while (!file.eof()) {
		char ch;
		file.get(ch);
		if (ch == ' ' || ch == '\n') {
			if (num != -1) {
				arr.push_back(num);
			}
			num = -1;
		}
		else {
			if (num == -1) {
				num = 0;
			}
			int x = int(ch) - 48;
			if (x < 0 || x > 9) {
				cout << "file input to read must be number" << endl;
				arr.clear();
				return false;
			}
			num = num * 10 + x;
		}
	}
	file.close();
	return true;
}
// Hàm đọc dữ liệu đầu vào để tìm kiếm
bool readFindData() {
	fstream file("search.txt", ios::in | ios::out);
	// Đọc mảng các số cần sắp xếp
	int num = 0;
	while (!file.eof()) {
		char ch;
		file.get(ch);
		if (ch == ' ' || ch == '\n') {
			if (num != -1) {
				findValue.push_back(num);
			}
			num = -1;
		}
		else {
			if (num == -1) {
				num = 0;
			}
			int x = int(ch) - 48;
			if (x < 0 || x > 9) {
				cout << "file input to find must be number" << endl;
				findValue.clear();
				return false;
			}
			num = num * 10 + x;
		}
	}
	if (findValue.size() == 0) {
		cout << "file input is no content" << endl;
		return false;
	}
	file.close();
	return true;
}
// Thread B đọc dữ liệu đầu vào và sắp xếp chúng
void BThread() {
	cout << "thread B start" << endl;
	// đợi mutex
	WaitForSingleObject(hMutex, INFINITE);
	// đọc dữ liệu
	if (!readInputData()) {
		SetEvent(hSorted);
		ReleaseMutex(hMutex);
		return;
	}
	// khởi tạo đối tượng sắp xếp
	Sort* s = new SystemSort();
	s->sortVector(&arr);
	SetEvent(hSorted);
	// giải phóng mutex
	ReleaseMutex(hMutex);
	cout << "thread B end" << endl;
}
// Thread C đọc các giá trị tìm kiếm tìm kiếm chúng
void CThread() {
	cout << "thread C start" << endl;
	if (!readFindData()) {
		SetEvent(hFound);
		return;
	}
	// đợi đến khi thread B sắp xếp xong
	WaitForSingleObject(hSorted, INFINITE);
	// khởi tạo một đối tượng tìm kiếm trên vector
	if (arr.size() == 0) {
		cout << "C: sort arr is null" << endl;
		SetEvent(hFound);
		return;
	}
	FindValueInVector<int>* f  = new FindValueEqual(arr);
	for (int value : findValue) {
		// đợi quyền sở hữu mutex và thread A đã làm xong việc in kết quả
		WaitForSingleObject(hPrint, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		// Tìm kiếm giá trị tiếp theo
		foundPosition = f->find(value);
		// giải phóng quyền mutex và thiết lập lại các event
		SetEvent(hFound);
		ResetEvent(hPrint);
		ReleaseMutex(hMutex);
	}
	cout << "thread C end" << endl;

}
// Thread A in kết quả ra màn hình
void AThread() {
	cout << "thread A start" << endl;
	// đợi đến khi thread B sắp xếp xong
	WaitForSingleObject(hSorted, INFINITE);
	if (arr.size() == 0) {
		cout << "A: sort arr is null" << endl;
		return;
	}
	if (findValue.size() == 0) {
		cout << "A: find arr is null " << endl;
		return ;
	}
	// duyệt mảng các giá trị tìm kiếm
	for (int i = 0; i < findValue.size(); i++) {
		// đợi đến khi có một kết quả, có quyền sở hữu mutex
		WaitForSingleObject(hFound, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		// tạo đối tượng in ra màn hình console
		Print* p = new ConsolePrint();
		char printStr[1024];
		memset(printStr, 0, sizeof(printStr));
		// nếu không tìm thấy, in ra màn hình không tìm thấy
		if (foundPosition == -1) {
			sprintf_s(printStr, "value %d is'nt found in vector", findValue[i]);
		}
		// nếu có, in ra vị trí tìm thấy
		else {
			sprintf_s(printStr, "value %d is found at position %d in vector", findValue[i], foundPosition);
		}
		p->print(printStr);
		// giải phóng quyền sở hữu mutex, thiết lập lại các event cần thiết.
		SetEvent(hPrint);
		ResetEvent(hFound);
		ReleaseMutex(hMutex);
	}
	cout << "thread A end" << endl;
}
// Hàm sinh dữ liệu ngẫu nhiên
void generateData() {
	fstream fileInput, fileFind;
	fileInput.open("input.txt", ios::out);
	for (int i = 0; i < 10000; i++) {
		fileInput << rand() << " ";
	}
	fileInput.close();
	fileFind.open("search.txt", ios::out);
	for (int i = 0; i < 100; i++) {
		fileFind << rand() << " ";
	}
	fileFind.close();
}
int main() {
	generateData();
	createEvent();
	thread sortarr(BThread);
	thread findvalue(CThread);
	thread printresult(AThread);
	sortarr.join();// Thread B
	findvalue.join();// Thread C
	printresult.join();// Thread A
	cin.get();
}