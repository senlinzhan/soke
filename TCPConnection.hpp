#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "IPAddress.hpp"


class TCPConnection
{
public:
    TCPConnection(int sockfd, const IPAddress &addr);
    TCPConnection(int sockfd, IPAddress &&addr);

    TCPConnection(const TCPConnection &) = delete;
    TCPConnection &operator=(const TCPConnection &) = delete;   
    
    ~TCPConnection();
    
    void setNonBlock();
    void setTCPNoDelay();
    
    int sockfd() const;
    std::string address() const;
    
private:
    int sockfd_;
    IPAddress addr_;
};



#endif /* TCPCONNECTION_H */
o
