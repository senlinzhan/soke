#include "../TCPClient.hpp"
#include "../IOUtils.hpp"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <memory>

int main(int argc, char *argv[])
{
    std::unique_ptr<TCPClient> client = nullptr;
    try {
        client.reset(new TCPClient("127.0.0.1", 9388));
    } catch (const TCPClientError &e) {
        std::cout << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    fd_set readSet;
    FD_ZERO(&readSet);    
    
    static constexpr int MAXLINE = 8192;
    char sendline[MAXLINE], recvline[MAXLINE];

    Rio rio(client->sockfd());
    while (true)
    {
        FD_SET(fileno(stdin), &readSet);
        FD_SET(client->sockfd(), &readSet);
        
        int maxfd = std::max(fileno(stdin), client->sockfd()) + 1;
        int readNum = select(maxfd, &readSet, nullptr, nullptr, nullptr);
        if (readNum == -1)
        {
            std::cerr << "error when call select(): " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(client->sockfd(), &readSet))
        {
            ssize_t n = rio.readline(recvline, MAXLINE);
            if (n == -1)
            {
                std::cerr << "error when read from network: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (n == 0)
            {
                std::cerr << "connection close by server" << std::endl;
                exit(EXIT_FAILURE);
            }       
            if (fputs(recvline, stdout) == EOF)
            {
                std::cerr << "error when write to stdout" << std::endl;
                exit(EXIT_FAILURE);
            }            
        }

        if (FD_ISSET(fileno(stdin), &readSet))
        {
            auto ptr = fgets(sendline, MAXLINE, stdin);
            if (ptr == nullptr)
            {
                if (ferror(stdin))
                {
                    std::cerr << "error when read from stdin: " << strerror(errno) << std::endl;
                    exit(EXIT_FAILURE);
                }
                else
                {
                    std::cout << "connection close by user input" << std::endl;
                    exit(EXIT_SUCCESS);
                }                
            }
            ssize_t n = writen(client->sockfd(), sendline, strlen(sendline));
            if (n == -1)
            {
                std::cerr << "error when write to network: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    } 
    
    return 0;
}
