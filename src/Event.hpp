#ifndef EVENT_H
#define EVENT_H

#include <unistd.h>
#include <functional>
#include <sys/epoll.h>
#include <memory>

class Reactor;

class Event: public std::enable_shared_from_this<Event>
{
    friend class Reactor;    
public:            
    using Callback = std::function<void()>;
    
    static std::shared_ptr<Event> create(int fd)
    {
        return std::make_shared<Event>(fd);
    }
    
    ~Event()
    {
    }

    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;

    int fd() const
    {
        return fd_;
    }

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

    void disableAll();

    void activeRead();
    void disableRead();
    
    void activeWrite();
    void disableWrite();    
    
    bool isActive() const;
    
private:
    static constexpr uint32_t NONE_EVENT = 0;
    static constexpr uint32_t READ_EVENT = EPOLLIN | EPOLLPRI;
    static constexpr uint32_t WRITE_EVENT = EPOLLOUT;

    Event(int fd)
        : fd_(fd),
          interestedEvents_(NONE_EVENT),
          readyEvents_(0),
          reactor_(nullptr)
    {        
    }

    void update();
    
    int       fd_;
    uint32_t  interestedEvents_;
    uint32_t  readyEvents_;
    Reactor  *reactor_;
    
    Callback whenRead_;
    Callback whenWrite_;
    Callback whenClose_;
    Callback whenError_;

};

#endif /* EVENT_H */
