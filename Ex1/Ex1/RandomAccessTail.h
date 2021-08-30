#pragma once
#include"TextFile.h"
#ifndef RANDOMACCESSTAIL_H
#define RANDOMACCESSTAIL_H
class RandomAccessTail : public TextFile
{
	// gán giá trị cho filename
	//void setFileName(char* fileName);
	// đọc n dòng từ cuối file
	void tailFile(int numRow);
};
#endif
