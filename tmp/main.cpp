#include "EventLoop.hpp"

int main(int argc, char *argv[])
{
    soke::EventLoop loop;
    std::thread t([&loop]() {
            loop.loop();
        });
    t.join();
    
    return 0;
}

