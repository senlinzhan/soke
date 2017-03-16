#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <stdexcept>
#include <errno.h>
#include <string.h>

class SocketError : public std::exception
{
public:
    explicit SocketError(const std::string &message, int error_code = 0)
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

#endif /* ERROR_H */
