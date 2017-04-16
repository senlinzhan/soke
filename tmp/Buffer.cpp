#include "Buffer.hpp"

#include <algorithm>
#include <assert.h>

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
