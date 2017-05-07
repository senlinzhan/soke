#include "EventLoop.hpp"
#include "Channel.hpp"

#include <unistd.h>
#include <strings.h>
#include <iostream>
#include <sys/timerfd.h>

soke::EventLoop *g_loop;

void timeout()
{
    std::cout << "Timeout!" << std::endl;
    g_loop->quit();
}

int main()
{
    soke::EventLoop loop;
    g_loop = &loop;
    
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    soke::Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);
    
    loop.loop();
    
    ::close(timerfd);
}
