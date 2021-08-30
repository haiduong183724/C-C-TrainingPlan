#include<iostream>
#include"ProducerMessage.h"
#include"ConsumerMessage.h"
#include<thread>
#include<string>
#include<vector>
using namespace std;
const int MAX_PRODUCT = 3;// Số msg tối đa trong queue
int main() {
	queue<string> productQueue;// Message queue
	HANDLE hQNotEpmty, hQNotFull, hMutex;
	hQNotEpmty = CreateEventA(NULL, true, false, NULL);
	hQNotFull = CreateEventA(NULL, true, true, NULL);
	hMutex = CreateMutexA(NULL, FALSE, NULL);
	thread* consumerThread = new thread(ConsumerMessage(), &productQueue, &hQNotEpmty, &hQNotFull, &hMutex);
	thread* producerThread = new thread(ProducerMessage(), &productQueue, &hQNotEpmty,&hQNotFull, &hMutex);
	consumerThread->join();
	producerThread->join();
	return 0;
}