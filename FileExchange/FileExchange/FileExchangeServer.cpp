#include<iostream>
#include<WinSock2.h>
#include<fstream>
#include<thread>
#include"HandleClientRequest.h"
string serverPath;
int port = 0;
using namespace std;
void ClientThread(SOCKET clientS, SOCKADDR_IN caddr)
{
	// thông báo kết nối thành công
	cout << "Client" << inet_ntoa(caddr.sin_addr) << " connect!" << endl;
	TLVPackage p(100, 40, (char*)"Welcome to file exchange server");
	// Khởi tạo đối tượng xử lý các yêu cầu
	HandleClientRequest hHandleRq(clientS, serverPath.c_str());
	// Nhận các yêu cầu của client
	// gửi danh sách các file trong thư mục cho client
	hHandleRq.sendCurrentDirectory();
	// Buffer chứa dữ liệu nhận được
	char buffer[1024];
	// nơi chứa các gói tin TLV gửi đến
	TLVBuffer TLVbuff;
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		int byteRecv = recv(clientS, buffer, sizeof(buffer), 0);
		if (byteRecv == -1) break;
		TLVbuff.addData(buffer, byteRecv);
		TLVPackage p = TLVbuff.getPackage();
		while (p.getTitle() != -1) {
			// Xử lý các request
			if(p.getTitle() == 101)
			{
				hHandleRq.handleRequest(p.getValue());
			}
			else
			{
				hHandleRq.handleData(p.getValue(), p.getLength() - 8);
				if (p.getTitle() == 0) {
					hHandleRq.closeFile();
					TLVPackage k(100, 15, (char*)"done!\n");
					send(clientS, k.packageValue(), k.getLength(), 0);
				}
			}
			p = TLVbuff.getPackage();
		}
	}
}
void readConfig()
{
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
		// đọc các config
		// cổng thiết lập
		if (!strcmp(configName, "PORT")) {
			sscanf(configValue, "%d", &port);
		}
		// đường dẫn tới thư mục lưu các file
		if (!strcmp(configName, "SERVER_LOCATE")) {
			serverPath = configValue;
		}
	}
}
SOCKET createSocket(int port)
{
	u_long iMode = 1;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in saddr, daddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	return s;
}
int main() {
	readConfig();
	// Khởi tạo socket
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET s = createSocket(port);
	listen(s, 10);
	while (true) {
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
		if (c != WSAEWOULDBLOCK) {
			new thread(ClientThread, c, caddr);
		}
		else {
			cout << "no connect" << endl;
		}
	}
	WSACleanup();
	return 0;
}