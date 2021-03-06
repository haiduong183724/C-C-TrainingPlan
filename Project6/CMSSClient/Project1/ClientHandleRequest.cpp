#include "ClientHandleRequest.h"
#include<iostream>
using namespace std;

ClientHandleRequest::ClientHandleRequest()
{
	isConnect = false;
}

ClientHandleRequest::ClientHandleRequest(SOCKET clientS, const char* Path)
{
	memset(path, 0, sizeof(path));
	s = clientS;
	strcat(path, Path);
	d = new Directory(path);
	isConnect = false;
}

void ClientHandleRequest::handleResponse(char* response)
{
	cout << response << endl;
}

void ClientHandleRequest::handleResponse2(char* response)
{
	char rq[1024],// Request
		cnt[1024];// Nội dung request
	memset(rq, 0, sizeof(rq));
	memset(cnt, 0, sizeof(cnt));
	sscanf(response, "%s%s", rq, cnt);
	switch (rq[0]) {
		// xác thực đăng nhập
		case 'V':
		{
			login();
			break;
		}
		case 'G':
		{
			char filePath[1024]{ 0 }, position[1024]{};
			sscanf(response + strlen(rq) + 1, "%s%s", filePath, position);
			sendFile(cnt,atoi(position));
			break;
		}
		case 'A':
		{
			char filePath[1024]{ 0 }, position[1024]{};
			sscanf(response + strlen(rq) + 1, "%s%s", filePath, position);
			openFile(cnt, atoi(position));
			break;
		}
		case 'D':
		{
			deleteFile(cnt);
			break;
		}
		case 'R':
		{
			char oldName[1024]{ 0 }, newName[1024]{};
			sscanf(response + strlen(rq) + 1,"%s%s", oldName, newName);
			renameFile(oldName, newName);
			break;
		}
		case 'C':
		{
			d->clear();
			break;
		}
	}
}

void ClientHandleRequest::handleData(char* data, int dataLen)
{
	if (f.is_open()) {
		f.write(data, dataLen);
	}
}
void ClientHandleRequest::login() {
	char username[1024]{ 0 }, password[1024]{ 0 }, request[1024]{ 0 };
	cout << "Enter username: ";
	cin >> username;
	cout << " Enter password: ";
	cin >> password;
	// gửi tới server
	sprintf(request, "%s %s", username, password);
	TLVPackage p(CONTROL_MESSAGE, 1, strlen(request) + 8, request);
	send(s, p.packageValue(), p.getLength(), 0);
}
void ClientHandleRequest::openFile(char* fileName, int filePositon)
{
	// mở file lên để ghi
	char filePath[1024];
	memset(filePath, 0, sizeof(filePath));
	sprintf(filePath, "%s\\%s", path, fileName);
	// nếu vị trí mở file > 0 => mở file ở chế độ ghi tiếp
	if (filePositon > 0) {
		f.open(filePath, ios::out | ios::binary | ios::app);
		f.seekg(filePositon);
	}
	else {// tạo file mới
		f.open(filePath, ios::out | ios::binary);
	}
	FileInfomation newFile(filePath);
	d->addFile(newFile);
	if (!f.is_open()) {
		cout << "can'nt open file!" << endl;
	}
}

void ClientHandleRequest::closeFile()
{
	d->listFile = d->settingFile();
	// đóng file lại
	if (f.is_open()) {
		f.close();
	}
}

void ClientHandleRequest::sendFile(char* fileName, int position)
{
	fstream f;
	// mở file
	char filePath[1024];
	memset(filePath, 0, sizeof(filePath));
	sprintf(filePath, "%s\\%s", path, fileName);
	cout << "sending file " << fileName << endl;
	f.open(filePath, ios::in | ios::binary);
	// Chuyển con trỏ file tới cuối file
	if (!f.is_open()) {
		TLVPackage pakage(DATA_STREAM_END, id, 8, (char*)"");
		send(s, pakage.packageValue(), 8, 0);
		cout << "cant open file" << endl;
	}
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
		TLVPackage pakage(DATA_STREAM, id, byteSend, data);
		sent += byteRead;
		if (sent >= flen) {
			// Nếu đã đọc hết file => gửi gói tin với cờ 201;
			pakage.setTitle(DATA_STREAM_END);
		}
		memset(pakage.packageValue(), 0, sizeof(buffer));
		memcpy(buffer, pakage.packageValue(), byteSend);
		int byteSent = send(s, buffer, byteSend, 0);
		if (byteSent < 0) {
			isConnect = false;
			FileInfomation nFile(filePath);
			closeFile();
			d->logFile(nFile, FILE_ADD);
			break;
		}
		while (byteSent < byteSend) {
			int sent = send(s, buffer + byteSend, byteSend - byteSent, 0);
			byteSent += sent;
		}
	}
	if (flen == 0) {
		TLVPackage pakage(DATA_STREAM_END, id, 8, (char*)"");
		send(s, pakage.packageValue(), 8, 0);
	}
	f.close();
}

void ClientHandleRequest::deleteFile(char* fileName)
{
	char fullPath[1024]{0};
	sprintf(fullPath, "%s\\%s", path, fileName);
	remove(fullPath);
}

void ClientHandleRequest::renameFile(char* oldName, char* newName)
{
	char oldFullPath [1024] { 0 };
	char newFullPath[1024]{ 0 };
	sprintf(oldFullPath, "%s\\%s", path, oldName);
	sprintf(newFullPath, "%s\\%s", path, newName);
	int ret = rename(oldFullPath, newFullPath);
}

void ClientHandleRequest::setId(int Id)
{
	id = Id;
}

int ClientHandleRequest::getId()
{
	return id;
}

void ClientHandleRequest::setSocket(SOCKET clientS)
{
	s = clientS;
}

void ClientHandleRequest::setPath(const char* Path, const char* tableName)
{
	strcat(path, Path);
	d = new Directory(Path);
	d->Connector = new DBConnector((char*)tableName);
}

SOCKET ClientHandleRequest::getSocket()
{
	return s;
}
