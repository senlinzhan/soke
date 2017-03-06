#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include "TCPConnection.hpp"

TCPConnection::TCPConnection(int sockfd, const IPAddress &addr)
    : sockfd_(sockfd), addr_(addr)
{    
}

TCPConnection::TCPConnection(int sockfd, IPAddress &&addr)
    : sockfd_(sockfd), addr_(std::move(addr))
{    
}

TCPConnection::~TCPConnection()
{
    close(sockfd_);
}

int TCPConnection::sockfd() const
{
    return sockfd_;
}

std::string TCPConnection::address() const
{
    return addr_.toString();
}


void TCPConnection::setNonBlock()
{ 
    int flags = fcntl(sockfd_, F_GETFL, 0);
    if (flags != -1)
    {
        fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK);        
    }
}


void TCPConnection::setTCPNoDelay()
{
    int optval = 1;

    // FIXME: add error checking
    setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
}


