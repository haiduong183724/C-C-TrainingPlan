#pragma once
#include <WinSock2.h>
#include "TLVBuffer.h"
#include"SocketClient.h"
#ifndef CLIENT_H
#define CLIENT_H
class FileHandle:public SocketClient
{
private:
	SOCKET s;
public:
	/// <summary>
	/// Hàm khởi tạo
	/// </summary>
	/// <param name="path"> đường dẫ tới thư mục chứa của client</param>
	/// <param name="s"> Socket kết nối tới server</param>
	FileHandle(SOCKET s);
	/// <summary>
	/// Nhận file từ server
	/// </summary>
	/// <param name="fileName">Tên file cần nhận</param>
	void getFile(const char* fileName);
	/// <summary>
	/// Gửi file tới server
	/// </summary>
	/// <param name="fileName"> Tên file gửi</param>
	void sendFile(const char* fileName);
};

#endif // !CLIENT_H


