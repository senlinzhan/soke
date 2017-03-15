#include "Reactor.hpp"
#include "Event.hpp"

constexpr uint32_t Event::NONE_EVENT;
constexpr uint32_t Event::READ_EVENT;
constexpr uint32_t Event::WRITE_EVENT;

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

void Event::update()
{
    if (reactor_ != nullptr)
    {
        reactor_->updateEvent(shared_from_this());
    }       
}

void Event::disableAll()
{
    if (interestedEvents_ != NONE_EVENT)
    {
        interestedEvents_ = NONE_EVENT;
        update();
    }
}

void Event::activeRead()
{
    if (interestedEvents_ != (interestedEvents_ | READ_EVENT))
    {
        interestedEvents_ |= READ_EVENT;
        update();
    }
}

 void Event::activeWrite()
 {
     if (interestedEvents_ != (interestedEvents_ | WRITE_EVENT))
     {
         interestedEvents_ |= WRITE_EVENT;
         update();
     }
 }


void Event::disableRead()
{
    if (interestedEvents_ != (interestedEvents_ & ~READ_EVENT))
    {
        interestedEvents_ &= ~READ_EVENT;
        update();
    }
}

void Event::disableWrite()
{
    if (interestedEvents_ != (interestedEvents_ & ~WRITE_EVENT))
    {
        interestedEvents_ &= ~WRITE_EVENT;
        update();
    }
}

bool Event::isActive() const
{
    return interestedEvents_ != NONE_EVENT; 
}
