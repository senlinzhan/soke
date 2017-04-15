#include "TCPServer.hpp"
#include "TCPConnection.hpp" 
#include <glog/logging.h>

using namespace soke;

TCPServer::TCPServer(EventLoop *loop, const IPAddress &addr)
    : loop_(loop),
      acceptor_(loop, addr),
      nextConnID(1)
{
    acceptor_.setNewConnectionCallback([this](std::unique_ptr<Socket> sock)
                                       {
                                           newConnection(std::move(sock));
                                       });
}

void TCPServer::setConnectionCallback(ConnectionCallback callback)
{
    connectionCallback_ = callback;
}

void TCPServer::setMessageCallback(MessageCallback callback)
{
    messageCallback_ = callback;
}
  
void TCPServer::newConnection(std::unique_ptr<Socket> sock)
{
    auto connectionName = std::to_string(nextConnID++);
    auto connectionPtr = std::make_shared<TCPConnection>(loop_, std::move(sock));
    connections_[connectionName] = connectionPtr;
    
    connectionPtr->setConnectionCallback(connectionCallback_);
    connectionPtr->setMessageCallback(messageCallback_);
    connectionPtr->connectEstablished();   
}

void TCPServer::start()
{
    acceptor_.listen();
}
 
