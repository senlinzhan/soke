#include "EventLoop.hpp"
#include "TCPConnection.hpp"
#include <unistd.h>
#include <glog/logging.h>

using namespace soke;

TCPConnection::TCPConnection(EventLoop *loop, std::unique_ptr<Socket> socket)
    : loop_(loop), 
      socket_(std::move(socket)),
      channel_(loop_, socket_->fd())      
{
    channel_.setReadCallback([this] ()
                             {
                                 char buf[65535];
                                 ssize_t n = ::read(channel_.fd(), buf, sizeof(buf));
                                 if (messageCallback_)
                                 {
                                     messageCallback_(shared_from_this(), buf, n);
                                 }
                             });
} 

TCPConnection::~TCPConnection()
{  
    // loop_->removeChannel(&channel_);
}

void TCPConnection::setConnectionCallback(ConnectionCallback callback)
{
    connectionCallback_ = callback;
}

void TCPConnection::setMessageCallback(MessageCallback callback)
{
    messageCallback_ = callback;
}

void TCPConnection::connectEstablished()
{
    channel_.enableReading();
    if (connectionCallback_)
    {
        connectionCallback_(shared_from_this());
    }
}

const IPAddress &TCPConnection::addr() const
{ 
    return socket_->addr();
}
