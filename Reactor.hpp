#ifndef REACTOR_H
#define REACTOR_H

#include "Epoll.hpp"

#include <thread>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Event;

class Reactor
{
public:
    using EventPtr = std::shared_ptr<Event>;
    
    Reactor();
    ~Reactor();

    Reactor(const Reactor &) = delete;
    Reactor &operator=(const Reactor &) = delete;

    void insertEvent(EventPtr event);
    void deleteEvent(EventPtr event);
    
    void run();

private:
    std::thread::id currentThreadId_;
    Epoll epoll_;
    std::unordered_map<int, EventPtr> events_;
    std::unordered_set<int> activeFds_;
};



#endif /* REACTOR_H */
