#pragma once
#include<WinSock2.h>
#include "TLVBuffer.h"
#ifndef SERVERAPI_H
#define SERVERAPI_H
/// <summary>
/// chứa các giao tiếp với client
/// </summary>
class ServerAPI
{
protected:
	SOCKET s;
	TLVBuffer buff;
public:
	void sendFile(const char* fileName);
	void getFile(const char* fileName);
	ServerAPI(SOCKET c);
};
#endif // !SERVERAPI_H



