#pragma once
#ifndef SYSTEMSORT_H
#define SYSTEMSORT_H
#include"Sort.h"
#include<algorithm>
/// <summary>
/// Lớp sắp xếp vector theo thứ tự tăng dần
/// </summary>
class SystemSort:public Sort
{
	// Inherited via Sort
public:
	/// <summary>
	/// Sắp xếp một vector tăng dần
	/// </summary>
	/// <param name="sortArr">vector cần sắp xếp</param>
	virtual void sortVector(std::vector<int>* sortArr) override;
};
#endif // !SYSTEMSORT_H



