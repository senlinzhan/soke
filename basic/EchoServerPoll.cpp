#include "../TCPServer.hpp"
#include "../IOUtils.hpp"

#include <cstdlib>
#include <memory>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <set>
#include <map>
#include <vector>
#include <poll.h>

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
    
    std::vector<pollfd> clients = {
        pollfd {
            .fd = server->sockfd(),
            .events = POLLIN
        }
    };
    std::map<int, std::shared_ptr<TCPConnection>> connections;
    
    while (true)
    {
        int readyNum = poll(clients.data(), clients.size(), -1);
        if (readyNum == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            std::cerr << "error when call poll(): " << std::endl;
            exit(EXIT_FAILURE);
        }
        if (clients[0].revents & POLLIN)
        {
            std::shared_ptr<TCPConnection> conn;
            try {
                conn = server->accept();
                std::cout << "accept new connection[" << conn->address() << "]" << std::endl;           
            } catch (const TCPServerError &e) {
                std::cerr << e.what() << std::endl;
                continue;
            }
            int i = 1;
            for (; i < clients.size(); ++i)
            {
                if (clients[i].fd == -1)
                {
                    clients[i].fd = conn->sockfd();
                    break;
                }
            }
            if (i == clients.size())
            {
                clients.push_back(pollfd{.fd = conn->sockfd(), .events = POLLIN});                
            }            
            connections[conn->sockfd()] = conn;
            if (--readyNum <= 0)
            {
                continue;
            }
        }

        for (int i = 1; i < clients.size(); ++i)
        {
            int fd = clients[i].fd;
            if (fd < 0) {
                continue;
            }
            if (clients[i].revents & (POLLIN | POLLERR))
            {
                ssize_t n;
                do {
                    n = read(fd, buf, MAXLINE);
                } while (n == -1 && errno == EINTR);                

                if (n > 0)
                {
                    if (writen(fd, buf, n) == -1)
                    {                        
                        std::cerr << "error when write to socket-" << fd << ": " << strerror(errno) << std::endl;
                        std::cerr << "socket-" << fd << " close by server" << std::endl;
                        clients[i].fd = -1;
                        connections.erase(fd);
                    }
                }
                else
                {
                    if (n == 0)
                    {
                        std::cerr << "connection close by client[" << connections[fd]->address() << "]" << std::endl;
                    }
                    else
                    {
                        std::cerr << "error when read from socket-" << fd << ": " << strerror(errno) << std::endl;
                    }
                    std::cerr << "socket-" << fd << " close by server" << std::endl;
                    clients[i].fd = -1;
                    connections.erase(fd);
                }
                if (--readyNum <= 0)
                {
                    break;
                }
            }
        }
    }
        
    return 0;
}

