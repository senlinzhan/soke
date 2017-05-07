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

    void registerEvent(EventPtr event);
    void deregisterEvent(EventPtr event);    
    void updateEvent(EventPtr event);
    bool hasRegisterEvent(EventPtr event);
    
    void run();
    void quit();    
private:
    bool quit_;
    std::thread::id currentThreadId_;

    int wakeup_fd_;
    EventPtr wakeupEvent_;
    
    Epoll epoll_;
    std::unordered_map<int, EventPtr> events_;
    std::unordered_set<int> invalidFds_;
};

#endif /* REACTOR_H */
