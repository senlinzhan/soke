#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "IPAddress.hpp"
#include "TCPConnection.hpp"

#include <memory>
#include <string>
#include <string.h>

class TCPServer
{
public:
    TCPServer(const std::string &addr, unsigned short port);
    ~TCPServer();

    TCPServer(const TCPServer &) = delete;
    TCPServer &operator=(const TCPServer &) = delete;

    std::shared_ptr<TCPConnection> accept();
    
private:
    static constexpr int BACKLOG = 128;
    std::string addr_;
    unsigned short port_;
    int sockfd_;
};

class TCPServerError : public std::exception
{
public:
    explicit TCPServerError(const std::string &message, int error_code = 0)
	: message_(message), error_code_(error_code)
    {
	if (has_error_code())
	{
	    message_ = message_ + ": " + ::strerror(error_code_) +
		", error code: " + std::to_string(error_code_);
	}
    }

    virtual const char * what() const noexcept override 
    {
        return message_.c_str();
    }    

    bool has_error_code() const
    {
	return error_code_ != 0;
    }
    
    int error_code() const
    {
	return error_code_;
    }
    
private:
    std::string message_;
    int error_code_;
};


#endif /* TCPSERVER_H */
