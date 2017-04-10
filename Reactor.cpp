#include "Event.hpp"
#include "Reactor.hpp"

#include <glog/logging.h>
#include <assert.h>

thread_local Reactor *reactorInThisThread = nullptr;

Reactor::Reactor()
    : quit_(false),
      currentThreadId_(std::this_thread::get_id()),
      event_fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
{
    if (reactorInThisThread != nullptr)
    {
        LOG(FATAL) << "This thread already has a reactor";
    }
    reactorInThisThread = this;
    if (wakeup_fd_ < 0)
    {
        LOG(FATAL) << "The event fd is invalid";
    }

    auto whenRead = [this] ()
    {
        uint64_t one = 1;
        ::read(wakeup_fd_, &one, sizeof(one));
    };
    
    wakeupEvent_ = Event::create(wakeup_fd_);
    wakeupEvent_->doWhenRead(whenRead);
    wakeupEvent_->activeRead();    
}

Reactor::~Reactor()
{
    reactorInThisThread = nullptr;
    ::close(wakeup_fd_);
}

void Reactor::quit()
{
    quit_ = true;
}


void Reactor::registerEvent(EventPtr event)
{
    assert(std::this_thread::get_id() == currentThreadId_);
    assert(event != nullptr);
    
    int fd = event->fd();
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
        if (event->isActive())
        {
            invalidFds_.erase(fd);
            epoll_.addEvent(fd, event->interestedEvents(), event.get());
        }
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

bool Reactor::hasRegisterEvent(EventPtr event)
{
    assert(event != nullptr);
    return events_.find(event->fd()) != events_.end();
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
