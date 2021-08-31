#include "ClientHandleRequest.h"
#include<iostream>
using namespace std;

ClientHandleRequest::ClientHandleRequest(SOCKET clientS, const char* Path)
{
	memset(path, 0, sizeof(path));
	s = clientS;
	strcat(path, Path);
	c = new FileHandle(s);
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
		case 'G':
		{
			sendFile(cnt);
		}
		case 'S':
		{
			openFile(cnt);
		}
	}

}

void ClientHandleRequest::handleData(char* data, int dataLen)
{
	if (f.is_open()) {
		f.write(data, dataLen);
	}
}

void ClientHandleRequest::openFile(char* fileName)
{
	char filePath[1024];
	memset(filePath, 0, sizeof(filePath));
	sprintf(filePath, "%s\\%s", path, fileName);
	f.open(filePath, ios::out|ios::binary);
	if (!f.is_open()) {
		cout << "file is'nt exist!" << endl;
	}
}

void ClientHandleRequest::closeFile()
{
	if (f.is_open()) {
		f.close();
	}
}

void ClientHandleRequest::sendFile(char* fileName)
{
	char filePath[1024];
	memset(filePath, 0, sizeof(filePath));
	sprintf(filePath, "%s\\%s", path, fileName);
	c->sendFile(filePath);
}
