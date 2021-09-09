#include<WinSock2.h>
#include<iostream>
#include"Client.h"
#include "HandleClientRequest.h"
using namespace std;
HandleClientRequest sHandle;
HANDLE gMutex;
void CreateMtx() {
	gMutex = CreateMutexA(NULL, false, NULL);
	if (gMutex == INVALID_HANDLE_VALUE) {
		cout << "Create mutex false" << endl;
	}
}
DWORD WINAPI HandleClientRequests(LPVOID param) {
	bool isSending = false;
	while (true) {
		if (sHandle.numClient > 0) {
			WaitForSingleObject(gMutex, INFINITE);
			TLVPackage p = sHandle.rqBuffer.getPackage();
			ReleaseMutex(gMutex);
			while (p.getTitle() != -1) {
				if (p.getTitle() == 100 && sHandle.state == true) {
					WaitForSingleObject(gMutex, INFINITE);
					sHandle.rqBuffer.addData(p.packageValue(), p.getLength());
					ReleaseMutex(gMutex);
				}
				else {
					sHandle.HandlePacket(p);
				}
				WaitForSingleObject(gMutex, INFINITE);
				p = sHandle.rqBuffer.getPackage();
				ReleaseMutex(gMutex);
			}
		}
	}
}
DWORD WINAPI WaitClientEvent(LPVOID param){
	int id = (int) param;
	bool isClient = false;
	Client client = sHandle.getClient(id);
	TLVPackage p(100, id, 31, (char*)"send account: username password");
	send(client.socket, p.packageValue(), p.getLength(), 0);
	TLVPackage p1(101, id, 17, (char*)"VALIDATE");
	send(client.socket, p1.packageValue(), p1.getLength(), 0);
	while (true) {
		char buff[1024]{ 0 };
		int byteRecv = recv(client.socket, buff, sizeof(buff), 0);
		if (byteRecv > 0) {
			if (isClient) {
				WaitForSingleObject(gMutex, INFINITE);
				sHandle.rqBuffer.addData(buff, byteRecv);
				ReleaseMutex(gMutex);
			}
			else {
				char userName[1024]{ 0 }, password[1024]{ 0 };
				TLVPackage p(buff);
				sscanf(p.getValue(), "%s%s", userName, password);
				isClient = client.checkAccount(userName, password, id);
				if (!isClient) {
					TLVPackage p(100, id, 31 , (char*)"account is'nt correct!");
					send(client.socket, p.packageValue(), p.getLength(), 0);
					TLVPackage p1(101, id, 17, (char*)"VALIDATE");
					send(client.socket, p1.packageValue(), p1.getLength(), 0);
				}
				else {
					TLVPackage p(200, id, 28, (char*)"account is correct!");
					send(client.socket, p.packageValue(), p.getLength(), 0);
				}
			}
		}
		else {
			cout << "A client has disconnect" << endl;;
			break;
		}
	}
	return 0;
}


int main() {
	CreateMtx();
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);
	int numThread = 0;
	CreateThread(NULL, 0, HandleClientRequests, NULL, 0, NULL);
	while (0 == 0) {
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
		DWORD ID;
		Client client(c, sHandle.numClient);
		sHandle.addClient(client);
		CreateThread(NULL, 0, WaitClientEvent, (LPVOID)sHandle.numClient, 0, &ID);
		sHandle.numClient += 1;
	}
}