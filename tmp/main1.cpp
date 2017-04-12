#include "IPAddress.hpp"
#include "EventLoop.hpp"
#include "Acceptor.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    IPAddress addr("127.0.0.1", 9533);
    soke::EventLoop loop;

    soke::Acceptor acceptor(&loop, addr);

    auto callback = [] (int sockfd, const IPAddress &addr)
                    {
                        std::cout << "new connection from" << addr.toString() << std::endl;
                        ::write(sockfd, "How are you?\n", 13);
                    };
    acceptor.setNewConnectionCallback(callback);
    loop.loop();
    

    
    
    return 0;
}

