#include <string>
#include <stdint.h>

class TimeStamp {
public:
    TimeStamp() = default;
    TimeStamp(int second):second_(second) {};
    static TimeStamp now();
    std::string toString();
private:
    int64_t second_;
};