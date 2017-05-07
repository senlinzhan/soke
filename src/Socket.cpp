#include "Socket.hpp"
#include <assert.h>
#include <glog/logging.h>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

using namespace soke;

Socket::Socket(int fd, const IPAddress &addr)
    : fd_(fd), addr_(addr)
{        
}
    
Socket::~Socket()
{
    LOG(INFO) << "close connection from " << addr().toString();
    ::close(fd_);
}
        
int Socket::fd() const
{
    return fd_;
}
    
const IPAddress &Socket::addr() const
{
    return addr_;
}
