#include "Buffer.hpp"
#include "Callback.hpp"
#include "IPAddress.hpp"
#include "EventLoop.hpp"
#include "TCPServer.hpp"
#include <unistd.h>
#include <iostream>

void onConnection(soke::TCPConnectionPtr conn)
{
    std::cout << "onConnection(): connection from " << std::endl;
}

void onMessage(soke::TCPConnectionPtr, soke::Buffer *buff)
{
    std::cout << "onMessage(): " << buff->retrieveAsString();
}

int main(int argc, char *argv[])
{
    soke::IPAddress addr("127.0.0.1", 9533);
    soke::EventLoop loop;

    soke::TCPServer server(&loop, addr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();    
    
    return 0;
}

