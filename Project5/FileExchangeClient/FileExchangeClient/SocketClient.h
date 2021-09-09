#pragma once
#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
class SocketClient
{
private:
public:
	virtual void sendFile(const char* fileName) = 0;
	virtual void getFile(const char* fileName) = 0;
};
#endif // !SOCKETCLIENT_H
