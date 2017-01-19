#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "IPAddress.hpp"
#include <string>

class TCPServer
{
public:
    TCPServer(const std::string &addr, unsigned short port);
    ~TCPServer();

    TCPServer(const TCPServer &) = delete;
    TCPServer &operator=(const TCPServer &) = delete;

    IPAddress accept();
    
private:
    static constexpr int BACKLOG = 128;
    std::string addr_;
    unsigned short port_;
    int sockfd_;
};



#endif /* TCPSERVER_H */
