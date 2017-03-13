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

void Reactor::run()
{
    assert(std::this_thread::get_id() == currentThreadId_);

    auto readyEvents = epoll_.pollEvents(1000);
    for (auto event: readyEvents)
    {
        event->handleEvent();
    }
}

void Reactor::insertEvent(EventPtr event)
{
    assert(std::this_thread::get_id() == currentThreadId_);
    
    int fd = event->fd();
    if (events_.find(fd) == events_.end())
    {
        events_[fd] = event;
        activeFds_.insert(fd);
        epoll_.addEvent(fd, event->interestedEvents(), event.get());
    }
    else
    {
        if (activeFds_.find(fd) == activeFds_.end())
        {
            activeFds_.insert(fd);
            epoll_.addEvent(fd, event->interestedEvents(), event.get());
        }
        else
        {
            if (event->isValid())
            {
                epoll_.updateEvent(fd, event->interestedEvents(), event.get());
            }
            else
            {
                activeFds_.erase(fd);
                epoll_.deleteEvent(fd);
            }
        }        
    }
}
 
void Reactor::deleteEvent(EventPtr event)
{
    assert(std::this_thread::get_id() == currentThreadId_);
    
    int fd = event->fd();
    assert(events_.find(fd) != events_.end());

    events_.erase(fd);
    activeFds_.erase(fd);    
    epoll_.deleteEvent(fd);
}
