#include "HandleClientRequest.h"

HandleClientRequest::HandleClientRequest(SOCKET ClientSocket, const char* serverRepo)
{
	s = ClientSocket;
	sApi = new ServerAPI(s);
	if (strlen(serverRepo) == 0)
		path = new Directory(SERVER_DEFAULT_REPO);
	else
		path = new Directory(serverRepo);
}

void HandleClientRequest::handleRequest(char* request)
{
	char rq[1024],// Request
		cnt[1024];// Nội dung request
	memset(rq, 0, sizeof(rq));
	memset(cnt, 0, sizeof(cnt));
	sscanf(request, "%s%s", rq, cnt);
	strupr(rq);
	switch (rq[0]) {
	case 'C':// di chuyển thư mục đọc ghi file
	{
		changePath(cnt);
		break;
	}
	case 'G':// tải 1 file từ server về
	{
		sendFile(cnt);
		break;
	}
	case 'S':// gửi 1 file lên server
	{
		getFile(cnt);
		break;
	}
	case 'D':// ngắt kết nối
	{
		closesocket(s);
		break;
	}
	default:
	{

	}
	}

}

void HandleClientRequest::changePath(char* newPath)
{
	if (strcmp(newPath, "..") == 0) {
		if (path->goBack()) {
			// gửi danh sách các thư mục cho Client
			sendCurrentDirectory();
		}
		// Thông báo lỗi, gửi danh sách các thư mục hiện tại cho Client
		sendCurrentDirectory();
	}
	else {
		if (path->goIn(newPath)) {
			// gửi danh sách các thư mục lại cho Client
			sendCurrentDirectory();
		}
		// Thông báo lỗi không thấy thư mục cho client, gửi ds các thư mục
		sendCurrentDirectory();
	}
}

void HandleClientRequest::closeConnect()
{
	closesocket(s);
}

void HandleClientRequest::sendFile(char* fileName)
{
	if (path->checkFileExist(fileName)) {
		char fullPath[1024];
		memset(fullPath, 0, sizeof(fullPath));
		sprintf(fullPath, "%s\\%s", path->getPath(), fileName);
		char response[1024];
		memset(response, 0, sizeof(response));
		sprintf(response, "SENDFILE %s", fileName);
		TLVPackage p(101, 8+ strlen(response), response);
		send(s, p.packageValue(), p.getLength(), 0);
		sApi->sendFile(fullPath);
		TLVPackage k(100,15, (char*)"done!\n");
		send(s, k.packageValue(), k.getLength(), 0);
	}
	else {
		// file đã tồn tại ,gửi response từ chối
		TLVPackage p(100, 23, (char*)"file is'nt exist!");
		send(s, p.packageValue(), p.getLength(), 0);
	}
}
void HandleClientRequest::getFile(char* fileName)
{
	// Kiểm tra file có tồn tại trong thư mục hay chưa
	if (!path->checkFileExist(fileName)) {
		char fullPath[1024];
		memset(fullPath, 0, sizeof(fullPath));
		sprintf(fullPath, "%s\\%s", path->getPath(), fileName);
		f.open(fullPath, std::ios::out|ios::binary);
		char response[1024];
		memset(response, 0, sizeof(response));
		sprintf(response, "GET %s", fileName);
		TLVPackage p(101, 8 + strlen(response), response);
		send(s, p.packageValue(), p.getLength(), 0);
	}
	else {
		// file chưa tồn tại, gửi response từ chối
		TLVPackage p(100, 26, (char*)"file is exist!");
		send(s, p.packageValue(), p.getLength(), 0);
	}
}

void HandleClientRequest::closeFile()
{
	if (f.is_open()) {
		f.close();
	}

}

void HandleClientRequest::handleData(char* data, int dLen)
{
	if(f.is_open())
	{
		f.write(data, dLen);
	}
}

void HandleClientRequest::sendCurrentDirectory()
{
	vector<string> files = path->scanFolder();
	char response[1024];
	memset(response, 0, sizeof(response));
	strcat(response, "List file and directory at current pointer: \n ");
	for (int i = 0; i < files.size(); i++) {
		if (i > 0) {
			strcat(response, files.at(i).c_str());
			strcat(response, "\n");
		}
	}
	TLVPackage p(100, strlen(response) + 8, response);
	send(s, p.packageValue(), p.getLength(), 0);
}
