#include<WinSock2.h>
#include<iostream>
#include"Client.h"
#include "HandleClientRequest.h"
#include<mutex>
using namespace std;
HandleClientRequest sHandle;
mutex g_mtx;
HANDLE bufferFullEvent;
void CreateBufferEvent() {
	bufferFullEvent = CreateEventA(NULL, true, true, NULL);
}

DWORD WINAPI HandleClientRequests(LPVOID param) {
	TLVBuffer buff;
	while (true) {
		// kiểm tra có client nào đang chạy không
		if (sHandle.numClient > 0) {
			g_mtx.lock();
			// kiểm tra buffer có đủ gói tin TLV hay không
			TLVPackage p = sHandle.rqBuffer.getPackage();
			SetEvent(bufferFullEvent);
			g_mtx.unlock();
			while (p.getTitle() != -1) {
				// nếu trạng thái của HandleClientRequest là đang bận,
				//và gói tin là gói tin nghiệp vụ, 
				//gói tin này sẽ được đưa xuống cuối buffer để xử lý sau khi hết bận
				if (p.getTitle() == CONTROL_MESSAGE && sHandle.state == true) {
					// tim hieu auto lock
					g_mtx.lock();
					sHandle.rqBuffer.addData(p.packageValue(), p.getLength());
					g_mtx.unlock();
				}
				else if(p.getTitle() == NO_CONTENT_PACKET){
				}
				else {
					// nếu không thì sẽ sử lý gói tin
					sHandle.HandlePacket(p);
				}
				g_mtx.lock();
				p = sHandle.rqBuffer.getPackage();
				g_mtx.unlock();
			}
		}
	}
}
DWORD WINAPI WaitClientEvent(LPVOID param){
	int id = (int)param;// id của client
	bool isClient = false;// tình trạng xác thực, true: đã xác thực, false: chưa
	Client client = sHandle.getClient(id);
	while (true) {
		char buff[1024]{ 0 };
		int byteRecv = recv(client.socket, buff, sizeof(buff), 0);
		if (byteRecv > 0) {
			if (isClient) {// nếu đã xác thực thì đưa gói tin vào TLV buffer để đối tượng HandleClientRequest xử lý
				
				while (true) {
					WaitForSingleObject(bufferFullEvent, INFINITE);
					g_mtx.lock();
					bool s = sHandle.rqBuffer.addData(buff, byteRecv);
					if (s) {
						g_mtx.unlock();
						break;
					}
					ResetEvent(bufferFullEvent);
					g_mtx.unlock();
				}
			}
			else {
				// nếu không thì gói tin này là gói tin xác thực
				char userName[1024]{ 0 }, password[1024]{ 0 };
				TLVPackage p(buff);
				sscanf(p.getValue(), "%s%s", userName, password);
				// xác thực
				isClient = client.checkAccount(userName, password, id);
				if (!isClient) {// chưa xác minh đăng nhập được, gửi lại y/c xác thực đăng nhập
					TLVPackage p(NOTIFY_MESSAGE, id, 31, (char*)"account is'nt correct!");
					send(client.socket, p.packageValue(), p.getLength(), 0);
				}
				else {// nếu đúng, gửi yêu cầu thành công
					TLVPackage p(LOGIN_SUCESS, id, 28, (char*)"account is correct!");
					send(client.socket, p.packageValue(), p.getLength(), 0);
				}
			}
		}
		else {
			cout << "A client has disconnect" << endl;
			// kiểm tra trạng thái của client
			Client c = sHandle.getClient(id);
			// nếu client này đang gửi file tới server
			if (c.status.second != Client::FREE) {
				// log lại vị trí đã gửi được
				sHandle.logs.push_back(c.getStatus());
				TLVPackage p(DATA_STREAM_END, id, 8, (char*)"");
				sHandle.HandleData(p, id);
				sHandle.state = false;
			}
			// gưi một gói tin rỗng để client đóng file đang mở để ghi.
			sHandle.removeClient(id);
			break;
		}
	}
	return 0;
}


int main() {
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
	sHandle.setPath((char*)"Repo");
	CreateBufferEvent();
	// tạo thread xử lý các request từ client
	CreateThread(NULL, 0, HandleClientRequests, NULL, 0, NULL);
	while (0 == 0) {
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
		DWORD ID;
		Client client(c, sHandle.numClient, caddr);
		sHandle.addClient(client);
		// tạo thread để lắng nghe các request từ client
		CreateThread(NULL, 0, WaitClientEvent, (LPVOID)sHandle.numClient, 0, &ID);
		sHandle.numClient += 1;
	}
}