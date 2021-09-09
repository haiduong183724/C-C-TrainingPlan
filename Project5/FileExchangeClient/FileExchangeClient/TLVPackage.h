#pragma once
#include<string>
#include<WinSock2.h>
using namespace std;
#ifndef TLVPAGKAGE_H
#define TLVPACKAGE_H

namespace Common {
	void intToByte(int n, char* result);
	int byteToInt(char* bytes);
}
class TLVPackage
{
private:
	int title;
	int length;
	char value[1016];
public:
	TLVPackage(char* package);
    TLVPackage(int Title, int Length, char* Value);
    TLVPackage();
	int getTitle();
	int getLength();
	char* getValue();
    char* packageValue();
    void setValue(char* Value, int valueLength);
    void setTitle(int Title);
    void setLength(int Length);
};
#endif // !TLVPAGKAGE_H


