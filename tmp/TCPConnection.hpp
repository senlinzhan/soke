#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Callback.hpp"
#include "Channel.hpp"
#include "IPAddress.hpp"
#include "Socket.hpp"

#include <memory>
#include <string>

namespace soke
{ 
    class EventLoop;
    
    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    public:
        // for internal use, user should not use this function directly
        TCPConnection(EventLoop *loop, std::unique_ptr<Socket> socket, const std::string &name);

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

        const IPAddress &addr() const;
        const std::string &name() const;
        
    private:
        EventLoop               *loop_;
        std::unique_ptr<Socket>  socket_;
        std::string              name_;
        Channel                  channel_;
        ConnectionCallback       connectionCallback_;
        MessageCallback          messageCallback_;
    };    
};

#endif /* TCPCONNECTION_H */
