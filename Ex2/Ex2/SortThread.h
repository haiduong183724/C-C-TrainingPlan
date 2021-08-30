#pragma once
#ifndef SORTTHREAD_H
#define SORTTHREAD_H
#include<vector>
#include"Sort.h"
#include"MergeSort.h"
#include"QuickSort.h"
#include<Windows.h>
#include<iostream>
using namespace std;
class SortThread
{
public:
	void operator ()(int sortArr[], int start, int end);

};
#endif // !SORTTHREAD_H



