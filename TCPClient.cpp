#include "TCPClient.hpp"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>

using std::string;

TCPClient::TCPClient(const string &host, unsigned short port)
{
    struct addrinfo hints;

    bzero(&hints, sizeof(hints)); 
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    auto service = std::to_string(port);

    struct addrinfo *result = nullptr;
    int n = getaddrinfo(host.c_str(), service.c_str(), &hints, &result);
    if (n != 0 || result == nullptr)
    {
        throw TCPClientError("TCPClient::TCPClient() - invalid address: \"" +
                             host + ":" + service + "\"", errno);
    }

    
    int sockfd;
    auto ptr = result;
    for(; ptr != nullptr; ptr = ptr->ai_next)
    {
        sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sockfd < 0 )
        {
            continue;
        }
        if (connect(sockfd, ptr->ai_addr, ptr->ai_addrlen) == 0)
        {
            break;
        }
        close(sockfd);
    }

    freeaddrinfo(result);
    if (ptr == nullptr)
    {
        throw TCPClientError("TCPClient::TCPClient() - invalid address:\"" +
                             host + ":" + service + "\"", errno);            
    }
    sockfd_ = sockfd;
}


TCPClient::~TCPClient()
{
    close(sockfd_);
}
