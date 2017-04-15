#ifndef TCPSERVER_H
#define TCPSERVER_H
 
#include "IPAddress.hpp"
#include "Callback.hpp"
#include "Acceptor.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
  
namespace soke
{
    class EventLoop;
    class TCPConnection;
    class Socket;
    
    class TCPServer
    {
    public:
        TCPServer(EventLoop *loop, const IPAddress &addr);
        ~TCPServer() = default;

        // disable the copy operations
        TCPServer(const TCPServer &) = delete;
        TCPServer &operator=(const TCPServer &) = delete;

        void setConnectionCallback(ConnectionCallback callback);
        void setMessageCallback(MessageCallback callback);
        void start();
        
    private: 
        //void newConnection(int sockfd, const IPAddress &addr);
        void newConnection(std::unique_ptr<Socket> socket);
        
        EventLoop *loop_;
        Acceptor   acceptor_;
        int        nextConnID;
        
        std::unordered_map<std::string, TCPConnectionPtr> connections_;
        ConnectionCallback connectionCallback_;
        MessageCallback    messageCallback_;
    };
}; 

#endif /* TCPSERVER_H */
