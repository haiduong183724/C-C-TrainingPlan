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
string addr, clientRepoPath, TableName;
int port = 0;
TLVBuffer TLVBuff;
ClientHandleRequest hRequest;
HANDLE ValidatedEvent;

inline bool exists_test(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}
void checkConnect() {
	while (true) {
		if (hRequest.isConnect) {
			m.lock();
			TLVPackage p(NO_CONTENT_PACKET, hRequest.getId(), 8, (char*)"");
			int ret = send(hRequest.getSocket(),"", 0, 0);
			if (ret < 0) {
				hRequest.isConnect = false;
			}
			m.unlock();
		}
		Sleep(1000);
	}
}

void CreateVaildateEvent() {
	ValidatedEvent = CreateEventA(NULL, true, false, NULL);
	if (ValidatedEvent == INVALID_HANDLE_VALUE) {
		cout << "false to create event" << endl;
	}
}
void ConnectToServer(SOCKADDR_IN caddr) {
	while (1) {
		if (!hRequest.isConnect) {
			SOCKADDR_IN serverAddr = caddr;
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			int ret = connect(s, (sockaddr*)&serverAddr, sizeof(serverAddr));
			if (ret == 0) {
				// xác minh đăng nhập
				hRequest.setSocket(s);
				while (true) {
					hRequest.login();
					char buff[1024]{ 0 };
					int byteRecv = recv(s, buff, sizeof(buff), 0);
					if (byteRecv > 0) {
						TLVBuff.addData(buff, byteRecv);
						TLVPackage p = TLVBuff.getPackage();
						if (p.getTitle() == LOGIN_SUCESS) {
							hRequest.handleResponse(p.getValue());
							hRequest.isConnect = true;
							hRequest.setId(p.getId());
							if (exists_test("list_file.txt")) {
								hRequest.d->readFileListSave();
							}
							break;
						}
						else {
							hRequest.handleResponse(p.getValue());
						}
					}
					else {
						break;
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
		if (!strcmp(configName, "DATA_BASE")) {
			TableName = configValue;
		}
	}
}
void monitorDirectory()
{
	WaitForSingleObject(ValidatedEvent, INFINITE);
	cout << "start follow" <<hRequest.d->getPath()<< endl;
	while (1) {
		WaitForSingleObject(ValidatedEvent, INFINITE);
		m.lock();
		hRequest.d->traceFile();// kiểm tra trạng thái các file
		hRequest.d->traceDirectory();// kiểm tra trạng thái thư mục
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
	// tạo thread để giám sát thư mục
	thread monitorThread(monitorDirectory);
	thread conectServerThread(ConnectToServer, saddr);
	thread checkConnectThread(checkConnect);
	hRequest.setPath(clientRepoPath.c_str(), TableName.c_str());
	// tạo thread để gửi các thay đổi tới server
	thread getChangeThread(ReportChange(), &hRequest, &hMutex);
	// Gửi tài khoảng, mật khẩu tới cho client
	// Nhận các request của server
	while (1) {
		if (hRequest.isConnect) {
			char buff[1024]{ 0 };
			int byteRecv = recv(hRequest.getSocket(), buff, sizeof(buff), 0);
			if (byteRecv > 0) {
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
						ResetEvent(ValidatedEvent);
						hRequest.handleResponse2(p.getValue());
					}
					else if (p.getTitle() == NO_SYNC) {
						SetEvent(ValidatedEvent);
					}
					else if (p.getTitle() == NO_CONTENT_PACKET) {
						hRequest.d->listFile = hRequest.d->settingFile();
						fstream logFile;
						logFile.open("list_file.txt", ios::out);
						char logs[1024]{ 0 };
						strcat(logs, hRequest.d->getLog());
						logFile << logs;
						logFile.close();
						SetEvent(ValidatedEvent);
					}
					 //gói tin dữ liệu
					else {
						hRequest.handleData(p.getValue(), p.getLength() - 8);
						if (p.getTitle() == DATA_STREAM_END) {
							fstream logFile;
							logFile.open("list_file.txt", ios::out);
							char logs[1024]{ 0 };
							strcat(logs, hRequest.d->getLog());
							logFile << logs;
							logFile.close();
							hRequest.closeFile();
						}
					}
					p = TLVBuff.getPackage();
				}
			}
		}
	}
	monitorThread.join();
	getChangeThread.join();
	return 0;
}
