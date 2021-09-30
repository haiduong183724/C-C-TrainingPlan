﻿#include "HandleClientRequest.h"
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
}

Client HandleClientRequest::getClient(int clientId)
{
	WaitForSingleObject(hmutex, INFINITE);
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].getId() == clientId) {
			ReleaseMutex(hmutex);
			return clients[i];
		}
	}
}

void HandleClientRequest::HandlePacket(TLVPackage p)
{
	state = true;
	if (p.getTitle() == CONTROL_MESSAGE) {// title = 100 => gói tin nghiệp vụ
		HandleRequest(p.getValue(), p.getId());
	}
	else if(p.getTitle() == DATA_STREAM || p.getTitle() == DATA_STREAM_END) {// title = 0 hoặc 1 => gói tin chứa dữ liệu
		HandleData(p, p.getId());
	}
}

void HandleClientRequest::HandleRequest(char* rq, int from)
{
	char request[1024]{ 0 }, ctn[1024]{ 0 };
	// tách request để lấy được yêu vầu
	sscanf(rq, "%s%s", request, ctn);
	char log[1024]{ 0 };
	DateTime d = DateTime::Now();
	sprintf(log, "%d-%d-%d-%d-%d-%d: %s\n",
		d.year, d.month, d.day, d.hour, d.minute, d.second, rq);
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
	case 'C':// yêu cầu tiếp tục gửi dữ liệu lên
	{
		Continue(from);
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
			string filePath = clients[i].status.first;
			int position = clients[i].status.second;
			if (p.getTitle() == DATA_STREAM_END) {
				//client đã truyền xong
				clients[i].changeStatus("", Client::FREE);
			}
			else {
				//Client chưa truyền xong
				clients[i].changeStatus(filePath, position + p.getLength() - 8);

			}
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
		}
	}
	state = false;
}

void HandleClientRequest::Add(char* fileName, int from)
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
			clients[i].changeStatus(filePath, 0);
		}
	}
}

void HandleClientRequest::Rename(char* filePath, int from)
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
		}
	}
	state = false;
}

void HandleClientRequest::Edit(char* fileName, int from)
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
			clients[i].changeStatus(filePath, 0);
		}
	}
	
	
}

void HandleClientRequest::Continue(int id)
{
	// lấy trạng thái logs của client
	pair<string, int> log = getClientLog(id);
	char filePath[1024]{ 0 };
	sprintf(filePath, "%s\\%s", repoPath, log.first);
	file.open(filePath, ios::out | ios::app);
	file.seekg(log.second);
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i].getId() != id) {
			char request[1024]{ 0 };
			// gửi thông báo cho các client khác mở file lên và ghi tiếp vào cuối file
			sprintf(request, "%s %s %d", "ADD", log.first, log.second);
			TLVPackage p(CONTROL_MESSAGE, clients[i].getId(), strlen(request) + 8, request);
			send(clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
		else {
			// yêu cầu client from gửi tiếp nội dung file từ vị trí log.second lên 
			char request[1024]{ 0 };
			sprintf(request, "%s %s %d", "GET", log.first, log.second);
			TLVPackage p(CONTROL_MESSAGE, id, strlen(request) + 8, request);
			send(clients[i].socket, p.packageValue(), p.getLength(), 0);
			clients[i].changeStatus(log.first, log.second);
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


