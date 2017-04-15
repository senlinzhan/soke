#ifndef SOCKET_H
#define SOCKET_H
 
#include "IPAddress.hpp"
#include <memory>

namespace soke
{
    class Socket
    {
    public:
        Socket(int fd, const IPAddress &addr);    
        ~Socket();

        // disable the copy operations
        Socket(const Socket &) = delete;
        Socket &operator=(const Socket &) = delete;

        // enable the default move operations
        Socket(Socket &&) = default;
        Socket &operator=(Socket &&) = default;
        
        int fd() const;        
        const IPAddress &addr() const;    

    private:
        int fd_;
        IPAddress addr_;
    };
};

#endif /* SOCKET_H */
