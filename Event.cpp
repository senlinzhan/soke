#include "Event.hpp"
#include <unistd.h>

constexpr uint32_t Event::READ_EVENT;
constexpr uint32_t Event::WRITE_EVENT;

Event::Event(int fd, uint32_t interestedEvents)
    : fd_(fd),
      isValid_(true),
      interestedEvents_(interestedEvents),
      readyEvents_(0)
{    
}

Event::~Event()
{
    ::close(fd_);
}

int Event::fd() const
{
    return fd_;
}

void Event::handleEvent()
{
    if ((readyEvents_ & EPOLLHUP) && !(readyEvents_ & EPOLLIN) && whenClose_)
    {
        whenClose_();
    }

    if ((readyEvents_ & EPOLLERR) && whenError_)
    {
        whenError_();
    }

    if ((readyEvents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) && whenRead_)
    {
        whenRead_();
    }

    if ((readyEvents_ & EPOLLOUT) && whenWrite_)
    {
        whenWrite_();
    }
}
