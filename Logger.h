#include <string>

#include "noncopyable.h"

enum class LoggerLevel {
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

#define LOG_INFO(fmt, ...) \
do {\
    char buf[1024]{}; \
    snprintf(buf, sizeof buf, fmt, ##__VA_ARGS__);  \
    Logger& logger = Logger::instance(); \
    logger.setLevel(LoggerLevel::INFO); \
    Logger::instance().log(buf);\
} while(0)

#define LOG_ERROR(fmt, ...) \
do {\
    char buf[1024]{}; \
    snprintf(buf, sizeof buf, fmt, ##__VA_ARGS__);  \
    Logger& logger = Logger::instance(); \
    logger.setLevel(LoggerLevel::ERROR); \
    Logger::instance().log(buf);\
} while(0)

#define LOG_FATAL(fmt, ...) \
do {\
    char buf[1024]{}; \
    snprintf(buf, sizeof buf, fmt, ##__VA_ARGS__);  \
    Logger& logger = Logger::instance(); \
    logger.setLevel(LoggerLevel::FATAL); \
    Logger::instance().log(buf); \
    exit(-1); \
} while(0)

#ifdef MUDEBUG
#define LOG_DEBUG(fmt, ...) \
do {\
    char buf[1024]{}; \
    snprintf(buf, sizeof buf, fmt, ##__VA_ARGS__);  \
    Logger& logger = Logger::instance(); \
    logger.setLevel(LoggerLevel::DEBUG); \
    Logger::instance().log(buf);\
} while(0)
#else
#define LOG_DEBUG(fmt, ...)
#endif

class Logger : noncopyable {
public:
    static Logger& instance();
    void setLevel(LoggerLevel level);
    void log(std::string msg);
private:
    LoggerLevel level_;
};