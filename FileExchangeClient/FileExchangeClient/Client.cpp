#include "Client.h"
#include<iostream>
#include<fstream>
#include "TLVPackage.h"
using namespace std;
Client::Client( SOCKET c)
{
	s = c;
}

void Client::getFile(const char* fileName)
{
	//char buffer[1024];
	//memset(buffer, 0, sizeof(buffer));
	//recv(s, buffer, sizeof(buffer), 0);
	//TLVPackage pk(buffer);
	//if (pk.getTitle() == 200) {
	//	// Thực hiện nhận file từ server;
	//	cout << "start receiv file" << endl;
	//	char path[2048];
	//	memset(path, 0, sizeof(path));
	//	//sprintf_s(path, "%s\\%s", cData->getFilePath(), fileName);
	//	fstream file;
	//	file.open(path, ios::out | ios::binary);
	//	while (true) {
	//		memset(buffer, 0, sizeof(buffer));
	//		int byteRecv = recv(s, buffer, sizeof(buffer), 0) - 8;
	//		buff.addData(buffer, byteRecv + 8);
	//		TLVPackage pk = buff.getPackage();
	//		while (pk.getTitle() != -1) {
	//			if (file.is_open() && byteRecv > 0) {
	//				file.write(pk.getValue(), byteRecv);
	//			}
	//			else {
	//				cout << "file isnot opened" << endl;
	//			}
	//			// Tag = 0 kết thúc file
	//			if (pk.getTitle() == 201) {
	//				break;
	//			}
	//			pk = buff.getPackage();
	//		}
	//		// Nếu tag = 201 thì kết thúc việc nhận file
	//		if (pk.getTitle() == 201) {
	//			break;
	//		}
	//	}
	//	file.flush();
	//	file.close();
	//}
	//else {
	//	// Thông báo file chưa có trên hệ thống
	//	cout << "File \"" << fileName << "\" is'nt exist on server\n";
	//}
}

void Client::sendFile(const char* fileName)
{
	fstream f;
	// mở file
	f.open(fileName, ios::in | ios::binary);
	// Chuyển con trỏ file tới cuối file
	f.seekg(0, ios::end);
	// lấy độ dài của file
	int flen = f.tellg();
	// chuyển con trỏ file về đầu
	f.seekg(0, ios::beg);
	// biến đếm số byte đọc được
	int sent = 0;
	// Biến lưu dữ liệu đọc được
	char data[1016];
	// Đọc lần lượt file
	char buffer[1024];
	while (sent < flen) {
		memset(data, 0, sizeof(data));
		// Số byte đọc từ file
		int byteRead = min(1016, flen - sent);
		// Số byte gửi
		int byteSend = byteRead + 8;
		f.read(data, byteRead);
		TLVPackage pakage(1, byteSend, data);
		sent += byteRead;
		if (sent == flen) {
			// Nếu đã đọc hết file => gửi gói tin với cờ 201;
			pakage.setTitle(0);
		}
		memset(pakage.packageValue(), 0, sizeof(buffer));
		memcpy(buffer, pakage.packageValue(), byteSend);
		send(s, buffer, byteSend, 0);
	}
	f.close();
}

