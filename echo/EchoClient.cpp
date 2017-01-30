#include "../TCPClient.hpp"
#include "../IOUtils.hpp"

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

    static constexpr int MAXLINE = 8192;

    char sendline[MAXLINE], recvline[MAXLINE];

    char *ptr;
    Rio rio(client->sockfd());
    while ((ptr = fgets(sendline, MAXLINE, stdin)) != nullptr)
    {
        ssize_t n = writen(client->sockfd(), sendline, strlen(sendline));
        if (n == -1)
        {
            std::cerr << "error when write to network: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        n = rio.readline(recvline, MAXLINE);
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

    if (ferror(stdin))
    {
        std::cerr << "error when read from stdin: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "connection close by user input" << std::endl;
    }
    
    return 0;
}
