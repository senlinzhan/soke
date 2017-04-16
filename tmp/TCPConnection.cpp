#include "EventLoop.hpp"
#include "TCPConnection.hpp"
#include "TCPServer.hpp"

#include <unistd.h>
#include <glog/logging.h>

using namespace soke;
 
TCPConnection::TCPConnection(EventLoop *loop, TCPServer *server,
                             std::unique_ptr<Socket> socket, const std::string &name) 
    : loop_(loop),
      server_(server),
      socket_(std::move(socket)),
      name_(name),
      channel_(loop_, socket_->fd())      
{
    channel_.setReadCallback([this] ()
                             {
                                 handleRead();
                             });
} 

void TCPConnection::handleRead()
{
    char buf[65535];
    ssize_t n = ::read(channel_.fd(), buf, sizeof(buf));    
    if (n > 0)
    {
        if (messageCallback_)
        {
            messageCallback_(shared_from_this(), buf, n);
        }
    }
    else if (n == 0)
    {
        channel_.disableAll();
        server_->removeConnection(shared_from_this());
    }
    else
    {
        LOG(ERROR) << "TCPConnection::handleRead() - read error: " << strerror(errno);
    }
}

TCPConnection::~TCPConnection()
{  
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

void TCPConnection::connectDesytoyed()
{
    loop_->removeChannel(&channel_);
}

const IPAddress &TCPConnection::addr() const
{ 
    return socket_->addr();
}

const std::string &TCPConnection::name() const
{
    return name_;
}
