#pragma once
#include<string>
#ifndef DATETIME_H
#define DATETIME_H
#include<time.h>
#include<WinSock2.h>
class DateTime
{
public:
	int year = 2000;
	int month = 1;
	int day = 1;
	int hour = 12;
	int minute = 0;
	int second = 0;
	DateTime();
	/// <summary>
	/// Thiết lập ngày tháng năm
	/// </summary>
	/// <param name="st"> kiểu dữ liệu thời gian của window</param>
	void setDate(SYSTEMTIME st);
	DateTime(int Year, int Month, int Day);
	DateTime(int Year, int Month, int Day, int Hour, int Min, int Second);
	/// <summary>
	/// So sánh với một đối tượng khác
	/// </summary>
	/// <param name="date">đối tượng cần so sánh</param>
	/// <returns>true nếu lớn hơn, false ngược lại</returns>
	bool operator >(DateTime date);
	/// <summary>
	/// so sánh bằng
	/// </summary>
	/// <param name="date"></param>
	/// <returns></returns>
	bool operator == (DateTime date);
	/// <summary>
	/// Hàm khởi tạo khi đầu vào là một chuỗi
	/// </summary>
	/// <param name="dateString"></param>
	DateTime(std::string dateString);
	/// <summary>
	/// Lấy ngày giờ hiện tại
	/// </summary>
	/// <returns>ngày giờ hiện tại</returns>
	static DateTime Now();
	int operator -(DateTime d);
};

#endif // !DATETIME_H


