#include "EventLoop.hpp"
#include "TCPServer.hpp"
#include "TCPConnection.hpp" 

#include <assert.h>
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
    auto name = std::to_string(nextConnID++);   
    auto connectionPtr = TCPConnection::create(loop_, this, std::move(sock), name);
    connections_[name] = connectionPtr;    
    connectionPtr->setConnectionCallback(connectionCallback_);
    connectionPtr->setMessageCallback(messageCallback_);
    connectionPtr->connectEstablished();   
}

void TCPServer::removeConnection(TCPConnectionPtr conn)
{
    assert(conn != nullptr);
    LOG(INFO) << "TCPServer::removeConnection - remove connection-" << conn->name();    
    assert(connections_.find(conn->name()) != connections_.end());
    connections_.erase(conn->name());

    loop_->queueInLoop([conn] ()
                       {
                           conn->connectDesytoyed();
                       });
}

void TCPServer::start()
{
    acceptor_.listen();
}
