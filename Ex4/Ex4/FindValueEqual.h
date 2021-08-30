#pragma once
#ifndef FINDVALUEEQUAL_H
#define FINDVALUEEQUAL_H
#include "FindValueInVector.h"
/// <summary>
/// lớp tìm kiếm các kết quả bằng nhau trong một vector kiểu int
/// </summary>
class FindValueEqual :
    public FindValueInVector<int>
{
    // Inherited via FindValueInVector
public:
    /// <summary>
    /// Hàm khởi tạo
    /// </summary>
    /// <param name="findArr"> vector tìm kiếm</param>
    FindValueEqual(std::vector<int> findArr);
    /// <summary>
    /// Hàm tìm kiếm
    /// </summary>
    /// <param name="findValue"> giá trị cần tìm</param>
    /// <returns> vị trí tìm thấy hoặc -1 nếu không tìm thấy</returns>
    virtual int find(int findValue);
};
#endif // !FINDVALUEEQUAL_H


