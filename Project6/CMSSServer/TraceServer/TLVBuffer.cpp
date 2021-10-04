#include "TLVBuffer.h"

TLVBuffer::TLVBuffer()
{
	// Khởi tạo mảng rỗng, độ dài 0
	resetBuffer();
}

TLVPackage TLVBuffer::getPackage()
{
	const int MSG_TYPE_EMPTY = -1;
	TLVPackage p;
	p.setTitle(INVALID_MESSAGE);
	// Kiểm tra độ dài của luồng dữ liệu để xác nhận nó có là một TLV package hay không
	if (buffLen < 8) {
		return p;
	}
	else {
		// Đọc các giá trị title, length
		char Title[2];
		char Id[2];
		char Length[4];
		char Value[1016];
		memcpy(Title, buff, 2);
		memcpy(Id, buff + 2, 2);
		memcpy(Length, buff + 4, 4);
		int length = Common::byteToInt(Length);
		int title = Common::byteToShortIn(Title);
		int id = Common::byteToShortIn(Id);
		// Trả về NULL nếu dữ liệu trong buff không bằng độ dài của TLVPackage đang xét
		if (buffLen < length) {
			return p;
		}
		else {
			if (length == 0) {
				printf("err");
			}
			// Khởi tạo đối tượng TLVPackage
			memcpy(Value, buff + 8, length - 8);
			TLVPackage package(title , id, length, Value);
			// Dịch các dữ liệu còn lại trong package về đầu
			memcpy(buff, buff + length, buffLen - length);
			memset(buff + buffLen - length, 0, length);
			// Sửa lại độ dài package
			buffLen -= length;
			return package ;
		}
	}

}

bool TLVBuffer::addData(char* data, int len)
{
	if (BUFFER_SIZE - buffLen < len) {
		return false;
	}
	// Thêm dữ liệu vào cuối buff
	memcpy(buff + buffLen, data, len);
	//Sửa lại độ dài
	buffLen += len;
	return true;
}

char* TLVBuffer::getData()
{
	return buff;
}

bool TLVBuffer::addTLVBuffer(TLVBuffer buf)
{
	return addData(buf.getData(), buf.buffLen);
}

void TLVBuffer::resetBuffer()
{
	memset(buff, 0, sizeof(buff));
	buffLen = 0;
}

//int main() {
//	TLVBuffer buff;
//	TLVPackage pack(1,1, 16, (char*)"HelloHD");
//	TLVPackage pack2(1,1, 20, (char*)"Hello World");
//	buff.addData(pack.packageValue(), pack.getLength());
//	buff.addData(pack2.packageValue(), 10);
//	char a[10];
//	memcpy(a, pack2.packageValue()+10, 10);
//	buff.addData(a, 10);
//	TLVPackage p = buff.getPackage();
//	p = buff.getPackage();
//}