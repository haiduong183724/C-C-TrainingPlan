﻿#pragma once
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
	/// <summary>
	/// trạng thái làm việc của đối tượng
	/// true: đang bận, không thể xử lý các y/c khác
	/// false : đanh rảnh
	/// </summary>
	bool state = false;
	/// <summary>
	/// Hàm khởi tạo
	/// </summary>
	HandleClientRequest();
	/// <summary>
	/// nơi chứa các gói tin TLV được gửi tới server
	/// </summary>
	TLVBuffer rqBuffer;
	/// <summary>
	/// số lượng client đang có
	/// </summary>
	int numClient = 0;
	/// <summary>
	/// Thêm một client mới 
	/// </summary>
	/// <param name="c"></param>
	void addClient(Client c);
	/// <summary>
	/// Xóa bớt một client
	/// </summary>
	/// <param name="clientId">id của client</param>
	void removeClient(int clientId);
	/// <summary>
	/// Lấy đối tượng client
	/// </summary>
	/// <param name="clientId">Id của client</param>
	/// <returns></returns>
	Client getClient(int clientId);
	/// <summary>
	/// Xử lý một TLV packet
	/// </summary>
	/// <param name="p">TLV packet</param>
	void HandlePacket(TLVPackage p);
	/// <summary>
	/// Xử lý một request từ client
	/// </summary>
	/// <param name="rq">nội dung</param>
	/// <param name="from">id của client gửi tới</param>
	void HandleRequest(char* rq, int from);
	/// <summary>
	/// Xử lý dữ liệu file từ client gửi tới
	/// </summary>
	/// <param name="p">gói tin TLV</param>
	/// <param name="from">Id của client gửi</param>
	void HandleData(TLVPackage p, int from);
	/// <summary>
	/// xử lý yêu cầu xóa file của client
	/// </summary>
	/// <param name="filePath">đường dẫn tới file xóa</param>
	/// <param name="from">id của client gửi tới</param>
	void Delete(char* filePath, int from);
	/// <summary>
	/// Xử lý yêu cầu thêm một file mới
	/// </summary>
	/// <param name="filePath">tên file</param>
	/// <param name="from">id của client</param>
	void Add(char* filePath, int from);
	/// <summary>
	/// Xử lý yêu cầu đổi tên
	/// </summary>
	/// <param name="filePath"> nội dung gửi</param>
	/// <param name="from">id của client gửi tới</param>
	void Rename(char* filePath, int from);
	/// <summary>
	/// xử lý yêu cầu chỉnh sửa một file
	/// </summary>
	/// <param name="filePath">tên file chỉnh sửa</param>
	/// <param name="from">id của client gửi</param>
	void Edit(char* filePath, int from);
};

#endif // !HANDLECLIENTREQUEST_H

