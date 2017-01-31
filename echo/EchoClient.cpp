#include "../TCPClient.hpp"
#include "../IOUtils.hpp"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <unordered_set>

int main(int argc, char *argv[])
{
    std::unique_ptr<TCPClient> client = nullptr;
    try {
        client.reset(new TCPClient("127.0.0.1", 9388));
    } catch (const TCPClientError &e) {
        std::cout << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    bool isStdinEOF = false;
    
    fd_set readSet;
    FD_ZERO(&readSet);    
    
    static constexpr int MAXLINE = 8192;
    char buf[MAXLINE];
    
    std::unordered_set<int> fds = {
        client->sockfd(),
        fileno(stdin)
    };
    
    while (true)
    {
        for (auto fd: fds )
        {
            FD_SET(fd, &readSet);
        }        
        int maxfd = std::max(fileno(stdin), client->sockfd()) + 1;
        int readNum = select(maxfd, &readSet, nullptr, nullptr, nullptr);
        if (readNum == -1)
        {
            std::cerr << "error when call select(): " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(client->sockfd(), &readSet))
        {
            ssize_t n;
            do {
                n = read(client->sockfd(), buf, MAXLINE);
            } while (n == -1 && errno == EINTR);
            
            if (n == -1)
            {
                std::cerr << "error when read from network: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (n == 0)
            {
                if (!isStdinEOF)
                {
                    std::cerr << "server terminated prematurely" << std::endl;
                    exit(EXIT_FAILURE);
                }
                std::cout << "connection close by server" << std::endl;
                exit(EXIT_SUCCESS);
            }
            n = writen(fileno(stdout), buf, n);
            if (n == -1)
            {
                std::cerr << "error when write to stdout" << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        if (FD_ISSET(fileno(stdin), &readSet))
        {
            ssize_t n;
            do {
                n = read(fileno(stdin), buf, MAXLINE);
            } while (n == -1 && errno == EINTR);

            if (n == -1)
            {
                std::cerr << "error when read from stdin: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);                    
            }
            else if (n == 0)
            {
                shutdown(client->sockfd(), SHUT_WR);
                isStdinEOF = true;
                FD_CLR(fileno(stdin), &readSet);
                fds.erase(fileno(stdin));
                std::cout << "connection close by user input" << std::endl;
                continue;                
            }
            n = writen(client->sockfd(), buf, n);
            if (n == -1)
            {
                std::cerr << "error when write to network: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    } 
    
    return 0;
}
