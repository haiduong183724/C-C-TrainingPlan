#pragma once

#ifndef PRODUCERMESSAGE_H
#define PRODUCERMESSAGE_H
#include<queue>
#include<string>
#include<iostream>
#include<Windows.h>
const int MAX_QUEUE_LENGTH = 3;
class ProducerMessage
{
public:
	void operator()(std::queue<std::string>* productQ, HANDLE* hNotEmpty, HANDLE* hNotFull, HANDLE* hMutex);
	void produce() ;
};
#endif // !PRODUCERMESSAGE_H


