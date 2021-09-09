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
	if (p.getTitle() == 100) {// title = 100 => gói tin nghiệp vụ
		HandleRequest(p.getValue(), p.getId());
	}
	else if(p.getTitle() == 0 || p.getTitle() == 1) {// title = 0 hoặc 1 => gói tin chứa dữ liệu
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
	case 'D':// DELETE 
	{
		Delete(ctn, from);
		break;
	}
	case 'E':// EDIT
	{
		state = true;
		Edit(ctn, from);
		break;
	}
	case 'A'://ADD
	{
		state = true;
		cout << 1 << endl;
		Add(ctn, from);
		break;
	}
	case 'R'://RENAME
	{
		Rename(rq+strlen(request) +1, from);
		break;
	}
	default:
		break;
	}
}

void HandleClientRequest::HandleData(TLVPackage p, int from)
{
	// chuyển tiếp dữ liệu này cho các client khác 
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			p.setId(Clients[i].getId());
			char dataSend[1024]{ 0 };
			memcpy(dataSend, p.packageValue(), p.getLength());
			send(Clients[i].socket, dataSend, p.getLength(), 0);
		}
	}
	if (p.getTitle() == 0) {
		state = false;
		cout << 2 << endl;
	}
}

void HandleClientRequest::Delete(char* filePath, int from)
{
	// gửi yêu cầu xóa file đến cho các client khác
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "DELETE", filePath);
			TLVPackage p(101, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
	}
}

void HandleClientRequest::Add(char* filePath, int from)
{
	// thông báo các client khác mở file
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "ADD", filePath);
			TLVPackage p(101, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
	}
	// thông báo client gửi file lên
	Client c = getClient(from);
	char request[1024]{ 0 };
	sprintf(request, "%s %s", "GET", filePath);
	TLVPackage p(101, from, strlen(request)+8, request);
	send(c.socket, p.packageValue(), p.getLength(), 0);
	
}

void HandleClientRequest::Rename(char* filePath, int from)
{
	// tách lấy nội dung tên file cũ và tên file đổi
	vector<Client> Clients = clients;
	char oldName[1024]{ 0 }, newName[1024]{ 0 };
	sscanf(filePath, "%s %s", oldName, newName);
	// gửi yêu cầu đổi tên cho các client khác
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s %s", "RENAME", oldName, newName);
			TLVPackage p(101, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
	}
}

void HandleClientRequest::Edit(char* filePath, int from)
{
	// thông báo cho các client khác mở file để nhận dữ liệu
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "ADD", filePath);
			TLVPackage p(101, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
		else {
			// yêu cầu client from gửi nội dung file lên 
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "GET", filePath);
			TLVPackage p(101, from, strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
	}
	
	
}

