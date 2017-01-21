#include <unistd.h>
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
