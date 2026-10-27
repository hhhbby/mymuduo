#include "Logger.h"
#include "TimeStamp.h"

#include <iostream>

// 获取日志唯一的实例对象
Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLevel(LoggerLevel level)
{
    level_ = level;
}

// 写日志  [级别信息] time : msg
void Logger::log(std::string msg)
{
    switch (level_)
    {
    case LoggerLevel::INFO:
        std::cout << "[INFO]";
        break;
    case LoggerLevel::ERROR:
        std::cout << "[ERROR]";
        break;
    case LoggerLevel::FATAL:
        std::cout << "[FATAL]";
        break;
    case LoggerLevel::DEBUG:
        std::cout << "[DEBUG]";
        break;
    default:
        break;
    }

    // 打印时间和msg
    std::cout << TimeStamp::now().toString() << " : " << msg << std::endl;
}