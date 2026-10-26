#include <time.h>

#include "TimeStamp.h"

TimeStamp TimeStamp::now()
{
    return TimeStamp(time(NULL));
}

std::string TimeStamp::toString()
{
    tm* tm = localtime(&second_);
    char buf[64]{};
    snprintf(buf, sizeof buf, "%4d/%02d/%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buf;
}

// #include <iostream>
// int main()
// {
//     std::cout << TimeStamp::now().toString() << std::endl;
// }