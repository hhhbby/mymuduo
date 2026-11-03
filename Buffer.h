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
    
    std::string retrieveAllasString(size_t n);
    void retrieve(size_t n);
    void ensureSpace(size_t n);

    void append(const char* dat, size_t len);

    size_t readFd(int fd, int *savedErrno);
    size_t writeFd(int fd, int *savedErrno);
private:
    void makeSpace(size_t n);
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;
    
    std::vector<char> buffer_;
    size_t readIdx_;
    size_t writeIdx_;
};