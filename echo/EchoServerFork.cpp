#include "../TCPServer.hpp"
#include "../IOUtils.hpp"

#include <cstdlib>
#include <memory>
#include <iostream>
#include <unistd.h>

void handleClient(std::shared_ptr<TCPConnection> conn)    
{
    static constexpr int MAXLINE = 8196;

    int sockfd = conn->sockfd();    
    char buf[MAXLINE];

    ssize_t n;

    while (true) {
        n = read(sockfd, buf, MAXLINE);
        if (n == 0) {
            std::cerr << "connection close by client[" << conn->address() << "]" << std::endl;
            return;
        }
        if (n > 0) {
            if (writen(sockfd, buf, n) == -1) {
                std::cerr << "handleClient(): write error - " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }            
        } else {
            if (n == -1 && errno != EINTR) {
                std::cerr << "handleClient(): read error - " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }        
    }
}

int main(int argc, char *argv[])
{
    std::unique_ptr<TCPServer> server = nullptr;
    try {
        server.reset(new TCPServer("0.0.0.0", 9388));            
    } catch (TCPServerError &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
   
    while (true)
    {
        std::shared_ptr<TCPConnection> conn;
        try {
            conn = server->accept();
            std::cout << "accept new connection[" << conn->address() << "]" << std::endl;
           
        } catch (const TCPServerError &e) {
            std::cerr << e.what() << std::endl;
            continue;
        }
        
        auto n = fork();
        if (n == -1) {
            std::cerr << "fork error: %s" << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (n == 0) {
            handleClient(conn);
            exit(EXIT_SUCCESS);
        } 
    }

    return 0;
}
