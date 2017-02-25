#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

typedef struct threadpool_task_t
{
    void (*function)(void *);    
    void *argument;
    struct threadpool_task_t *next;    
} threadpool_task_t;


class ThreadPool
{
public:
    ThreadPool(int maxThreads);
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

private:
    int maxThreads_;
    int currentThreads_;
    int idleThreads_;
    bool quit_;
    
    std::mutex mutex_;
    std::condition_variable cond_;
    std::condition_variable quit_cond;
    std::queue<threadpool_task_t> tasks_;
};

ThreadPool::ThreadPool(int maxThreads)
    : maxThreads_(maxThreads),
      currentThreads_(0),
      idleThreads_(0),
      quit_(false)
{    
}


#endif /* THREADPOOL_H */
