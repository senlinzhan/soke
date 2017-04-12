#include "Acceptor.hpp"

using namespace soke;

Acceptor::Acceptor(EventLoop *loop, const IPAddress &addr)
    :loop_(loop),
     socket_(addr),
     channel_(loop_, socket_.fd())
{
    // FIXME: ensure setNewConnectionCallback before listening
    channel_.setReadCallback([this] ()
                             {
                                 handleRead();
                             });
    channel_.enableReading();    
}

Acceptor::~Acceptor()
{    
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback callback)
{
    callback_ = callback;
}

void Acceptor::handleRead()
{
    auto clientSock = socket_.accept();
    if (clientSock != nullptr)
    {
        if (callback_)
        {
            callback_(clientSock->fd(), clientSock->addr());
        }
    }
}
