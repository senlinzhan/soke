#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>

class TCPClient
{
public:
    TCPClient(const std::string &host, unsigned short port);
    ~TCPClient();

    TCPClient(const TCPClient &) = delete;
    TCPClient &operator=(const TCPClient &) = delete;

private:
    int sockfd_;
};


class TCPClientError : public std::exception
{
public:
    explicit TCPClientError(const std::string &message, int error_code = 0)
	: message_(message), error_code_(error_code)
    {
	if (has_error_code())
	{
	    message_ = message_ + ": " + std::strerror(error_code_) +
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




#endif /* TCPCLIENT_H */
