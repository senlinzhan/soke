#include "TCPServer.hpp"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>

using std::string;
using std::tuple;
using std::shared_ptr;

TCPServer::TCPServer(const string &addr, unsigned short port)
    : addr_(addr), port_(port)
{
    struct addrinfo hints;

    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    auto service = std::to_string(port_);

    struct addrinfo *result = nullptr;
    int n = getaddrinfo(addr_.c_str(), service.c_str(), &hints, &result);
    if (n != 0 || result == nullptr)
    {
	throw TCPServerError("TCPServer::TCPServer() - invalid address: \"" +
			     addr_ + ":" + service + "\"", errno);
    }
    
    int sockfd;
    auto ptr = result;    
    for( ; ptr != nullptr; ptr = ptr->ai_next)
    {
	sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (sockfd < 0)
	{
	    continue;
	}
	if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == 0)
	{
	    break;
	}
	close(sockfd);
    }

    freeaddrinfo(result);
    
    if(ptr == nullptr)
    {
	throw TCPServerError("TCPServer::TCPServer() - invalid address: \"" +
			     addr_ + ":" + service + "\"", errno);	
    }

    sockfd_ = sockfd;

    if (listen(sockfd_, BACKLOG) == -1)
    {
        close(sockfd_);
        throw TCPServerError("TCPServer::TCPServer() - listen error", errno);
    }
}


shared_ptr<TCPConnection> TCPServer::accept()
{  
    sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    int clientSockfd = ::accept(sockfd_, reinterpret_cast<sockaddr *>(&addr), &len);
    if (clientSockfd == -1)
    {
        throw TCPServerError("TCPServer::accept() accept error", errno);
    }
    
    return std::make_shared<TCPConnection>(clientSockfd, IPAddress(addr));
}

TCPServer::~TCPServer()
{
    close(sockfd_);
}
