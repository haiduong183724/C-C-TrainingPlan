#pragma once
#include "TLVPackage.h"
#ifndef TLVBUFFER_H
#define TLVBUFFER_H
/// <summary>
/// Chứa các gói tin TLV 
/// </summary>
#define BUFFER_SIZE 50000
class TLVBuffer
{
private:
	char buff[BUFFER_SIZE];
public:
	int buffLen = 0;
	/// <summary>
	/// Hàm khởi tạo
	/// </summary>
	TLVBuffer();
	/// <summary>
	/// Lấy ra một TLVPackage từ buffer
	/// </summary>
	/// <returns> Trả về một TLV Package đầu tiên tìm được, nếu không có thù trả về một package NULL</returns>
	TLVPackage getPackage();
	/// <summary>
	/// Thêm một luồng dữ liệu vào buffer
	/// </summary>
	/// <param name="data">con trỏ luồng dữ liệu</param>
	/// <param name="len">Số lượng byte</param>
	bool addData(char* data, int len);
	/// <summary>
	/// lấy dữ liệu trong buffer
	/// </summary>
	/// <returns></returns>
	char* getData();
	/// <summary>
	/// copy dữ liệu của một buffer này vào một buffer khác
	/// </summary>
	/// <param name="buf"></param>
	/// <returns></returns>
	bool addTLVBuffer(TLVBuffer buf);
	/// <summary>
	/// reset lại buffer
	/// </summary>
	void resetBuffer();
};
#endif // !TLVBUFFER_H



