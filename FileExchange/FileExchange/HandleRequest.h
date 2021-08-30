#pragma once
#ifndef HANDLEREQUEST_H
#define HANDLEREQUEST_H
#include<WinSock2.h>
#define SERVER_DEFAULT_REPO "C:\\"
class HandleRequest {
public:
	virtual void handleRequest(char* request) = 0;
	virtual void handleData(char* data, int dLen) = 0;
};
#endif // !HANDLEREQUEST_H
