
#include "ServerAPI.h"
#include<iostream>
#include <WinSock2.h>
using namespace std;

ServerAPI::ServerAPI(const char* path, SOCKET c)
{
	sFilter = new ServerFilter(path);
	sData = new ServerData(path);
	s = c;
}

void ServerAPI::sendFile(const char* fileName)
{
	// khi file chưa tồn tại
	if (!sFilter->checkFileExist(fileName)) {
		// gửi từ chối tới client
		send(s, "file is'nt exist on server", 27, 0);
	}
	else {
		// gửi nội dung tới client
		cout << "file was found" << endl;
		send(s, "OK", 3, 0);
		pair<char*, int> fileContent = sData->readFile(fileName);
		int sent = 0;
		while (sent < fileContent.second) {
			sent += send(s, fileContent.first + sent, fileContent.second - sent, 0);
		}
		cout << "Sending ok" << endl;
		send(s, "ENDFILE", 8, 0);
	}
}
void ServerAPI::getFile(const char* fileName)
{
	if (sFilter->checkFileExist(fileName)) {
		// gửi từ chối tới client
		send(s, "file is already exist on server", 32, 0);
	}
	else {
		// gửi yêu cầu nhận file, nội dung file
		send(s, "OK", 3, 0);
		while (true) {
			char buffer[1000000];
			memset(buffer, 0, sizeof(buffer));
			recv(s, buffer, sizeof(buffer), 0);
			// Nếu hết file thì kết thúc
			if (!strcmp(buffer, "ENDFILE")) {
				break;
			}
			// thực hiện ghi nội dung file
			sData->writeFile(buffer, fileName);
		}

	}
}
//int main() {
//	ServerAPI sAPI("C:\\Users\\Admin\\source\\repos\\FileExchangeServer\\FileExchangeServer");
//	ResultMsg msg = sAPI.readFile("input.txt");
//	ResultMsg msg2 = sAPI.writeFile(msg.getContent().c_str(), "hello2.txt");
//	char ch = cin.get();
//	cout << ch;
//}