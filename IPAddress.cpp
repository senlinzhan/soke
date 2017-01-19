#include "IPAddress.hpp"

#include <utility>
#include <arpa/inet.h>
#include <vector>

using std::vector;

IPAddress::IPAddress(const sockaddr_storage &addr)
    : addr_(addr)
{
    unsigned short port;    
    const char *ptr = nullptr;

    size_t addrLen = isIPv4() ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;    
    vector<char> clientAddress(addrLen);    
    
    if (isIPv4())
    {
        auto addr = reinterpret_cast<sockaddr_in *>(const_cast<sockaddr_storage *>(&addr_));	
	ptr = inet_ntop(AF_INET, &(addr->sin_addr), &clientAddress[0], INET_ADDRSTRLEN);
	port = ntohs(addr->sin_port);
    }
    else
    {
	auto addr = reinterpret_cast<sockaddr_in6 *>(const_cast<sockaddr_storage *>(&addr_));
	ptr = inet_ntop(AF_INET6, &(addr->sin6_addr), &clientAddress[0], INET6_ADDRSTRLEN);
	port = ntohs(addr->sin6_port);	
    }

    if (ptr == nullptr)
    {
	throw IPAddressFormatError("the format of ip address is invalid");	
    }
    
    ip_ = std::string(ptr);
    port_ = std::to_string(port);    
}


bool IPAddress::isIPv4() const
{
    return addr_.ss_family == AF_INET;    
}

bool IPAddress::isIPv6() const
{
    return addr_.ss_family == AF_INET6;    
}


size_t IPAddress::size() const
{
  
    return size_;    
}

std::string IPAddress::toString() const
{
    return IPAddr() + ":" + port();    
}

std::string IPAddress::IPAddr() const
{
    return ip_;    
}

std::string IPAddress::port() const
{
    return port_;
}
