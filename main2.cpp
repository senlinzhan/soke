#include "Buffer.hpp"
#include "Callback.hpp"
#include "IPAddress.hpp"
#include "EventLoop.hpp"
#include "TCPServer.hpp"
#include "TCPConnection.hpp"
#include <unistd.h>
#include <iostream>

void onConnection(soke::TCPConnectionPtr conn)
{
    std::cout << "onConnection(): connection from " << std::endl;
}

void onMessage(soke::TCPConnectionPtr conn, soke::Buffer *buff)
{
    auto msg = buff->retrieveAsString();
    std::cout << "onMessage(): " << msg;
    conn->send(msg);
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

