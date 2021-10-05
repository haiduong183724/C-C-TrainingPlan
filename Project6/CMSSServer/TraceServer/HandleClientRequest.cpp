#include "HandleClientRequest.h"
#include <iostream>
#include<fstream>
#include"DateTime.h"
HandleClientRequest::HandleClientRequest()
{
	hmutex = CreateMutexA(NULL, false, NULL);
}

void HandleClientRequest::addClient(Client c)
{
	clients.push_back(c);
}

void HandleClientRequest::removeClient(int clientId)
{
	WaitForSingleObject(hmutex, INFINITE);
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].getId() == clientId) {
			clients.erase(clients.begin() + i);
			ReleaseMutex(hmutex);
			break;
		}
	}
	ReleaseMutex(hmutex);
}

Client HandleClientRequest::getClient(int clientId)
{
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].getId() == clientId) {
			return clients[i];
		}
	}
}

void HandleClientRequest::HandlePacket(TLVPackage p)
{
	if (p.getTitle() == CONTROL_MESSAGE) {// title = 100 => gói tin nghiệp vụ
		HandleRequest(p.getValue(), p.getId());
	}
	else if(p.getTitle() == DATA_STREAM || p.getTitle() == DATA_STREAM_END) {// title = 0 hoặc 1 => gói tin chứa dữ liệu
		HandleData(p, p.getId());
	}
}

void HandleClientRequest::HandleRequest(char* rq, int from)
{
	state = true;
	char request[1024]{ 0 }, ctn[1024]{ 0 };
	// tách request để lấy được yêu vầu
	sscanf(rq, "%s%s", request, ctn);
	char log[1024]{ 0 };
	DateTime d = DateTime::Now();
	sprintf(log, "%s : %s\n",
		d.dateStr(), rq);
	fstream f;
	f.open((char*)"log.txt", ios::out | ios::app);
	// ghi lại các chỉnh sửa để theo dõi
	f.write(log, strlen(log));
	switch (request[0])
	{
	case 'D':// Xóa file
	{
		Delete(ctn, from);
		break;
	}
	case 'E':// Chỉnh sửa nội dung của file
	{
		Edit(ctn, from);
		break;
	}
	case 'A':// Thêm một file
	{
		cout << 1 << endl;
		Add(ctn, from);
		break;
	}
	case 'R':// đổi tên file
	{
		Rename(rq + strlen(request) + 1, from);
		break;
	}
	case 'S':// yêu cầu đồng bộ với server
	{
		Synchronize(from);
	}
	default:
		break;
	}
}

void HandleClientRequest::HandleData(TLVPackage p, int from)
{
	// chuyển tiếp dữ liệu này cho các client khác 
	int pLen = p.getLength() - 8;
	file.write(p.getValue(), pLen);
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			p.setId(Clients[i].getId());
			char dataSend[1024]{ 0 };
			memcpy(dataSend, p.packageValue(), p.getLength());
			send(Clients[i].socket, dataSend, p.getLength(), 0);
		}
		else{
			string filePath = Clients[i].status.first;
			int position = Clients[i].status.second;
			if (p.getTitle() == DATA_STREAM_END) {
				//client đã truyền xong
				Clients[i].changeStatus("", Client::FREE);
			}
			else {
				//Client chưa truyền xong
				Clients[i].changeStatus(filePath, position + p.getLength() - 8);
			}
		}
		if (p.getTitle() == DATA_STREAM_END) {
			TLVPackage p(NO_CONTENT_PACKET, from, 8, (char*)"");
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
	}
	if (p.getTitle() == 0) {
		closeFile();
	}
}

void HandleClientRequest::Delete(char* fileName, int from)
{
	char filePath[1024]{ 0 };
	sprintf(filePath, "%s\\%s", repoPath, fileName);
	remove(filePath);
	// gửi yêu cầu xóa file đến cho các client khác
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "DELETE", fileName);
			TLVPackage p(CONTROL_MESSAGE, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
			TLVPackage p1(NO_CONTENT_PACKET, Clients[i].getId(), 8, (char*)"");
			send(Clients[i].socket, p1.packageValue(), p1.getLength(), 0);
		}
	}
	state = false;
}

void HandleClientRequest::Add(const char* fileName, int from)
{
	// thông báo các client khác mở file
	vector<Client> Clients = clients;
	char filePath[1024]{ 0 };
	sprintf(filePath, "%s\\%s", repoPath, fileName);
	file.open(filePath, ios::out|ios::binary);
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s %d", "ADD", fileName, 0);
			TLVPackage p(CONTROL_MESSAGE, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
		else {
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "GET", fileName);
			TLVPackage p(CONTROL_MESSAGE, from, strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
			clients[i].changeStatus(fileName, 0);
			TLVPackage p1(NO_CONTENT_PACKET, Clients[i].getId(), 8, (char*)"");
			send(Clients[i].socket, p1.packageValue(), p1.getLength(), 0);
		}
	}
}

