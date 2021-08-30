#pragma once
#ifndef FINDVALUEINVECTOR_H
#define FINDVALUEINVECTOR_H
#include<vector>
template<class T>
/// <summary>
/// Tìm kiếm một giá trị thỏa mãn nào đó trong một vector
/// </summary>
/// <typeparam name="T"> kiểu dl vector cần tìm kiếm</typeparam>
class FindValueInVector {
protected:
	std::vector<T> findVector;
public:
	virtual int find(T findValue) = 0;
};
#endif // !FINDVALUEINVECTOR_H
