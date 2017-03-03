#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    using TaskType = std::function<void()>;
    using MutexGuard = std::lock_guard<std::mutex>;
    
    ThreadPool()
        : ThreadPool(std::thread::hardware_concurrency())
    {        
    }
    
    explicit ThreadPool(size_t size)
        : quit_(false), size_(size)
    {
        for (size_t i = 0; i < size_; ++i)
        {
            threads_.emplace_back(&ThreadPool::worker, this);
        }
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    
    ~ThreadPool()
    {
        {
            MutexGuard guard(mutex_);
            quit_ = true;            
        }
        cv_.notify_all();
        
        for (auto &t: threads_)
        {
            t.join();
        }
    }
    
    void worker()
    {
        while (true)
        {
            TaskType task;
            {
                std::unique_lock<std::mutex> uniqueLock(mutex_);
                cv_.wait(uniqueLock, [this]()
                {
                    return quit_ || !tasks_.empty();
                });
            
                if (quit_ && tasks_.empty())
                {
                    return;
                }
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    template<typename Func, typename... Ts>
    auto submit(Func func, Ts&&... params)
    {
        auto execute = [func, &params...]
        {
            return func(std::forward<Ts>(params)...);
        };

        using ReturnType = std::result_of_t<decltype(execute)()>;
        using PackagedTask = std::packaged_task<ReturnType()>;
        
        auto task = std::make_shared<PackagedTask>(std::move(execute)); 
        auto result = task->get_future();
        {
            MutexGuard guard(mutex_);
            tasks_.emplace([task]()
            {
                (*task)();
            });
        }        
        cv_.notify_one();
        return result;
    }

    // the size of the pool will never change, so this function is thread-safe
    size_t size() const
    {
        return size_;
    }    
    
private:
    bool quit_;
    size_t size_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<std::thread> threads_;
    std::queue<TaskType> tasks_;
};


#endif /* THREADPOOL_H */
