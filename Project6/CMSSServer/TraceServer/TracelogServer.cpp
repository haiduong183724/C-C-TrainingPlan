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
	while (true) {
		// kiểm tra có client nào đang chạy không
		if (sHandle.numClient > 0) {
			WaitForSingleObject(gMutex, INFINITE);
			// kiểm tra buffer có đủ gói tin TLV hay không
			TLVPackage p = sHandle.rqBuffer.getPackage();
			ReleaseMutex(gMutex);
			while (p.getTitle() != -1) {
				// nếu trạng thái của HandleClientRequest là đang bận,
				//và gói tin là gói tin nghiệp vụ, 
				//gói tin này sẽ được đưa xuống cuối buffer để xử lý sau khi hết bận
				if (p.getTitle() == 100 && sHandle.state == true) {
					WaitForSingleObject(gMutex, INFINITE);
					sHandle.rqBuffer.addData(p.packageValue(), p.getLength());
					ReleaseMutex(gMutex);
				}
				else {
					// nếu không thì sẽ sử lý gói tin
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
	int id = (int) param;// id của client
	bool isClient = false;// tình trạng xác thực, true: đã xác thực, false: chưa
	Client client = sHandle.getClient(id);
	// xác minh đăng nhập
	TLVPackage p(100, id, 31, (char*)"send account: username password");
	send(client.socket, p.packageValue(), p.getLength(), 0);
	TLVPackage p1(101, id, 17, (char*)"VALIDATE");
	send(client.socket, p1.packageValue(), p1.getLength(), 0);
	while (true) {
		char buff[1024]{ 0 };
		int byteRecv = recv(client.socket, buff, sizeof(buff), 0);
		if (byteRecv > 0) {
			if (isClient) {// nếu đã xác thực thì đưa gói tin vào TLV buffer để đối tượng HandleClientRequest xử lý
				WaitForSingleObject(gMutex, INFINITE);
				sHandle.rqBuffer.addData(buff, byteRecv);
				ReleaseMutex(gMutex);
			}
			else {
				// nếu không thì gói tin này là gói tin xác thực
				char userName[1024]{ 0 }, password[1024]{ 0 };
				TLVPackage p(buff);
				sscanf(p.getValue(), "%s%s", userName, password);
				// xác thực
				isClient = client.checkAccount(userName, password, id);
				if (!isClient) {// nếu sai, gửi lại y/c xác thực đăng nhập
					TLVPackage p(100, id, 31 , (char*)"account is'nt correct!");
					send(client.socket, p.packageValue(), p.getLength(), 0);
					TLVPackage p1(101, id, 17, (char*)"VALIDATE");
					send(client.socket, p1.packageValue(), p1.getLength(), 0);
				}
				else {// nếu đúng, gửi yêu cầu thành công
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
	// khởi tạo mutex đồng bộ
	CreateMtx();
	WSADATA data;
	// tạo socket lắng nghe kết nối
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	// lắng nghe cổng 8888
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);
	int numThread = 0;
	// tạo thread xử lý các request từ client
	CreateThread(NULL, 0, HandleClientRequests, NULL, 0, NULL);
	while (0 == 0) {
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
		DWORD ID;
		Client client(c, sHandle.numClient);
		sHandle.addClient(client);
		// tạo thread để lắng nghe các request từ client
		CreateThread(NULL, 0, WaitClientEvent, (LPVOID)sHandle.numClient, 0, &ID);
		sHandle.numClient += 1;
	}
}