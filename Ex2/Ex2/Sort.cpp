#include<fstream>
#include<vector>
#include<iostream>
#include<thread>
#include<time.h>
#include<Windows.h>
#include "SortThread.h"
#include"QuickSort.h"
using namespace std;
const int MAX_THREAD = 32;
const int MAX_NUMBER = 1e7;
int arr[MAX_NUMBER];// danh sách các phần tử cần sắp xếp
int numThread;// số thread thực hiện việc sắp xếp
int sizeArr = 0;// số phần tử cần sắp xếp
// Nối các mảng đã sắp xếp lại
void mergeSort(int arrSort[], int posStart[], int posEnd[]) 
{
	int* arrSorted = new int[sizeArr];
	for (int i = 0; i < sizeArr; i++) {
		arrSorted[i] = arrSort[i];
	}
	for (int i = 0; i < sizeArr; i++) {
		int minId = 0, minCost = INT32_MAX;
		for (int j = 0; j < numThread; j++) {
			int idS = posStart[j];
			int idE = posEnd[j];
			if (idS <= idE) {
				if (minCost > arrSorted[idS]) {
					minCost = arrSorted[idS];
					minId = j;
				}
			}
		}
		arrSort[i] = minCost;
		posStart[minId]++;
	}
}
// đọc mảng cần sắp xếp
bool readInput() 
{
	fstream configFile("config.txt", ios::in | ios::out);
	char numthread[100];
	// cấp phát bộ nhớ cho mảng
	// Đọc file cấu hình từ file config.txt
	while (!configFile.eof()) {
		memset(numthread, 0, sizeof(numthread));
		configFile.getline(numthread, sizeof(numthread));
		string th = string(numthread);
		string delimiter = ":";
		// Tách chuỗi
		string configName = th.substr(0, th.find(delimiter));
		string cost = th.substr((th.find(delimiter) + delimiter.length()), th.length() - 1);
		// Đọc giá trị thông số
		if (configName == "NUM_THREAD") {
			numThread = atoi(cost.c_str());
			if (numThread > MAX_THREAD || numThread <= 0 ) {
				cout << "Number of thread is overlimit" << endl;
				return false;
			}
		}
		if (configName == "ARR_SIZE") {
			sizeArr = atoi(cost.c_str());
			if (sizeArr > MAX_NUMBER || sizeArr <= 0) {
				cout << "Number element is out of range" << endl;
				return false;
			}
		}
	}
	configFile.close();
	return true;
}

bool readData() 
{
	fstream file("input.txt", ios::in | ios::app);
	// Đọc mảng các số cần sắp xếp
	//arr = new int[sizeArr];
	if (!file.is_open()) {
		cout << "File is'nt exist" << endl;
		return false;
	}
	int num = 0;
	sizeArr = 0;
	while (!file.eof()) {
		char ch;
		try {
			file.get(ch);
		}
		catch (exception e) {
			cout << "file content is'nt text" << endl;
			return false;
		}
		if (ch == ' ' || ch == '\n') {
			if (num != -1) {
				arr[sizeArr] = num;
				sizeArr++;
			}
			num = -1;
		}
		else {
			if (num == -1) {
				num = 0;
			}
			int x = int(ch) - 48;
			// kiểm tra nội dung trong fil có phải là số hay không
			if (x < 0 || x> 9) {
				cout << "Input file must be number" << endl;
				return false;
			}
			
			num = num * 10 + int(ch) - 48;
		}
	}
	if (num != -1) {
		arr[sizeArr] = num;
		sizeArr++;
	}
	// file không có nội dung
	if (sizeArr == 0) {
		cout << "File is empty" << endl;
		return false;
	}
	file.close();
	return true;
}
// ghi kết quả đã sắp xếp ra mh
void printOutPut() {
	fstream ouput("output.txt", ios::out);
	for (int i = 0; i < sizeArr; i++) {
		ouput << arr[i] << " ";
	}
	ouput.close();
}
// Sinh dữ liệu ngẫu nhiên
void generateData() {
	// sinh dữ liệu ngẫu nhiên
	fstream input;
	input.open("input.txt", ios::out);
	for (int i = 0; i < sizeArr; i++) {
		int d = rand();
		input << d << ' ';
	}
	input.close();
}
int main() {
	if (!readInput()) {
		return 0;
	}
	//generateData();
	// Đọc các dữ liệu vào

	if (!readData()) {
		return 1;
	}
	
	// Tạo các Thread để sắp xếp
	// tính số lượng các phần tử trong mỗi nhóm để sắp xếp
	int groupLength = sizeArr / numThread;
	// tạo các thread để sắp xếp
	HANDLE h[MAX_THREAD];
	int posStart[MAX_THREAD];
	int posEnd[MAX_THREAD];
	clock_t t_start = clock();
	for (int i = 0; i < numThread; i++) {
		int start = i * groupLength;
		int end = min((i + 1) * groupLength, sizeArr) - 1;
		posStart[i] = start;
		posEnd[i] = end;
		thread* td  = new thread(SortThread(),arr, start, end);
		h[i] = td->native_handle();
		//td->join();
	}
	//// đợi các thread tính toán xong
	WaitForMultipleObjects(numThread, h, true, INFINITE);
	// sắp xếp lại các mảng để được mảng sắp xếp hoàn chỉnh
	printf("\nTime taken to all thread sort done: %.4fs\n", (float)(clock() - t_start) / CLOCKS_PER_SEC);
	mergeSort(arr, posStart, posEnd);
	clock_t t_end = clock();
	// in thời gian tính toán ra màn hình
	printf("\nTime taken to sort: %.4fs\n", (float)(t_end - t_start) / CLOCKS_PER_SEC);
	//printOutPut();
	cin.get();
	return 0;
}
