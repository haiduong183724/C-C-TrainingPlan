#pragma once
#include<string>
#include<WinSock2.h>
using namespace std;
#ifndef TLVPAGKAGE_H
#define TLVPACKAGE_H

namespace Common
{
	void intToByte(int n, char* result);
	int byteToInt(char* bytes);
}
/// <summary>
/// Gói tin TLV chuyển thông tin qua socket
/// </summary>
class TLVPackage
{
private:
	int title;// Tiêu đề 200: OK, 201: ENDFILE, 404: ERROR
	int length;// Chiều dài gói tin
	char value[1016];// Nội dung gói tin
public:
	/// <summary>
	/// hàm khởi tạo 1
	/// </summary>
	TLVPackage(char* package);
	/// <summary>
	/// Hàm khởi tạo 2
	/// </summary>
	TLVPackage(int Title, int Length, char* Value);
	/// <summary>
	/// Hàm khởi tạo 3
	/// </summary>
	TLVPackage();
	// Lấy tiêu đề
	int getTitle();
	// Lấy đọ dài
	int getLength();
	// Lấy giá trị
	char* getValue();
	// Lấy chuỗi gói tin
    char* packageValue();
	// thay đổi giá trị
    void setValue(char* Value, int valueLength);
	// Thay đổi tiêu đề
    void setTitle(int Title);
	// Thay đổi độ dài
    void setLength(int Length);
};
#endif // !TLVPAGKAGE_H


