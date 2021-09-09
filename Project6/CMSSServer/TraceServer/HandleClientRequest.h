#pragma once
#include <WinSock2.h>
#include<vector>
#include "TLVBuffer.h"
#include"Client.h"
#ifndef HANDLECLIENTREQUEST_H
#define HANDLECLIENTREQUEST_H
#define MAX_CLIENT 1024

class HandleClientRequest
{
private:
	std::vector<Client> clients;
	HANDLE hmutex;
	// trạng thái của đối tượng
	
public:
	bool state = false;
	HandleClientRequest();
	TLVBuffer rqBuffer;
	int numClient = 0;
	void addClient(Client c);
	void removeClient(int clientId);
	Client getClient(int clientId);
	void HandlePacket(TLVPackage p);
	void HandleRequest(char* rq, int from);
	void HandleData(TLVPackage p, int from);
	void Delete(char* filePath, int from);
	void Add(char* filePath, int from);
	void Rename(char* filePath, int from);
	void Edit(char* filePath, int from);
};

#endif // !HANDLECLIENTREQUEST_H


