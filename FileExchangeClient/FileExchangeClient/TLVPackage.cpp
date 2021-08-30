#include "TLVPackage.h"
#include<vector>
#include<iostream>
#include<fstream>
    // Chuyển từ số int sang dạng  4 byte 
    void Common::intToByte(int n, char* result) {

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
int TLVPackage::getTitle()
{
    return title;
}

TLVPackage::TLVPackage(char* package)
{
    char Length[4];
    char Title[4];
    // Khởi tạo value
    memcpy(Length, package + 4, 4);
    memcpy(Title, package, 4);
    memset(value, 0, sizeof(value));
    title = Common::byteToInt(Title);
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

TLVPackage::TLVPackage(int Title, int Length, char* Value)
{
    memset(value, 0, sizeof(value));
    memcpy(value, Value, Length - 8);
    title = Title;
    length = Length;
}

TLVPackage::TLVPackage()
{
    memset(value, 0, sizeof(value));
    length = 0;
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

char* TLVPackage::packageValue()
{
    char package[1024];
    char Title[4];
    char Length[4];
    memset(package, 0, sizeof(package));
    Common::intToByte(length, Length);
    Common::intToByte(title, Title);
    memcpy(package, Title, 4);
    memcpy(package+ 4, Length, 4);
    memcpy(package + 8, value, length - 8);
    return package;
}
//int main() {
//    FILE* f = fopen("anh.jpg", "rb");
//    fstream file;
//    file.open("anh2.jpg", ios::out | ios::binary);
//    fseek(f, 0, SEEK_END);
//    int flen = ftell(f);
//    fseek(f, 0, SEEK_SET);
//    char data[1016];
//    int read = 0;
//    char packageValue[1024];
//    memset(packageValue, 0, sizeof(packageValue));
//    if (data != NULL) {
//        while (read < flen) {
//            memset(data, 0, sizeof(data));
//            int byteRead = min(1016, flen - read);
//            fread(data, 1, byteRead, f);
//            cout << ftell(f) << endl;
//            TLVPackage pakage;
//            pakage.setLength(flen);
//            pakage.setTitle(1);
//            pakage.setValue(data);
//            read += byteRead;
//            memset(packageValue, 0, sizeof(packageValue));
//            memcpy(packageValue, pakage.packageValue(), sizeof(packageValue));
//            TLVPackage p(packageValue, byteRead+8);
//            file.write(p.getValue(), byteRead);
//        }
//        fclose(f);
//        file.close();
//    }
//    //unsigned int n = 10000;
//    //char numberStr[4];
//    //memset(numberStr, 0, 4);
//    //Common::intToByte(1000, numberStr);
//    //printf("%x %x %x %x\n", numberStr[0], numberStr[1], numberStr[2], numberStr[3]);
//    //cout << Common::byteToInt(numberStr);
//    //cin.get();
//}