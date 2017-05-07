#ifndef IOUTILS_H
#define IOUTILS_H

#include <unistd.h>
#include <errno.h>
#include <algorithm>

ssize_t readn(int fd, void *buf, size_t count)
{
    ssize_t readCount = 0;
    
    while (count > 0)
    {
        auto n = read(fd, static_cast<char *>(buf) + readCount, count);
        if (n == 0)
        {
            break;
        }
        else if (n == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }  
        }
        readCount += n;
        count -= n;
    }
    
    return readCount;
}


ssize_t writen(int fd, const void *buf, size_t count)
{
    ssize_t writeCount = 0;
    
    while (count > 0)
    {
        auto n = write(fd, static_cast<const char *>(buf) + writeCount, count);
        if (n <= 0)
        {
            if (n == -1 && errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        count -= n;
        writeCount += n;
    }
    
    return writeCount;
}



class Rio
{
public:
    Rio(int fd)
        : fd_(fd), unreadBytes_(0)
    {
        pos_ = buf_;
    }

    Rio(const Rio &) = delete;
    Rio &operator=(const Rio &) = delete;    
    
    ssize_t read(char *userBuff, size_t n)
    {
        while (unreadBytes_ <= 0)
        {
            unreadBytes_ = ::read(fd_, buf_, BUFF_SIZE);
            if (unreadBytes_ == -1)
            {
                if (errno != EINTR)
                {
                    return -1;
                }
                else
                {
                    continue;
                }
            }

            if (unreadBytes_ == 0)
            {
                return 0;
            }
            pos_ = buf_;            
        }
        
        int count = std::min(static_cast<int>(n), unreadBytes_);
        memcpy(userBuff, pos_, count);
        unreadBytes_ -= count;
        pos_ += count;

        return count;
    }
    
    ssize_t readline(void *userBuff, size_t maxLen)
    {
        char *ptr = static_cast<char *>(userBuff);
        
        int i;      
        for (i = 0; i < maxLen - 1; i++)
        {
            char c;        
            int n = read(&c, 1);
            if (n == -1)
            {
                return -1;
            }
            else if (n == 0)
            {
                break;
            }            
            *ptr++ = c;
            if (c == '\n') {
                break;
            }
        }

        *ptr = '\0';
        return i;
    }

    
private:
    static constexpr size_t BUFF_SIZE = 8192;
    
    int fd_;
    int unreadBytes_;    
    char *pos_;
    char buf_[BUFF_SIZE];
};




#endif