void HandleClientRequest::Rename(const char* filePath, int from)
{
	// tách lấy nội dung tên file cũ và tên file đổi
	vector<Client> Clients = clients;
	char oldName[1024]{ 0 }, newName[1024]{ 0 };
	sscanf(filePath, "%s %s", oldName, newName);
	// gửi yêu cầu đổi tên cho các client khác
	char oldPath[1024]{ 0 }, newPath[1024]{ 0 };
	sprintf(oldPath, "%s\\%s", repoPath, oldName);
	sprintf(newPath, "%s\\%s", repoPath, newName);
	rename(oldPath, newPath);
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s %s", "RENAME", oldName, newName);
			TLVPackage p(CONTROL_MESSAGE, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
			TLVPackage p1(NO_CONTENT_PACKET, from, 8, (char*)"");
			send(clients[i].socket, p1.packageValue(), p1.getLength(), 0);
		}
	}
	state = false;
}

void HandleClientRequest::Edit(const char* fileName, int from)
{
	// thông báo cho các client khác mở file để nhận dữ liệu
	char filePath[1024]{ 0 };
	sprintf(filePath, "%s\\%s", repoPath, fileName);
	file.open(filePath, ios::out| ios::binary);
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s %d", "ADD", fileName, 0);
			TLVPackage p(CONTROL_MESSAGE, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
		else {
			// yêu cầu client from gửi nội dung file lên 
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "GET", fileName);
			TLVPackage p(CONTROL_MESSAGE, from, strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
			clients[i].changeStatus(fileName, 0);
			TLVPackage p1(NO_CONTENT_PACKET, from, 8, (char*)"");
			send(clients[i].socket, p1.packageValue(), p1.getLength(), 0);
		}
	}
	
	
}

pair<string, int> HandleClientRequest::getClientLog(int clientId)
{
	Client client = getClient(clientId);
	string clientIpAddr = inet_ntoa(client.caddr.sin_addr);
	for (int i = logs.size() - 1; i >= 0; i--) {
		char ipAddress[100]{ 0 }, fileName[1024]{ 0 }, position[1024]{ 0 };
		sscanf(logs[i].c_str(), "%s%s%s", ipAddress, fileName, position);
		// Nếu thấy log của client
		if (strcmp(clientIpAddr.c_str(), ipAddress) == 0) {
			logs.erase(logs.begin() + i);
			string filePath = fileName;
			return make_pair(filePath, atoi(position));
		}
	}
	return make_pair("no file match", -1);
}

void HandleClientRequest::setPath(char* path)
{
	memset(repoPath, 0, sizeof(repoPath));
	strcat(repoPath, path);
}

void HandleClientRequest::closeFile()
{
	file.close();
	state = false;
}

void HandleClientRequest::Synchronize(int clientId)
{
	pair<string, int> log = getClientLog(clientId);
	SOCKET s = getClient(clientId).socket;
	if (log.second != -1) {
		Add(log.first.c_str(), clientId);
	}
	else {
		TLVPackage pk(CONTROL_MESSAGE, clientId, 9, (char*)"C");
		send(s, pk.packageValue(), pk.getLength(), 0);
		WIN32_FIND_DATAA FindData;
		HANDLE hFile = FindFirstFileA("Repo\\*.*", &FindData);
		if (hFile != INVALID_HANDLE_VALUE) {
			int num = 0;
			// duyệt lần lượt các file trong thư mục
			while (FindNextFileA(hFile, &FindData)) {
				char filePath[1024]{ 0 };
				if (num > 0) {// bỏ qua file đầu tiên vì nó không có ý nghĩa
					sprintf(filePath, "%s\\%s", "Repo", FindData.cFileName);
					char request[1024]{ 0 };
					sprintf(request, "%s %s %d", "ADD", FindData.cFileName, 0);
					TLVPackage p(CONTROL_MESSAGE, clientId, strlen(request) + 8, request);
					char note[1024]{ 0 };
					sprintf(note, "Synchronizing file %s ", FindData.cFileName);
					TLVPackage p2(NOTIFY_MESSAGE, clientId, strlen(note) + 8, note);
					send(s, p2.packageValue(), p2.getLength(), 0);
					send(s, p.packageValue(), p.getLength(), 0);
					SendFile(FindData.cFileName, clientId);
				}
				num++;
			}
			TLVPackage p(NO_CONTENT_PACKET, clientId, 8, (char*)"");
			send(s, p.packageValue(), p.getLength(), 0);
		}
	}
	state = false;
}

void HandleClientRequest::SendFile(char* fileName, int ClientId)
{
	SOCKET s = getClient(ClientId).socket;
	fstream f;
	// mở file
	char filePath[1024];
	memset(filePath, 0, sizeof(filePath));
	sprintf(filePath, "%s\\%s", "Repo", fileName);
	f.open(filePath, ios::in | ios::binary);
	// Chuyển con trỏ file tới cuối file
	if (!f.is_open()) {
		if (!f.is_open()) {
			TLVPackage pakage(DATA_STREAM_END, ClientId, 8, (char*)"");
			send(s, pakage.packageValue(), 8, 0);
			cout << "cant open file" << endl;
		}
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
		TLVPackage pakage(DATA_STREAM, ClientId, byteSend, data);
		sent += byteRead;
		if (sent >= flen) {
			// Nếu đã đọc hết file => gửi gói tin với cờ 201;
			pakage.setTitle(DATA_STREAM_END);
		}
		memset(buffer, 0, sizeof(buffer));
		memcpy(buffer, pakage.packageValue(), byteSend);
		
		send(s, buffer, byteSend, 0);
	}
	if (flen == 0) {
		TLVPackage pakage(DATA_STREAM_END, ClientId, 8, (char*)"");
		send(s, pakage.packageValue(), 8, 0);
	}
	f.close();

}


