#include "../TCPServer.hpp"
#include "../IOUtils.hpp"

#include <cstdlib>
#include <memory>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <set>
#include <map>

int main(int argc, char *argv[])
{
    std::unique_ptr<TCPServer> server = nullptr;
    try {
        server.reset(new TCPServer("0.0.0.0", 9388));            
    } catch (TCPServerError &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    static constexpr int MAXLINE = 8192;
    char buf[MAXLINE];
    
    fd_set allSet;
    FD_ZERO(&allSet);
    FD_SET(server->sockfd(), &allSet);
    int maxfd = server->sockfd();

    std::map<int, std::shared_ptr<TCPConnection>> connections;
    
    while (true)
    {
        fd_set readSet = allSet;
        int readNum = select(maxfd + 1, &readSet, nullptr, nullptr, nullptr);
        if (readNum == -1)
        {
            std::cerr << "error when call select(): " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server->sockfd(), &readSet))
        {
            std::shared_ptr<TCPConnection> conn;
            try {
                conn = server->accept();
                std::cout << "accept new connection[" << conn->address() << "]" << std::endl;           
            } catch (const TCPServerError &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
            if (conn->sockfd() >= FD_SETSIZE)
            {
                std::cerr << "too many clients" << std::endl;
                exit(EXIT_FAILURE);
            }
            connections[conn->sockfd()] = conn;            
            FD_SET(conn->sockfd(), &allSet);
            maxfd = std::max(maxfd, conn->sockfd());
            if (--readNum <= 0)
            {
                continue;
            }
        }

        for (auto elem: connections)
        {
            auto fd = elem.first;
            auto conn = elem.second;
            if (FD_ISSET(fd, &readSet))
            {
                ssize_t n;
                do {
                    n = read(fd, buf, MAXLINE);
                } while (n == -1 && errno == EINTR);                                
                if (n == -1)
                {
                    std::cerr << "error when read from socket " << fd << ": " << strerror(errno) << std::endl;
                }
                else if (n == 0)
                {
                    std::cerr << "connection close by client[" << conn->address() << "]" << std::endl;
                    connections.erase(fd);
                    FD_CLR(fd, &allSet);
                }
                else
                {
                    if (writen(fd, buf, n) < 0)
                    {
                        std::cerr << "error when write to socket " << fd << ": " << strerror(errno) << std::endl;
                    }
                }
                if (--readNum <= 0)
                {
                    break;
                }
            }
        }
    }
    
    return 0;
}

