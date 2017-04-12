#include "IPAddress.hpp"

#include <glog/logging.h>
#include <utility>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>

using std::vector;

IPAddress::IPAddress(const std::string &host, uint16_t port)
    : host_(host), port_(port)
{
    struct addrinfo hints;
    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *result = nullptr;
    int ret = getaddrinfo(host.c_str(), NULL, &hints, &result);
    if (ret != 0)
    {
        LOG(ERROR) << "IPAddress::IPAddress - invalid argument host: " << host;
        return;        
    }

    if (result->ai_family == AF_INET)
    {
        type_ = Type::ipv4;
    }
    else if (result->ai_family == AF_INET6)
    {
        type_ = Type::ipv6;
    }
    else
    {
        LOG(ERROR) << "IPAddress::IPAddress - invalid argument host: " << host;
    }
}

IPAddress::IPAddress(const sockaddr_storage &addr)
{
    if (addr.ss_family == AF_INET)
    {
        type_ = Type::ipv4;
    }
    else if (addr.ss_family == AF_INET6)
    {
        type_ = Type::ipv6;
    }
    else
    {
        LOG(ERROR) << "IPAddress::IPAddress() - invalid argument addr";
        return;
    }
    
    const char *ptr = nullptr;

    size_t addrLen = isIPv4() ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;    
    vector<char> clientAddress(addrLen);    
    
    if (isIPv4())
    { 
        auto sockaddr = reinterpret_cast<sockaddr_in *>(const_cast<sockaddr_storage *>(&addr));	
	ptr = inet_ntop(AF_INET, &(sockaddr->sin_addr), clientAddress.data(), INET_ADDRSTRLEN);
	port_ = ntohs(sockaddr->sin_port);
    }
    else
    {
	auto sockaddr = reinterpret_cast<sockaddr_in6 *>(const_cast<sockaddr_storage *>(&addr));
	ptr = inet_ntop(AF_INET6, &(sockaddr->sin6_addr), clientAddress.data(), INET6_ADDRSTRLEN);
	port_ = ntohs(sockaddr->sin6_port);	
    }
    if (ptr == nullptr)
    {
        LOG(ERROR) << "IPAddress::IPAddress() - invalid ip address";
        return;
    }    
    host_ = std::string(ptr);
}

bool IPAddress::isIPv4() const
{
    return type_ == Type::ipv4;
}

bool IPAddress::isIPv6() const
{
    return type_ == Type::ipv6;
}

std::string IPAddress::toString() const
{
    return IPAddr() + ":" + std::to_string(port_);    
}

std::string IPAddress::IPAddr() const
{
    return host_;    
}

uint16_t IPAddress::port() const
{
    return port_;
}
