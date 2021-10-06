#pragma once
#include <WinSock2.h>
#include<string>
#ifndef CLIENT_H
#define CLIENT_H
class Client
{
private:
	int id;
public:
	bool isValidate = false;
	SOCKADDR_IN caddr;
	static const int FREE = -1;
	std::pair<std::string, int> status;
	SOCKET socket;
	Client(SOCKET c, int Id, SOCKADDR_IN saddr);
	bool checkAccount(char* userName, char* password, int id);
	int getId();
	void changeStatus(std::string fileName, int satus);
	std::string getStatus();
};

#endif // !CLIENT_H


