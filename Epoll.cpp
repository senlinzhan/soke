#include "Epoll.hpp"

#include <unistd.h>
#include <sys/epoll.h>


Epoll::Epoll()
{
    // FIXME: add error checking
    epfd_ = epoll_create1(EPOLL_CLOEXEC);    
}

Epoll::~Epoll()
{
    close(epfd_);
}

void Epoll::addRead(int sockfd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;

    // FIXME: add error checking
    epoll_ctl(efd_, EPOLL_CTL_ADD, sockfd, ev);    
}


