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
TLVBuffer TLVBuff;
char buff[1024]{ 0 };

HANDLE ValidatedEvent;
void CreateVaildateEvent() {
	ValidatedEvent = CreateEventA(NULL, true, false, NULL);
	if (ValidatedEvent == INVALID_HANDLE_VALUE) {
		cout << "false to create event" << endl;
	}
}
void ConnectToServer(SOCKET* s, SOCKADDR_IN* caddr, int* id) {
	int timeConnect = 0;
	while (1) {
		if (!SocketSendData::isConnect) {
			int ret = connect(*s, (sockaddr*)caddr, sizeof(*caddr));
			if (ret == 0) {
				ClientHandleRequest hRequest(*s, "");
				// xác minh đăng nhập
				hRequest.login();
				int byteRecv = recv(*s, buff, sizeof(buff), 0);
				if (byteRecv > 0) {
					TLVBuff.addData(buff, byteRecv);
					TLVPackage p = TLVBuff.getPackage();
					if (p.getTitle() == LOGIN_SUCESS) {
						hRequest.handleResponse(p.getValue());
						*id = p.getId();
						SocketSendData::isConnect = true;
						timeConnect++;
						if (timeConnect > 1) {
							// gửi yêu cầu đồng bộ dữ liệu với server
						}
						break;
					}
					else {
						hRequest.handleResponse(p.getValue());
					}
				}
			}
			
		}
	}
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
void monitorDirectory(Directory* d)
{
	WaitForSingleObject(ValidatedEvent, INFINITE);
	cout << "start follow" <<d->getPath()<< endl;
	while (1) {
		m.lock();
		d->traceFile();// kiểm tra trạng thái các file
		d->traceDirectory();// kiểm tra trạng thái thư mục
		m.unlock();
		Sleep(1000);
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
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = inet_addr(addr.c_str());
	saddr.sin_port = htons(port);
	int id = -1;
	// tạo thread để giám sát thư mục
	ClientHandleRequest hRequest(s, clientRepoPath.c_str());
	thread monitorThread(monitorDirectory, hRequest.d);
	thread conectServerThread(ConnectToServer, &s, &saddr, &id);
	hRequest.d->clear();
	// tạo thread để gửi các thay đổi tới server
	thread getChangeThread(ReportChange(), hRequest.d ,s, &hMutex, &id);
	// Gửi tài khoảng, mật khẩu tới cho client
	// Nhận các request của server
	while (1) {
		if (SocketSendData::isConnect) {
			int byteRecv = recv(s, buff, sizeof(buff), 0);
			if (byteRecv < 0) {
				break;
			}
			TLVBuff.addData(buff, byteRecv);
			TLVPackage p = TLVBuff.getPackage();
			hRequest.setId(p.getId());
			while (p.getTitle() != INVALID_MESSAGE) {
				// gói tin thông báo
				if (p.getTitle() == NOTIFY_MESSAGE) {
					hRequest.handleResponse(p.getValue());
				}
				// gói tin nghiệp vụ
				else if (p.getTitle() == CONTROL_MESSAGE) {
					hRequest.handleResponse2(p.getValue());
				}
				else if (p.getTitle() == INVALID_MESSAGE) {
					SetEvent(ValidatedEvent);
				}
				// gói tin dữ liệu
				else {
					hRequest.handleData(p.getValue(), p.getLength() - 8);
					if (p.getTitle() == DATA_STREAM_END) {
						hRequest.closeFile();
					}
				}
				p = TLVBuff.getPackage();
			}
		}
	}
	monitorThread.join();
	getChangeThread.join();
	return 0;
}
