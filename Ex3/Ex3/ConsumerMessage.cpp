#include "ConsumerMessage.h"

void ConsumerMessage::operator()(std::queue<std::string>* productQ, HANDLE* hNotEmpty, HANDLE* hNotFull, HANDLE* hMutex)
{
	while (1) {
		// đợi cho queue không rỗng
		WaitForSingleObject(*hNotEmpty, INFINITE);
		// đợi quyền sở hữu mutex
		WaitForSingleObject(*hMutex, INFINITE);
		// lấy message trong queue để sử dụng
		std::string s = productQ->front();
		productQ->pop();
		// thông báo event queue không bị đầy
		SetEvent(*hNotFull);
		consum(s);
		// nếu queue sau khi bị lấy mà rỗng thì gọi sự kiện queue rỗng
		if (productQ->empty()) {
			ResetEvent(*hNotEmpty);
			std::cout << "queue is empty!" << std::endl;;
		}
		// giải phóng quyền sử dụng của mutex
		ReleaseMutex(*hMutex);
		SleepEx(rand() % 200, true);
	}
}
void ConsumerMessage::consum(std::string s)
{
	std::cout << "Get " << s << std::endl;
}

