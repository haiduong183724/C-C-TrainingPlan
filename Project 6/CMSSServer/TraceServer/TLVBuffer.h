#pragma once
#include "TLVPackage.h"
#ifndef TLVBUFFER_H
#define TLVBUFFER_H
/// <summary>
/// Chứa các gói tin TLV 
/// </summary>
class TLVBuffer
{
private:
	char buff[100000];
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
	void addData(char* data, int len);
};
#endif // !TLVBUFFER_H



