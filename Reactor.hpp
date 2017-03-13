#ifndef REACTOR_H
#define REACTOR_H

#include "Epoll.hpp"

#include <thread>
#include <memory>
#include <vector>

class Event;

class Reactor
{
public:
    Reactor();
    ~Reactor();

    Reactor(const Reactor &) = delete;
    Reactor &operator=(const Reactor &) = delete;

    void updateEvent(std::shared_ptr<Event> event);
    
    void run();

private:
    std::thread::id currentThreadId_;
    Epoll epoll_;
    std::vector<std::shared_ptr<Event>> events_;
};



#endif /* REACTOR_H */
