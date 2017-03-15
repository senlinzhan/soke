#ifndef EVENT_H
#define EVENT_H

#include <unistd.h>
#include <functional>
#include <sys/epoll.h>
#include <memory>

class Reactor;

class Event : public std::enable_shared_from_this<Event>
{
    friend class Reactor;
    
public:    
    static constexpr uint32_t NONE_EVENT = 0;
    static constexpr uint32_t READ_EVENT = EPOLLIN | EPOLLPRI;
    static constexpr uint32_t WRITE_EVENT = EPOLLOUT;
        
    using Callback = std::function<void()>;
    
    Event(int fd, uint32_t interestedEvents)
        : fd_(fd),
          interestedEvents_(interestedEvents),
          readyEvents_(0),
          reactor_(nullptr)
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
