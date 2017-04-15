#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "IPAddress.hpp"
#include "ServerSocket.hpp"
#include "Channel.hpp"

#include <functional>
#include <string>

namespace soke
{
    class EventLoop;
    
    class Acceptor
    {
    public:
        using SocketPtr = std::shared_ptr<Socket>;
        using NewConnectionCallback = std::function<void (SocketPtr)>;
        
        Acceptor(EventLoop *loop, const IPAddress &addr);
        ~Acceptor() = default;

        // disable the copy operations
        Acceptor(const Acceptor &) = delete;
        Acceptor &operator=(const Acceptor &) = delete;

        void setNewConnectionCallback(NewConnectionCallback callback);
        void listen();
        
    private:
        void handleRead();
        
        EventLoop            *loop_;
        ServerSocket          serverSocket_;
        Channel               channel_;        
        NewConnectionCallback newConncallback_;
    };
};

#endif /* ACCEPTOR_H */
