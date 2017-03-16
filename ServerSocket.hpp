#ifndef SOCKET_H
#define SOCKET_H

#include "Error.hpp"

#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


class ServerSocket
{
public:
    ServerSocket(const std::string &addr, uint16_t port)
        : addr_(addr),
          port_(port)
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
            throw SocketError("TCPServer::TCPServer() - invalid address: \"" +
                              addr_ + ":" + service + "\"", errno);
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
            if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == 0)
            {
                break;
            }
            close(sockfd);
        }

        freeaddrinfo(result);
        
        if(ptr == nullptr)
        {
            throw SocketError("TCPServer::TCPServer() - invalid address: \"" +
                                 addr_ + ":" + service + "\"", errno);	
        }
        
        fd_ = sockfd;

        if (::listen(fd_, BACKLOG) == -1)
        {
            ::close(fd_);
            throw SocketError("TCPServer::TCPServer() - listen error", errno);
        }       
    }

    ~ServerSocket()
    {
        ::close(fd_);
    }
    
    int fd() const
    {
        return fd_;
    }
   

private:
    static constexpr int BACKLOG = 128;
    std::string          addr_;
    uint16_t             port_;
    int                  fd_;    
};

#endif /* SOCKET_H */
