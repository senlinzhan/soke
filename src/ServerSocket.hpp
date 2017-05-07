#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "IPAddress.hpp"
#include <memory>

namespace soke
{        
    class Socket;
    
    class ServerSocket
    {
    public:
        ServerSocket(const IPAddress &addr);
        ~ServerSocket();   

        // disable the copy operations
        ServerSocket(const ServerSocket &) = delete;
        ServerSocket &operator=(const ServerSocket &) = delete;

        // enable the default move operations
        ServerSocket(ServerSocket &&) = default;
        ServerSocket &operator=(ServerSocket &&) = default;
        
        void listen();
        
        int fd() const;
        
        void shutdownWrite();
        
        std::unique_ptr<Socket> accept();
        
        // set TCP_NODELAY socket option
        void setTCPNoDelay();

        // set TCP_CORK socket option
        void setTCPCork();

        // unset TCP_CORK socket option
        void unsetTCPCork();
    private:
        int setSocketReuseAddr(int sockfd);
        
        static constexpr int BACKLOG = 128;
        IPAddress            addr_;
        int                  fd_;
    };
};

#endif /* SERVERSOCKET_H */
