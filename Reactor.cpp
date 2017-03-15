#include "Event.hpp"
#include "Reactor.hpp"

#include <assert.h>

Reactor::Reactor()    
    : currentThreadId_(std::this_thread::get_id())
{    
}

Reactor::~Reactor()
{    
}

void Reactor::registerEvent(EventPtr event)
{
    assert(event != nullptr);

    int fd = event->fd();
    assert(std::this_thread::get_id() == currentThreadId_);
    assert(events_.find(fd) == events_.end());
    
    events_[fd] = event;
    if (event->isActive())
    {
        epoll_.addEvent(fd, event->interestedEvents(), event.get());        
    }
    else
    {
        invalidFds_.insert(fd);
    }

    event->reactor_ = this;
}

void Reactor::run()
{
    assert(std::this_thread::get_id() == currentThreadId_);

    auto readyEvents = epoll_.pollEvents(1000);
    for (auto event: readyEvents)
    {
        event->handleEvent();
    }
}
  
void Reactor::updateEvent(EventPtr event)
{
    assert(std::this_thread::get_id() == currentThreadId_);
    assert(event != nullptr);
    assert(events_.find(event->fd()) != events_.end());    

    int fd = event->fd();
    if (invalidFds_.find(fd) == invalidFds_.end())
    {
        invalidFds_.erase(fd);
        epoll_.addEvent(fd, event->interestedEvents(), event.get());
    }
    else
    {
        if (event->isActive())
        {
            epoll_.updateEvent(fd, event->interestedEvents(), event.get());
        }
        else
        {
            epoll_.deleteEvent(fd);
            invalidFds_.insert(fd);
        }
    }
}
  
void Reactor::deregisterEvent(EventPtr event)
{
    assert(std::this_thread::get_id() == currentThreadId_);
    assert(event != nullptr);
    assert(events_.find(event->fd()) != events_.end());
    
    int fd = event->fd();
    events_.erase(fd);
    if (invalidFds_.find(fd) != invalidFds_.end())
    {
        invalidFds_.erase(fd);
    }
    else
    {
        epoll_.deleteEvent(fd);
    }
    event->reactor_ = nullptr;
}
