#pragma once

#include <string>   
#include <vector>
#include <stddef.h>


class Buffer {
public:
    Buffer();
    size_t readableBytes();
    size_t writableBytes();
    size_t prependableBytes();

    char* peek();
    
    std::string retrieveAllasString();
    void retrieve(size_t n);
    
    void append(const char* data, size_t len);
    
    ssize_t readFd(int fd, int *savedErrno);
    ssize_t writeFd(int fd, int *savedErrno);

    void ensureSpace(size_t n);
private:
    void makeSpace(size_t n);
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;
    
    std::vector<char> buffer_;
    size_t readIdx_;
    size_t writeIdx_;
};