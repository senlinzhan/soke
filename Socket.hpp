#ifndef SOCKET_H
#define SOCKET_H

#include "IPAddress.hpp"

#include <assert.h>
#include <glog/logging.h>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

class Socket
{
public:
    Socket(int fd, const IPAddress &addr)
        : fd_(fd), addr_(addr)
    {        
    }
    
    ~Socket()
    {
        if (isValid())
        {
            ::close(fd_);
        }
    }

    Socket(Socket &&) = default;
    Socket &operator=(Socket &&) = default;
    
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    void close()
    {
        assert(isValid());
        ::close(fd_);
    }
    
    int fd() const
    {
        return fd_;
    }

    bool isValid() const
    {
        return fd_ != -1;
    }
    
    const IPAddress &addr() const
    {
        return addr_;
    }
    
private:
    int fd_;
    IPAddress addr_;
};

class ServerSocket
{
public:
    ServerSocket(const std::string &addr, uint16_t port)
        : addr_(addr), port_(port)
    {
        struct addrinfo hints;

        bzero(&hints, sizeof(hints));
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        auto service = std::to_string(port_);
        struct addrinfo *result = nullptr;
        int n = getaddrinfo(addr_.c_str(), service.c_str(), &hints, &result);
        if (n != 0 || result == nullptr)
        {
            LOG(FATAL) << "ServerSocket::ServerSocket() - invalid address: \""
                       << addr_ << ":" << service << "\", " << ::strerror(errno);
        }
    
        int sockfd;
        auto ptr = result;    
        for( ; ptr != nullptr; ptr = ptr->ai_next)
        {
            sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (sockfd < 0)
            {
                continue;
            }
            int status = setSocketReuseAddr(sockfd);
            if (status < 0)
            {
                LOG(FATAL) << "ServerSocket::ServerSocket() - error: "
                           << ::strerror(errno);
            }
            if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == 0)
            {
                break;
            }            
            close(sockfd);
        }

        freeaddrinfo(result);
        
        if(ptr == nullptr)
        {
            LOG(FATAL) << "ServerSocket::ServerSocket() - invalid address: \""
                       << addr_ << ":" << service << "\", " << ::strerror(errno);
        }
        
        fd_ = sockfd;

        if (::listen(fd_, BACKLOG) == -1)
        {
            ::close(fd_);
            LOG(FATAL) << "ServerSocket::ServerSocket() - listen error: "
                       << ::strerror(errno);
        }
    }

    ~ServerSocket()
    {
        ::close(fd_);
    }

    ServerSocket(ServerSocket &&) = default;
    ServerSocket &operator=(ServerSocket &&) = default;
    
    ServerSocket(const ServerSocket &) = delete;
    ServerSocket &operator=(const ServerSocket &) = delete;

    int fd() const
    {
        return fd_;
    }

    void shutdownWrite()
    {
        if (::shutdown(fd_, SHUT_WR) < 0)
        {
            LOG(ERROR) << "ServerSocket::shutdownWrite() - error: "
                       << ::strerror(errno); 
        }
    }

    Socket accept()
    {
        sockaddr_storage addr;
        socklen_t len = sizeof(addr);        
        int sockfd = ::accept4(sockfd, reinterpret_cast<sockaddr *>(&addr),
                               &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
        return Socket(sockfd, IPAddress(addr));
    }

    int setTCPNoDelay()
    {
        int on = 1;
        return ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    }
private:
    int setSocketReuseAddr()
    {
        int on = 1;
        return ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    }
    
    static constexpr int BACKLOG = 128;
    std::string          addr_;
    uint16_t             port_;
    int                  fd_;
};

#endif /* SOCKET_H */
