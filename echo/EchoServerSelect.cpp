#include "../TCPServer.hpp"

#include <cstdlib>
#include <memory>
#include <iostream>


int main(int argc, char *argv[])
{
    std::unique_ptr<TCPServer> server = nullptr;
    try {
        server.reset(new TCPServer("0.0.0.0", 9387));            
    } catch (TCPServerError &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        try {
            auto addr = server->accept();
            //std::cout << addr.toString() << std::endl;
        } catch (const TCPServerError &e) {
            std::cerr << e.what() << std::endl;
        }
    } 
    
    return 0;
}

