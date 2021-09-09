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
	void setDate(SYSTEMTIME st);
	DateTime(int Year, int Month, int Day);
	DateTime(int Year, int Month, int Day, int Hour, int Min, int Second);
	bool operator >(DateTime date);
	bool operator == (DateTime date);
	DateTime(std::string dateString);
	static DateTime Now();
};

#endif // !DATETIME_H


