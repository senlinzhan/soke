#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>

#include <array>
#include <vector>
#include <set>

class Event;

class Epoll
{
public:
    Epoll();
    ~Epoll();
    
    Epoll(const Epoll &) = delete;
    Epoll &operator=(const Epoll &) = delete;

    void addEvent(int fd, uint32_t events, Event *ptr);

    void updateEvent(int fd, uint32_t events, Event *ptr);
    
    void deleteEvent(int fd);

    std::vector<Event *> pollEvents(int timeoutMs);
    
private:
    static constexpr int MAX_EVENTS = 64;

    int epfd_;
    std::array<struct epoll_event, MAX_EVENTS> events_;
};



#endif /* EPOLL_H */
