#include "FileHandle.h"
#include<iostream>
#include <WinSock2.h>
#include<fstream>
#include "TLVPackage.h"
using namespace std;

FileHandle::FileHandle( SOCKET c)
{
	s = c;
}

void FileHandle::sendFile(const char* fileName)
{
	fstream f;
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
		char dataSend[1024];
		memset(dataSend, 0, sizeof(dataSend));
		memcpy(dataSend, pakage.packageValue(), byteSend);
		send(s, dataSend, byteSend, 0);
		/*TLVPackage p(dataSend);
		file.write(p.getValue(), byteRead);*/
	}
	cout << "Sending ok" << endl;
}


void FileHandle::getFile(const char* fileName)
{
	// gửi yêu cầu nhận file, nội dung file
	TLVPackage p(200, 11, (char*)"OK");
	send(s, p.packageValue(), p.getLength(), 0);
	fstream file;
	file.open(fileName, ios::out | ios::binary);
	int fileReceiveLength = 0;
	char buffer[1024];
	cout << "Start receive file from client";
		while (true) {
			memset(buffer, 0, sizeof(buffer));
			int byteRecv = recv(s, buffer, sizeof(buffer), 0) - 8;
			buff.addData(buffer, byteRecv + 8);
			TLVPackage pk = buff.getPackage();
			while (pk.getTitle() != -1) {
				if (file.is_open() && byteRecv > 0) {
					fileReceiveLength += byteRecv;
					/*cout << "receiv: " << byteRecv - 8;*/
					file.write(pk.getValue(), byteRecv);
				}
				else {
					cout << "file isnot opened" << endl;
				}
				// Tag = 201 kết thúc file
				if (pk.getTitle() == 201) {
					break;
				}
				pk = buff.getPackage();
			}
			if (pk.getTitle() == 201) {
				break;
			}
	}
	cout << "fileReceiveLength: " << fileReceiveLength << endl;
	file.flush();
	file.close();
}
//int main() {
//	FileHandle sAPI("C:\\Users\\Admin\\source\\repos\\FileExchangeServer\\FileExchangeServer");
//	ResultMsg msg = sAPI.readFile("input.txt");
//	ResultMsg msg2 = sAPI.writeFile(msg.getContent().c_str(), "hello2.txt");
//	char ch = cin.get();
//	cout << ch;
//}