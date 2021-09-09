#pragma once
#ifndef HANDLECLIENTREQUEST_H
#define HANDLECLIENTREQUEST_H
#include"HandleRequest.h"
#include"FileHandle.h"
#include"Directory.h"
#include"TLVPackage.h"
#include<fstream>
class HandleClientRequest:public HandleRequest
{
	SOCKET s;// SOCKET kết nối tới client
	FileHandle *sApi;// công cụ để ghi dữ liệu
	Directory *path;// đường dẫn đến thư mục hiện tại
	fstream f;// file ghi dữ liệu nhận được
	// Inherited via HandleRq
public:
	/// <summary>
	/// Hàm khởi tạo
	/// </summary>
	/// <param name="s">Socket kết nối</param>
	HandleClientRequest(SOCKET s, const char* ServerRepo);
	/// <summary>
	/// Xử lý request từ Client
	/// </summary>
	/// <param name="request"> Request</param>
	virtual void handleRequest(char* request) override;
	/// <summary>
	/// Thay đổi thư mục
	/// </summary>
	/// <param name="path"> thư mục mới</param>
	void changePath(char* path);
	/// <summary>
	/// đóng kết nối
	/// </summary>
	void closeConnect();
	/// <summary>
	/// Gửi dữ liệu một file đi
	/// </summary>
	/// <param name="fileName"> đường dẫn file</param>
	void sendFile(char* fileName);
	/// <summary>
	/// Nhận dữ liệu của một file
	/// </summary>
	/// <param name="fileName"> tên file</param>
	void getFile(char* fileName);
	/// <summary>
	/// Đóng file
	/// </summary>
	void closeFile();
	/// <summary>
	/// Xử lý dư liệu nhận từ server
	/// </summary>
	/// <param name="data"> dữ liệu</param>
	/// <param name="dLen"> độ dài dữ liệu</param>
	virtual void handleData(char* data, int dLen) override;
	
	void sendCurrentDirectory();
};
#endif // !HANDLECLIENTREQUEST_H



