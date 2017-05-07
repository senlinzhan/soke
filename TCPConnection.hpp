#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Buffer.hpp"
#include "Callback.hpp"
#include "Channel.hpp"
#include "IPAddress.hpp"
#include "Socket.hpp"

#include <memory>
#include <string>

namespace soke
{ 
    class EventLoop;
    class TCPServer;
    
    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    public:
        // for internal use, user should not use this function directly
        TCPConnection(EventLoop *loop, TCPServer *server,
                      std::unique_ptr<Socket> socket, const std::string &name);

        // create TCPConnectionPtr
        template <typename... Args>
        static TCPConnectionPtr create(Args&&... args)
        {
            return std::make_shared<TCPConnection>(std::forward<Args>(args)...);
        }
        
        ~TCPConnection();

        // disable the copy operations
        TCPConnection(const TCPConnection &) = delete;
        TCPConnection &operator=(const TCPConnection &) = delete;

        void setConnectionCallback(ConnectionCallback callback);
        void setMessageCallback(MessageCallback callback);
        void connectEstablished();
        void connectDesytoyed();
        
        const IPAddress &addr() const;
        const std::string &name() const;

        void send(const std::string &message);
        void handleWrite();
    private:
        void handleRead();
        
        EventLoop               *loop_;
        TCPServer               *server_;
        std::unique_ptr<Socket>  socket_;
        std::string              name_;
        Channel                  channel_;
        ConnectionCallback       connectionCallback_;
        MessageCallback          messageCallback_;
        Buffer                   inputBuffer_;
        Buffer                   outputBuffer_;
    };    
};

#endif /* TCPCONNECTION_H */
