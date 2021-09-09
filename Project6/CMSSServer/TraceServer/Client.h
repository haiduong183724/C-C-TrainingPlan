#pragma once
#include <WinSock2.h>
#ifndef CLIENT_H
#define CLIENT_H
class Client
{
private:
	int id;
public:
	SOCKET socket;
	Client(SOCKET c, int Id);
	bool checkAccount(char* userName, char* password, int id);
	int getId();
};

#endif // !CLIENT_H


