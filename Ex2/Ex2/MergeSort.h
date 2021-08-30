#pragma once
#ifndef MERGESORT_H
#define MERGESORT_H
#include"Sort.h"
class MergeSort:public Sort
{
public:// Inherited via Sort
	virtual void sort(int sortArr[], int start, int end) override;
};
#endif // !MERGESORT_H



