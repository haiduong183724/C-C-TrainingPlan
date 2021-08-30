#pragma 
#ifndef CONSUMERMESSAGE_H
#define CONSUMERMESSAGE_H
#include<queue>
#include<string>
#include<iostream>
#include<Windows.h>
class ConsumerMessage
{
public: 
	void operator()(std::queue<std::string>* productQ, HANDLE* hNotEmpty, HANDLE* hNotFull, HANDLE* hMutex);
	void consum(std::string s);
};
#endif // !CONSUMERMESSAGE_H



