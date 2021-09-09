#include "TLVPackage.h"
#include<vector>
#include<iostream>
#include<fstream>

// Chuyển từ số int sang dạng  4 byte 
void Common::intToByte(int n, char* result)
{

    result[0] = (char)(n & 0x000000ff);
    result[1] = (char)((n & 0x0000ff00) >> 8);
    result[2] = (char)((n & 0x00ff0000) >> 16);
    result[3] = (char)((n & 0xff000000) >> 24);
}
// Chuyển từ 4byte sang dạng int
int Common::byteToInt(char* bytes) {

    int n = 0;

    n = n + ((byte)bytes[0] & 0x000000ff);
    n = n + (((byte)bytes[1] & 0x000000ff) << 8);
    n = n + (((byte)bytes[2] & 0x000000ff) << 16);
    n = n + (((byte)bytes[3] & 0x000000ff) << 24);
    return n;
}

void Common::shortInttoByte(int n, char* result)
{
    result[0] = (char)(n & 0x000000ff);
    result[1] = (char)((n & 0x0000ff00) >> 8);
}

int Common::byteToShortIn(char* bytes)
{
    int n = 0;
    n = n + ((byte)bytes[0] & 0x000000ff);
    n = n + (((byte)bytes[1] & 0x000000ff) << 8);
    return n;
}

int TLVPackage::getTitle()
{
    return title;
}

TLVPackage::TLVPackage(char* package)
{
    char Length[4];
    char Title[2];
    char Id[2];
    // Khởi tạo value
    memcpy(Length, package + 4, 4);
    memcpy(Title, package, 2);
    memcpy(Id, package + 2, 2);
    memset(value, 0, sizeof(value));
    id = Common::byteToShortIn(Id);
    title = Common::byteToShortIn(Title);
    length = Common::byteToInt(Length);
    memcpy(value, package + 8, length - 8);
}

int TLVPackage::getLength()
{
    return length;
}

char* TLVPackage::getValue()
{
    return value;
}

TLVPackage::TLVPackage(int Title, int Id, int Length, char* Value)
{
    memset(value, 0, sizeof(value));
    memcpy(value, Value, Length - 8);
    title = Title;
    id = Id;
    length = Length;
}

TLVPackage::TLVPackage()
{
    memset(value, 0, sizeof(value));
    length = 0;
    id = 0;
    title = 0;
}

void TLVPackage::setValue(char* Value, int valueLength)
{
    memcpy(value, Value, valueLength);
}

void TLVPackage::setTitle(int Title)
{
    title = Title;
}

void TLVPackage::setLength(int Length)
{
    length = Length;
}

void TLVPackage::setId(int Id)
{
    id = Id;
}

int TLVPackage::getId()
{
    return id;
}

char* TLVPackage::packageValue()
{
    char package[1024];
    char Title[2];
    char Id[2];
    char Length[4];
    // Chuyển các giá trị sang chuỗi
    memset(package, 0, sizeof(package));
    Common::intToByte(length, Length);
    Common::shortInttoByte(title, Title);
    Common::shortInttoByte(id, Id);
    // Nối 
    memcpy(package, Title, 2);
    memcpy(package + 2, Id, 2);
    memcpy(package + 4, Length, 4);
    memcpy(package + 8, value, length - 8);
    return package;
}
//int main() {
//    TLVPackage p(200, 1, 14, (char*)"hello");
//    cout << p.getTitle() << endl;
//    cout << p.getLength() << endl;
//    cout << p.getId() << endl;
//    TLVPackage p1(p.packageValue());
//    cin.get();
//}