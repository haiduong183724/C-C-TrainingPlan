#pragma once
#ifndef SORT_H
#define SORT_H
#include<vector>
/// <summary>
/// Interface sắp xếp một vector
/// </summary>
class Sort
{
public:
	virtual void sortVector(std::vector<int>* sortArr) = 0;
};
#endif


