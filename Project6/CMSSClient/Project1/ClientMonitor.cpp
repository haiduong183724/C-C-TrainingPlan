#include"Directory.h"
#include<thread>
#include<iostream>
#include<mutex>
#include"ClientHandleRequest.h"
#include "TLVBuffer.h"
#include"ReportChange.h"
#include <mutex>
using namespace std;
mutex m;
string addr, clientRepoPath;
int port = 0;
HANDLE ValidatedEvent, HandleRequest;
void CreateVaildateEvent() {
	ValidatedEvent = CreateEventA(NULL, true, false, NULL);
	if (ValidatedEvent == INVALID_HANDLE_VALUE) {
		cout << "false to create event" << endl;
	}
	HandleRequest = CreateEventA(NULL, true, true, NULL);
}
void readConfig() {
	fstream file("config.txt", ios::in || ios::out);
	// đọc các cấu hình trong file cấu hình
	while (!file.eof()) {
		char line[1024];
		char configName[1024];
		char configValue[1024];
		memset(line, 0, sizeof(line));
		memset(configName, 0, sizeof(configName));
		memset(configValue, 0, sizeof(configValue));
		file.getline(line, sizeof(line));
		sscanf(line, "%s%s", configName, configValue);
		// cổng tiếp nhận của server
		if (!strcmp(configName, "PORT")) {
			sscanf(configValue, "%d", &port);
		}
		// địa chỉ của server
		if (!strcmp(configName, "ADDRESS")) {
			addr = configValue;
		}
		// đường dẫn tới thư mục lưu trữ file của client
		if (!strcmp(configName, "CLIENT_LOCATE")) {
			clientRepoPath = configValue;
		}
	}
}
SOCKET createSocket(char* addr, int port) {
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = inet_addr(addr);
	saddr.sin_port = htons(port);
	connect(s, (sockaddr*)&saddr, sizeof(saddr));
	return s;
}
void monitorDirectory(Directory* d)
{
	WaitForSingleObject(ValidatedEvent, INFINITE);
	d->settingFile();
	cout << "start follow" <<d->getPath()<< endl;
	while (1) {
		m.lock();
		d->traceDirectory();// kiểm tra trạng thái thư mục
		d->traceFile();// kiểm tra trạng thái các file
		m.unlock();
		Sleep(50);
	}
}
int main() {
	// đọc dữ liệu cấu hình
	readConfig();
	// Khởi tạo socket
	CreateVaildateEvent();
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	HANDLE hMutex = CreateMutexA(NULL, false, NULL);
	SOCKET s = createSocket((char*)addr.c_str(), port);
	// khởi tạo thư mục cần giám sát
	Directory d(clientRepoPath.c_str());
	// tạo thread để giám sát thư mục
	thread monitorThread(monitorDirectory, &d);
	int id = -1;
	// tạo thread để gửi các thay đổi tới server
	thread getChangeThread(ReportChange(), &d,s, &hMutex, &id);
	ClientHandleRequest hRequest(s, clientRepoPath.c_str());
	TLVBuffer TLVBuff;
	char buff[1024]{ 0 };
	// Nhận các request của server
	while (1) {
		// Nhận các request
		int byteRecv = recv(s, buff, sizeof(buff), 0);
		ResetEvent(HandleRequest);
		if (byteRecv < 0) {
			break;
		}
		TLVBuff.addData(buff, byteRecv);
		TLVPackage p = TLVBuff.getPackage();
		hRequest.setId(p.getId());
		id = p.getId();
		while (p.getTitle() != -1) {
			// Xác thực thành công
			if (p.getTitle() == 200) {
				SetEvent(ValidatedEvent);
				hRequest.handleResponse(p.getValue());
			}
			// gói tin thông báo
			if (p.getTitle() == 100) {
				hRequest.handleResponse(p.getValue());
			}
			// gói tin nghiệp vụ
			else if (p.getTitle() == 101) {
				hRequest.handleResponse2(p.getValue());
			}
			// gói tin dữ liệu
			else {
				hRequest.handleData(p.getValue(), p.getLength() - 8);
				if (p.getTitle() == 0) {
					hRequest.closeFile();
				}
			}
			p = TLVBuff.getPackage();
		}
		SetEvent(HandleRequest);
	}
	monitorThread.join();
	getChangeThread.join();
	return 0;
}
