#include "Epoll.hpp"
#include "Channel.hpp"

#include <assert.h>
#include <unistd.h>
#include <string.h>

using namespace soke;

constexpr int Epoll::MAX_EVENTS;

Epoll::Epoll()
    : epfd_(::epoll_create1(EPOLL_CLOEXEC))
{
}

Epoll::~Epoll()
{
    ::close(epfd_);
}

void Epoll::addEvent(int fd, uint32_t events, Channel *ptr)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = events;
    event.data.ptr = ptr;
    ::epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);    
}

void Epoll::updateEvent(int fd, uint32_t events, Channel *ptr)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = events;
    event.data.ptr = ptr;
    
    ::epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event);        
}

void Epoll::deleteEvent(int fd)
{
    ::epoll_ctl(epfd_, EPOLL_CTL_DEL, fd , nullptr);
}

std::vector<Channel *> Epoll::pollEvents(int timeoutMs) 
{
    std::vector<Channel *> readyEvents;
    int num = ::epoll_wait(epfd_, events_.data(), MAX_EVENTS, timeoutMs);
    for (int i = 0; i < num; ++i)
    {
        auto ptr = static_cast<Channel *>(events_[i].data.ptr);
        ptr->setReadyEvents(events_[i].events);
        readyEvents.push_back(ptr);
    }

    return readyEvents;
}
