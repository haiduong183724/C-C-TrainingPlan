#pragma once
#ifndef CLIENTHANDLEREQUEST_H
#define CLIENTHANDLEREQUEST_H
#include<WinSock2.h>
#include<fstream>
#include<io.h>
#include"Directory.h"
#include"TLVPackage.h"
using namespace std;
class ClientHandleRequest
{
private:
	SOCKET s;
	fstream f;
	char path[1024];
	int id = 0;
public:
	Directory* d;
	ClientHandleRequest();
	/// <summary>
	/// Hàm khởi tạo
	/// </summary>
	/// <param name="s">Socket kết nối với server</param>
	/// <param name="Path">thư mục lưu trữ các file</param>
	ClientHandleRequest(SOCKET s, const char* Path);
	/// <summary>
	/// Xử lý các response thông báo từ server
	/// </summary>
	/// <param name="response"></param>
	void handleResponse(char* response);
	/// <summary>
	/// Xử lý các response thao tác từ server
	/// </summary>
	/// <param name="response"></param>
	void handleResponse2(char* response);
	/// <summary>
	/// Xử lý dữ liệu từ server
	/// </summary>
	/// <param name="data">dữ liệu</param>
	/// <param name="dataLen">độ dài</param>
	void handleData(char* data, int dataLen);
	/// <summary>
	/// đăng nhập
	/// </summary>
	void login();
	/// <summary>
	/// mở file để thực hiện thao tác ghi
	/// </summary>
	/// <param name="fileName">đường dẫn tới file</param>
	void openFile(char* fileName, int filePositon);
	/// <summary>
	/// đóng file
	/// </summary>
	void closeFile();
	/// <summary>
	/// gửi file tới server
	/// </summary>
	/// <param name="fileName">đường dẫn tới file</param>
	void sendFile(char* fileName, int position);
	/// <summary>
	/// Xóa file
	/// </summary>
	/// <param name="fileName"></param>
	void deleteFile(char* fileName);
	/// <summary>
	/// Đổi tên file
	/// </summary>
	/// <param name="oldName"></param>
	/// <param name="newName"></param>
	void renameFile(char* oldName, char* newName);

	void setId(int Id);
	int getId();
	void setSocket(SOCKET s);
};
#endif // !CLIENTHANDLEREQUEST_H



