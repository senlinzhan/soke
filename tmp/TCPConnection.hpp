#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Callback.hpp"
#include "Channel.hpp"
#include "IPAddress.hpp"
#include "Socket.hpp"
#include <memory>

namespace soke
{
    class EventLoop;
    
    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    public:
        TCPConnection(EventLoop *loop, std::shared_ptr<Socket> socket);
        ~TCPConnection();

        // disable the copy operations
        TCPConnection(const TCPConnection &) = delete;
        TCPConnection &operator=(const TCPConnection &) = delete;

        void setConnectionCallback(ConnectionCallback callback);
        void setMessageCallback(MessageCallback callback);
        void connectEstablished();

        const IPAddress &addr() const;
        
    private:
        EventLoop               *loop_;
        std::shared_ptr<Socket>  socket_;
        Channel                  channel_;
        ConnectionCallback       connectionCallback_;
        MessageCallback          messageCallback_;
    };    
};

#endif /* TCPCONNECTION_H */
