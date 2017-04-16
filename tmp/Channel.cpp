#include "EventLoop.hpp"
#include "Channel.hpp"
#include <glog/logging.h>

using namespace soke;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    :loop_(loop),
     fd_(fd),
     events_(0),
     revents_(0)
{    
}

Channel::~Channel()
{    
}

void Channel::setReadyEvents(int revents)
{
    revents_ = revents;
}

EventLoop *Channel::ownerLoop() const
{
    return loop_;
}

void Channel::enableReading()
{
    events_ |= kReadEvent;
    loop_->updateChannel(this);
}

void Channel::enableWriting()
{
    events_ |= kWriteEvent;
    loop_->updateChannel(this);
}

void Channel::disableAll()
{
    events_ = kNoneEvent;
    loop_->updateChannel(this);
}

void Channel::disableWriting()
{
    events_ &= ~kWriteEvent;
    loop_->updateChannel(this);
}

void Channel::setReadCallback(EventCallback callback)
{
    readCallback_ = callback;
}

void Channel::setWriteCallback(EventCallback callback)
{
    writeCallback_ = callback;
}

void Channel::setErrorCallback(EventCallback callback)
{
    errorCallback_ = callback;
}

int Channel::fd() const
{
    return fd_;
}

bool Channel::isNoneEvent() const
{
    return events_ == kNoneEvent;
}

int Channel::events() const
{
    return events_;
}

void Channel::handleEvent()
{
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN) && closeCallback_)
    {
        closeCallback_();
    }

    if ((revents_ & EPOLLERR) && errorCallback_)
    {
        errorCallback_();
    }

    if ((revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) && readCallback_)
    {
        readCallback_();
    }

    if ((revents_ & EPOLLOUT) && writeCallback_)
    {
        writeCallback_();
    }
}
