#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <netinet/in.h>
#include <string>
#include <exception>

class IPAddressFormatError : public std::exception
{
public:
    IPAddressFormatError(const std::string &msg)
        : msg_(msg)
    {	
    }
    
    IPAddressFormatError(std::string &&msg) noexcept
        : msg_(std::move(msg))
    {	
    }
    
    virtual const char * what() const noexcept override    
    {        
        return msg_.c_str();        
    }
    
private:
    std::string msg_;
};


class IPAddress
{
public:
    IPAddress(const sockaddr_storage &addr);
    
    bool isIPv4() const;
    bool isIPv6() const;    

    size_t size() const;    

    std::string IPAddr() const;
    std::string port() const;    

    std::string toString() const;    

private:
    static constexpr size_t size_ = sizeof(sockaddr_storage);
    
    sockaddr_storage addr_;
    std::string ip_;
    std::string port_;    
};


#endif /* IPADDRESS_H */
 
