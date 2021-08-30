#include "ProducerMessage.h"

void ProducerMessage::operator()(std::queue<std::string>* productQ, HANDLE* hNotEmpty, HANDLE* hNotFull , HANDLE* hMutex)
{
	while (true)
	{
		// đợi queue không bị đầy
		WaitForSingleObject(*hNotFull, INFINITE);
		// đợi quyền sử hữu mutex
		WaitForSingleObject(*hMutex, INFINITE);
		// đưa một message vào queue
		productQ->push("Product A");
		produce();
		// Thiết lập sự kiện queue không rỗng
		SetEvent(*hNotEmpty);
		// Nếu queue sau khi thêm sản phẩm mà đầy thì gọi sự kiện queue đầy
		if (productQ->size() == MAX_QUEUE_LENGTH) {
			ResetEvent(*hNotFull);
			std::cout << "Queue is full!" << std::endl;
		}
		// giải phóng quyền sở hữu mutex
		ReleaseMutex(*hMutex);
		SleepEx(rand()%200, true);
	}
}

void ProducerMessage::produce()
{
	std::cout << "Add Product A" << std::endl;
}
