#include "DateTime.h"

DateTime::DateTime()
{
}

void DateTime::setDate(SYSTEMTIME st)
{
    year = st.wYear;
    month = st.wMonth;
    day = st.wDay;
    hour = st.wHour;
    minute = st.wMinute;
    second = st.wSecond;
}

DateTime::DateTime(int Year, int Month, int Day)
{
    year = Year;
    month = Month;
    day = day;
}

DateTime::DateTime(int Year, int Month, int Day, int Hour, int Min, int Second)
{
    year = Year;
    month = Month;
    day = day;
    hour = Hour;
    minute = Min;
    second = Second;
}

bool DateTime::operator>(DateTime date)
{
    if (year > date.year) {
        return true;
    }
    // chuyển về giây để so sánh
    int sum1 = month * 40 * 86400 + day * 86400 + hour * 3600 + minute * 60 + second;
    int sum2 = date.month * 40 * 86400 + date.day * 86400 + date.hour * 3600 + date.minute * 60 + date.second;
    return sum1 >= sum2;
}
bool DateTime::operator==(DateTime date)
{
    return year == date.year && month == date.month
        && day == date.day && hour == date.hour
        && minute == date.minute && second == date.second;
}
DateTime::DateTime(std::string dateString)
{
    // chuyển chuỗi định dạng 
    //năm/tháng/ngày/giờ/phút/giây sang datetime.
    int num = 0;
    int date[] = {2000, 11, 21, 0, 0, 0};
    int index = 0;
    for (int i = 0; i < dateString.length(); i++) {
        if (dateString[i] == '-' || dateString[i] == '/' || dateString[i] == ':') {
            date[index] = num;
            index++;
            num = 0;
        }
        else {
            int x = int(dateString[i]) - 48;
            if (x > 9 || x < 0) {
                break;
            }
            num = num * 10 + x;
            // giới hạn năm lại.
            if(num < 0 || num > 3000){
                break;
            }
        }
        if (num != 0) {
            date[index] = num;
        }
        year = date[0];
        month = date[1];
        day = date[2];
        hour = date[3];
        minute = date[4];
        second = date[5];
    }
}
char* DateTime::dateStr()
{
    char log[1024]{ 0 };
    sprintf(log, "%d:%d:%d - %d-%d-%d",
        hour, minute, second, day, month, year);
    return log;
}
DateTime DateTime::Now()
{
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    DateTime Nowdate;
    Nowdate.year = timeinfo->tm_year + 1900;
    Nowdate.month = timeinfo->tm_mon + 1;
    Nowdate.day = timeinfo->tm_mday;
    Nowdate.hour = timeinfo->tm_hour;
    Nowdate.minute = timeinfo->tm_min;
    Nowdate.second = timeinfo->tm_sec;
    return Nowdate;
}
using namespace std;

//int main()
//{
//    DateTime d = DateTime::Now();
//    return 0;
//}