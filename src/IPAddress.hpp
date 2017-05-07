#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <netinet/in.h>
#include <string>

namespace soke
{
    class IPAddress
    {
    public:
        enum Type { ipv4, ipv6 };
        
        IPAddress(const std::string &host, uint16_t port);
        IPAddress(const sockaddr_storage &addr);
    
        bool isIPv4() const;
        bool isIPv6() const;    
        
        std::string IPAddr() const;
        uint16_t port() const;
        
        std::string toString() const;    
        
    private:
        Type        type_;
        std::string host_;
        uint16_t    port_;
    };
}

#endif /* IPADDRESS_H */
