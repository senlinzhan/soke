#ifndef EVENT_H
#define EVENT_H

#include <unistd.h>
#include <functional>

class Event
{
public:
    using Callback = std::function<void()>;
    
    Event(int fd)
        : fd_(fd)
    {        
    }
    
    ~Event()
    {
        ::close(fd_);
    }

    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;

    int fd() const
    {
        return fd_;
    }
    
    void doWhenRead(Callback callback)
    {
        whenRead_ = callback;
    }
    
    void doWhenWrite(Callback callback)
    {
        whenWrite_ = callback;
    }
    
    void doWhenClose(Callback callback)
    {
        whenClose_ = callback;
    }
    
    void doWhenError(Callback callback)
    {
        whenError_ = callback;
    }

    void setReadyEvents(int readyEvents)
    {
        readyEvents_ = readyEvents;
    }    
    
private:
    int fd_;        
    int interestedEvents_;
    int readyEvents_;
    
    Callback whenRead_;
    Callback whenWrite_;
    Callback whenClose_;
    Callback whenError_;

};



#endif /* EVENT_H */
