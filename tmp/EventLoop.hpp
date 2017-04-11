#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Epoll.hpp"

#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace soke
{
    class Channel;
    
    class EventLoop
    {
    public:
        EventLoop();
        ~EventLoop();
        
        EventLoop(const EventLoop &) = delete;
        EventLoop &operator=(const EventLoop &) = delete;

        void updateChannel(Channel *channel);
        void removeChannel(Channel *channel);
        
        void loop();
        void quit();
        
    private:
        bool looping_;
        bool quit_;
        std::thread::id tid_;
        
        Epoll epoll_;
        std::unordered_map<int, Channel *> channels_;
        std::unordered_set<int> unregisteredFds_;
    };    
}

#endif /* EVENTLOOP_H */
