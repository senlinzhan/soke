#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>


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

    void add(void (*function)(void *), void *argument);        
private:
    void worker();
    
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


void ThreadPool::worker()
{
    while (true)
    {
        bool has_timedout = false;
        {
            std::lock_guard<std::mutex> guard(mutex_);
            idleThreads_++;
        }
        std::unique_lock<std::mutex> uni_lock(mutex_);
        
        while (tasks_.empty() || !quit_)
        {
            if (cond_.wait_for(uni_lock, std::chrono::seconds(2)) == std::cv_status::timeout)
            {
                has_timedout = true;
                break;
            }
        }
        
        {
            mutex_.lock();
            idleThreads_--;
            if (quit_)
            {
                currentThreads_--;
                mutex_.unlock();
                quit_cond.notify_all();
                return;
            }

            if (!tasks_.empty())
            {
                auto task = tasks_.front();
                tasks_.pop();
                task.function(task.argument);
                mutex_.unlock();
            }
            if (tasks_.empty() && has_timedout)
            {
                currentThreads_--;
                return;
            }
        }
    }
}


void ThreadPool::add(void (*function)(void *), void *argument)
{
    threadpool_task_t task;
    task.function = function;
    task.argument = argument;

    std::lock_guard<std::mutex> guard(mutex_);
    
    tasks_.push(task);
    if (idleThreads_ > 0)
    {
        cond_.notify_one();
    }
    else if (currentThreads_ < maxThreads_)
    {
        std::thread t(&ThreadPool::worker, this);
        currentThreads_++;
    }
}


ThreadPool::~ThreadPool()
{
    std::lock_guard<std::mutex> guard(mutex_);
    std::unique_lock<std::mutex> uni_lock(mutex_);
    
    if (currentThreads_ > 0)
    {
        quit_ = true;
        if (idleThreads_ > 0)
        {
            cond_.notify_all();
        }
        while (currentThreads_ > 0)
        {
            quit_cond.wait(uni_lock);            
        }
    }    
}

#endif /* THREADPOOL_H */
