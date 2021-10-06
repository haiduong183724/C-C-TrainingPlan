#pragma once
#include<string>
using namespace std;
#ifndef TLVPAGKAGE_H
#define TLVPACKAGE_H
#include<WinSock2.h>
enum MessageTitle
{
	DATA_STREAM = 1,
	DATA_STREAM_END = 0,
	NOTIFY_MESSAGE = 100,
	CONTROL_MESSAGE = 101,
	LOGIN_SUCESS = 200,
	INVALID_MESSAGE = -1,
	NO_CONTENT_PACKET = 201,
	NO_SYNC = 202,
};

namespace Common
{
	void intToByte(int n, char* result);
	int byteToInt(char* bytes);
	void shortInttoByte(int n, char* result);
	int byteToShortIn(char* bytes);
}
/// <summary>
/// Gói tin TLV chuyển thông tin qua socket
/// </summary>
class TLVPackage
{
private:
	int title;// Tiêu đề dùng để nhận dạng các loại gói tin -1 là gói tin ko hợp lệ
	int id; // là id của nguồn phát gói tin
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
	TLVPackage(int Title, int Id, int Length, char* Value);
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
	void setId(int Id);
	int getId();
};
#endif // !TLVPAGKAGE_H


