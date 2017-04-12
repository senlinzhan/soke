#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "IPAddress.hpp"
#include "Socket.hpp"
#include "Channel.hpp"

#include <functional>
#include <string>

namespace soke
{
    class EventLoop;
    
    class Acceptor
    {
    public:
        using NewConnectionCallback = std::function<void (int, const IPAddress &)>;
        
        Acceptor(EventLoop *loop, const IPAddress &addr);
        ~Acceptor();

        Acceptor(const Acceptor &) = delete;
        Acceptor &operator=(const Acceptor &) = delete;

        void setNewConnectionCallback(NewConnectionCallback callback);
        void handleRead();
        
    private:
        EventLoop            *loop_;
        ServerSocket          socket_;
        Channel               channel_;        
        NewConnectionCallback callback_;
    };
}

#endif /* ACCEPTOR_H */
