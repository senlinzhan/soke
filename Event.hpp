#ifndef EVENT_H
#define EVENT_H

#include <unistd.h>
#include <functional>
#include <sys/epoll.h>

class Event
{
public:
    using Callback = std::function<void()>;
    
    Event(int fd, uint32_t interestedEvents);
    
    ~Event();

    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;

    int fd() const;

    void handleEvent();
    
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

    void setReadyEvents(uint32_t readyEvents)
    {
        readyEvents_ = readyEvents;
    }    

    uint32_t interestedEvents() const
    {
        return interestedEvents_;
    }

    bool isValid() const
    {
        return isValid_;
    }
    
private:
    static constexpr uint32_t READ_EVENT = EPOLLIN | EPOLLPRI;
    static constexpr uint32_t WRITE_EVENT = EPOLLOUT;
    
    int      fd_;        
    bool     isValid_;
    uint32_t interestedEvents_;
    uint32_t readyEvents_;

    
    Callback whenRead_;
    Callback whenWrite_;
    Callback whenClose_;
    Callback whenError_;

};



#endif /* EVENT_H */
