#include "Socket.hpp"
#include "ServerSocket.hpp"
#include <assert.h>
#include <string>
#include <errno.h>
#include <glog/logging.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

using namespace soke;
 
ServerSocket::ServerSocket(const IPAddress &addr)
    : addr_(addr)
{
    struct addrinfo hints;

    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    auto service = std::to_string(addr_.port());
    struct addrinfo *result = nullptr;
    int n = getaddrinfo(addr_.IPAddr().c_str(), service.c_str(), &hints, &result);
    if (n != 0 || result == nullptr)
    {
        LOG(FATAL) << "ServerSocket::ServerSocket() - invalid address: \""
                   << addr_.toString() << "\", " << ::strerror(errno);
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
                   << addr_.toString() << "\", " << ::strerror(errno);
    }
    
    fd_ = sockfd;    
}

ServerSocket::~ServerSocket()
{
    ::close(fd_);
}

void ServerSocket::listen()
{
    if (::listen(fd_, BACKLOG) == -1)
    {
        ::close(fd_);
        LOG(FATAL) << "ServerSocket::ServerSocket() - listen error: "
                   << ::strerror(errno);
    }        
}

int ServerSocket::fd() const
{
    return fd_;
}
 
int ServerSocket::setSocketReuseAddr(int sockfd)
{
    int on = 1;
    return ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

std::unique_ptr<Socket> ServerSocket::accept()
{         
    sockaddr_storage addr;
    socklen_t len = sizeof(addr);        
    int sockfd = ::accept4(fd_, reinterpret_cast<sockaddr *>(&addr),
                           &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    
    if (sockfd == -1)
    {
        LOG(ERROR) << "accept connection failed";
        return nullptr;
    }
    return std::unique_ptr<Socket>(new Socket(sockfd, IPAddress(addr)));
}

void ServerSocket::shutdownWrite()
{
    if (::shutdown(fd_, SHUT_WR) < 0)
    {
        LOG(ERROR) << "ServerSocket::shutdownWrite() - error: "
                   << ::strerror(errno); 
    }
}

void ServerSocket::setTCPNoDelay()
{ 
    int state = 1;
    if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &state, sizeof(state)) == -1)
    {
        LOG(ERROR) << "ServerSocket::setTCPNoDelay() - error: "
                   << ::strerror(errno);                
    }
}

void ServerSocket::setTCPCork()
{
    int state = 1;
    if (setsockopt(fd_, IPPROTO_TCP, TCP_CORK, &state, sizeof(state)) == -1)
    {       
        LOG(ERROR) << "ServerSocket::setTCPCork() - error: "
                   << ::strerror(errno);        
    }
}

void ServerSocket::unsetTCPCork()
{
    int state = 0;
    if (setsockopt(fd_, IPPROTO_TCP, TCP_CORK, &state, sizeof(state)) == -1)
    {
        LOG(ERROR) << "ServerSocket::unsetTCPCork() - error: "
                   << ::strerror(errno);        
    }
}
