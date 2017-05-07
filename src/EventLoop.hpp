#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Epoll.hpp"

#include <queue>
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
        using Task = std::function<void ()>;
        
        EventLoop();
        ~EventLoop();
        
        EventLoop(const EventLoop &) = delete;
        EventLoop &operator=(const EventLoop &) = delete;

        void updateChannel(Channel *channel);
        void removeChannel(Channel *channel);
        
        void loop();
        void quit();

        void queueInLoop(Task task);
        
    private:
        bool                               looping_;
        bool                               quit_;
        std::thread::id                    tid_;
        std::queue<Task>                   taskQueue_;
        Epoll                              epoll_;
        std::unordered_map<int, Channel *> channels_;
        std::unordered_set<int>            unregisteredFds_;
    };    
};

#endif /* EVENTLOOP_H */
