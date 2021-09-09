#include<iostream>
#include<WinSock2.h>
#include<fstream>
#include<io.h>
#include"ClientHandleRequest.h"
using namespace std;
string addr, clientRepoPath;
int port = 0;

bool checkFileExist(char* fileName) {
	char path[1024];
	memset(path, 0, sizeof(path));
	sprintf(path, "%s\\%s", clientRepoPath.c_str(), fileName);
	return (_access(path, 0));
}
void makeRequest(SOCKET clientS) {
	char request[1024];
	while(1){
		cout << "please enter your request" << endl;
		memset(request, 0, sizeof(request));
		cin.getline(request, sizeof(request));
		// kiểm tra request
		if (request[0] == 'C'|| request[0] == 'D' || request[0] == 'G' 
			|| request[0] == 'c'|| request[0] == 'd' || request[0] == 'g') {

			TLVPackage p(101, strlen(request) + 8, request);
			send(clientS, p.packageValue(), p.getLength(), 0);
			break;
		}
		// Nếu là gửi file, kiểm tra file có tồn tại hay không
		else if (request[0] == 'S' || request[0] == 's') {
			char rq[1024]{ 0 }, ctn[1024]{ 0 };
			sscanf(request, "%s%s", rq, ctn);
			if (checkFileExist(ctn)) {
				cout << "file is'nt exist" << endl;
			}
			else {
				TLVPackage p(101, strlen(request) + 8, request);
				send(clientS, p.packageValue(), p.getLength(), 0);
				break;
			}
		}
		else {
			cout << "your request is'nt valid" << endl;
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
SOCKET createSocket(char* addr, int port ) {
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = inet_addr(addr);
	saddr.sin_port = htons(port);
	connect(s, (sockaddr*)&saddr, sizeof(saddr));
	return s;
}
int main() {
	readConfig();
	// Khởi tạo socket
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	SOCKET s = createSocket((char*)addr.c_str(), port);
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	TLVBuffer clienBuffer;
	// Khởi tạo CLient để trao đổi dữ liệu
	ClientHandleRequest hHandleRequest(s, clientRepoPath.c_str());
	while (1) {
		int byteRecv = recv(s, buffer, sizeof(buffer), 0);
		if (byteRecv == -1) break;
		clienBuffer.addData(buffer, byteRecv);
		TLVPackage p = clienBuffer.getPackage();
		while (p.getTitle() != -1) {
			// xử lý các yêu cầu
			if (p.getTitle() == 100) {
				hHandleRequest.handleResponse(p.getValue());
				makeRequest(s);
			}
			if (p.getTitle() == 101) {
				hHandleRequest.handleResponse2(p.getValue());
			}
			else {
				hHandleRequest.handleData(p.getValue(), p.getLength() - 8);
				if (p.getTitle() == 0) {
					hHandleRequest.closeFile();
				}
			}
			p = clienBuffer.getPackage();
		}
	}	
	WSACleanup();
}
