#pragma once
#ifndef QUICKSORT_H
#define QUICKSORT_H
#include"Sort.h"
class QuickSort:public Sort
{
public:// Inherited via Sort
	virtual void sort(int sortArr[], int start, int end) override;
};

#endif // !QUICKSORT_H


