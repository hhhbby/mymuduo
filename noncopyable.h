#pragma once

class noncopyable {
protected:
    noncopyable() = default;
    ~noncopyable() = default;
    
public:
    noncopyable& operator=(const noncopyable&) = delete;
    noncopyable(const noncopyable&) = delete;
};