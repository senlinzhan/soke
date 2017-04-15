#include "Acceptor.hpp"
#include "Socket.hpp"

using namespace soke;

Acceptor::Acceptor(EventLoop *loop, const IPAddress &addr)
    :loop_(loop),
     serverSocket_(addr),
     channel_(loop_, serverSocket_.fd())
{
    channel_.setReadCallback([this] ()
                             {
                                 handleRead();
                             });
}

void Acceptor::listen()
{
    serverSocket_.listen();
    channel_.enableReading();
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback callback)
{
    newConncallback_ = callback;
}

void Acceptor::handleRead()
{
    auto clientSock = serverSocket_.accept();
    if (clientSock != nullptr)
    {
        if (newConncallback_) 
        {    
            newConncallback_(clientSock);
        }
    }
}
