#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <string>

namespace soke
{
    class Buffer
    {
    public:
        static const size_t kCheapPrepend = 8;
        static const size_t kInitialSize  = 1024;
        
        Buffer();    

        size_t readableBytes() const;
        size_t writableBytes() const;
        size_t prependableBytes() const;
        void append(const char *data, size_t len);
        const char *peek() const;
        void retrieve(size_t len);
        std::string retrieveAsString();
        ssize_t readFd(int fd, int *savedErrno);
        
    private:
        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;
    };
};



#endif /* BUFFER_H */
