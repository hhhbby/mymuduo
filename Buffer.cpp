#include <algorithm>
#include <assert.h>
#include <sys/uio.h>
#include <unistd.h>

#include "Buffer.h"

Buffer::Buffer() : buffer_(kInitialSize, '0'),
    readIdx_(kCheapPrepend),
    writeIdx_(kCheapPrepend)
{
}

size_t Buffer::readableBytes()
{
    return writeIdx_ - readIdx_;
}

size_t Buffer::writableBytes()
{
    return buffer_.size() - writeIdx_;
}

size_t Buffer::prependableBytes()
{
    return readIdx_;
}


char* Buffer::peek()
{
    return buffer_.data() + readIdx_;
}


std::string Buffer::retrieveAllasString()
{
    char* start = peek();
    size_t n = readableBytes();
    retrieve(n);
    return std::string(start, n);
}

void Buffer::retrieve(size_t n)
{
    assert(n <= readableBytes());
    readIdx_ += n;
}


void Buffer::append(const char* data, size_t len)
{
    ensureSpace(len);
    std::copy(data, data + len, peek() + writeIdx_);
    writeIdx_ += len;
}


ssize_t Buffer::readFd(int fd, int *savedErrno)
{
    char extrabuf[65536]{};
    iovec vec[2]{};
    vec[0].iov_base = peek() + writeIdx_;
    vec[0].iov_len = writableBytes();
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    // add () to confirm the meaning
    int iovcnt = (writableBytes() < sizeof extrabuf) ? 2 : 1;
    ssize_t n = ::readv(fd, vec, iovcnt);
    // It's easy to forget to update the writeIdx_.
    if (n == -1)
    {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writableBytes()) {
        writeIdx_ += n;
    } else {
        writeIdx_ = buffer_.size();
        append(extrabuf, n - writableBytes());
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int *savedErrno)
{
    int n = ::write(fd, peek(), readableBytes());
    if (n == -1)
    {
        *savedErrno = errno;
    } else {
        writeIdx_ += n;
    }
    return n;
}

void Buffer::ensureSpace(size_t n)
{
    if (n <= writableBytes())
        return;
    makeSpace(n);
}

void Buffer::makeSpace(size_t n)
{
    if (kCheapPrepend + n <= writableBytes() + readIdx_)
    {
        std::copy(peek(), peek() + readableBytes(), peek() + kCheapPrepend);
        writeIdx_ = kCheapPrepend + readableBytes();
        readIdx_ = kCheapPrepend;
    } else {
        buffer_.resize(n + readableBytes());
    }
}
