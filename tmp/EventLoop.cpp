#include "EventLoop.hpp"

#include "Channel.hpp"

#include <assert.h>
#include <glog/logging.h>
#include <poll.h>

using namespace soke;

thread_local EventLoop *t_loopInThisThread = nullptr;

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      tid_(std::this_thread::get_id())
{
    if (t_loopInThisThread != nullptr)
    {
        LOG(FATAL) << "This thread already has a EventLoop";
    }
    else
    {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!looping_);
    assert(std::this_thread::get_id() == tid_);

    looping_ = true;
    quit_ = false;

    while (!quit_)
    {
        auto readyEvents = epoll_.pollEvents(1000);
        for (auto event: readyEvents)
        {
            event->handleEvent();
        }        
    }    
    looping_ = false;
}

void EventLoop::updateChannel(Channel *channel)
{
    assert(std::this_thread::get_id() == tid_);
    assert(channel != nullptr);
     
    int fd = channel->fd();
    if (channels_.find(fd) == channels_.end())
    {
        // new file descriptor
        channels_[fd] = channel;
        
        if (channel->isNoneEvent())
        {
            unregisteredFds_.insert(fd);
        }
        else
        { 
            epoll_.addEvent(fd, channel->events(), channel);
        }
    }
    else
    {
        if (unregisteredFds_.find(fd) != unregisteredFds_.end())
        {
            if (!channel->isNoneEvent())
            {
                epoll_.addEvent(fd, channel->events(), channel);
                unregisteredFds_.erase(fd);
            }
        }
        else
        {
            if (channel->isNoneEvent())
            {
                unregisteredFds_.insert(fd);
                epoll_.deleteEvent(fd);
            }
            else
            {
                epoll_.updateEvent(fd, channel->events(), channel);
            }
        }
    }    
}

void EventLoop::removeChannel(Channel *channel)
{
    assert(std::this_thread::get_id() == tid_);
    assert(channel != nullptr);
    assert(channels_.find(channel->fd()) != channels_.end());

    int fd = channel->fd();
    channels_.erase(fd);
    if (unregisteredFds_.find(fd) != unregisteredFds_.end())
    {
        unregisteredFds_.erase(fd);
    }
    else
    {
        epoll_.deleteEvent(fd);
    }
}


void EventLoop::quit()
{
    quit_ = true;
}
