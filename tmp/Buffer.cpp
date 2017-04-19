#include "Buffer.hpp"

#include <algorithm>
#include <assert.h>
#include <sys/uio.h>
#include <errno.h>

using namespace soke;

Buffer::Buffer()
    : buffer_(kCheapPrepend + kInitialSize),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend)      
{    
}

size_t Buffer::readableBytes() const
{
    return writerIndex_ - readerIndex_;
}

size_t Buffer::writableBytes() const
{
    return buffer_.size() - writerIndex_;
}

size_t Buffer::prependableBytes() const
{
    return readerIndex_;
}

void Buffer::append(const char *data, size_t len)
{
    if (writableBytes() < len)
    {
        if (prependableBytes() + writableBytes() < kCheapPrepend + len)
        {
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            size_t readable = readableBytes();
            std::copy(buffer_.data() + readerIndex_,
                      buffer_.data() + writerIndex_,
                      buffer_.data() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;            
        }
    }
    std::copy(data, data + len, buffer_.data() + writerIndex_);
    writerIndex_ += len;
}

const char *Buffer::peek() const
{
    return buffer_.data() + readerIndex_;
}

void Buffer::retrieve(size_t len)
{
    assert(len <= readableBytes());
    readerIndex_ += len;
}

std::string Buffer::retrieveAsString()
{
    auto str = std::string(peek(), readableBytes());
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
    return str;
}


ssize_t Buffer::readFd(int fd, int *savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const ssize_t writable = writableBytes();
    vec[0].iov_base = buffer_.data() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    const ssize_t n = readv(fd, vec, 2);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (n <= writable)
    {
        writerIndex_ += n;
    }
    else
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}
