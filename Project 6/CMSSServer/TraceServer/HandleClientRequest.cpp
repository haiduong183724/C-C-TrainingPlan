#include "HandleClientRequest.h"
#include <iostream>


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
	if (p.getTitle() == 100) {
		HandleRequest(p.getValue(), p.getId());
	}
	else {
		HandleData(p, p.getId());
	}
}

void HandleClientRequest::HandleRequest(char* rq, int from)
{
	char request[1024]{ 0 }, ctn[1024]{ 0 };
	sscanf(rq, "%s%s", request, ctn);
	switch (request[0])
	{
	case 'D':
	{
		Delete(ctn, from);
		break;
	}
	case 'E':
	{
		state = true;
		Edit(ctn, from);
		break;
	}
	case 'A':
	{
		state = true;
		cout << 1 << endl;
		Add(ctn, from);
		break;
	}
	case 'R':
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
	vector<Client> Clients = clients;
	char oldName[1024]{ 0 }, newName[1024]{ 0 };
	sscanf(filePath, "%s %s", oldName, newName);
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
	vector<Client> Clients = clients;
	for (int i = 0; i < Clients.size(); i++) {
		if (Clients[i].getId() != from) {
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "ADD", filePath);
			TLVPackage p(101, Clients[i].getId(), strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
		else {
			char request[1024]{ 0 };
			sprintf(request, "%s %s", "GET", filePath);
			TLVPackage p(101, from, strlen(request) + 8, request);
			send(Clients[i].socket, p.packageValue(), p.getLength(), 0);
		}
	}
	
	
}

