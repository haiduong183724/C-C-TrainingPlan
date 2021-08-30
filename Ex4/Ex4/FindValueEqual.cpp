#include "FindValueEqual.h"


FindValueEqual::FindValueEqual(std::vector<int> findArr)
{
    findVector = findArr;
}

int FindValueEqual::find(int findValue)
{
    for (int i = 0; i < findVector.size(); i++) {
        if (findVector[i] == findValue) {
            return i;
        }
    }
    return -1;
}
