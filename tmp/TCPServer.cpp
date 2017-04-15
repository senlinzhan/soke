#include "TCPServer.hpp"
#include "TCPConnection.hpp" 
#include <glog/logging.h>

using namespace soke;

TCPServer::TCPServer(EventLoop *loop, const IPAddress &addr)
    : loop_(loop),
      acceptor_(loop, addr),
      nextConnID(1)
{
    acceptor_.setNewConnectionCallback([this](std::shared_ptr<Socket> sock)
                                       {
                                           newConnection(sock);
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
  
void TCPServer::newConnection(std::shared_ptr<Socket> socket)
{
    auto connectionName = std::to_string(nextConnID++);
    auto connectionPtr = std::make_shared<TCPConnection>(loop_, socket);
    connections_[connectionName] = connectionPtr;
    
    connectionPtr->setConnectionCallback(connectionCallback_);
    connectionPtr->setMessageCallback(messageCallback_);
    connectionPtr->connectEstablished();   
}

void TCPServer::start()
{
    acceptor_.listen();
}
 
