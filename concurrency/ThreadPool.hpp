#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <unordered_map>
#include <cassert>

class ThreadPool
{
public:
    using TaskType = std::function<void()>;
    using MutexGuard = std::lock_guard<std::mutex>;

    ThreadPool()
        : ThreadPool(std::thread::hardware_concurrency())
    {        
    }
    
    explicit ThreadPool(size_t maxThreads)
        : quit_(false),
          currentThreads_(0),
          idleThreads_(0),
          maxThreads_(maxThreads)
    {        
    }
        
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    
    ~ThreadPool()
    {
        {
            MutexGuard guard(mutex_);
            quit_ = true;
        }

        for (auto &p: threads_)
        {
            if (p.second.joinable())
            {
                p.second.join();
            }
        }        
    }
    
    void worker()
    {
        while (true)
        {
            TaskType task;
            {
                std::unique_lock<std::mutex> uniqueLock(mutex_);
                ++idleThreads_;
                
                auto isNotTimedOut = cv_.wait_for(uniqueLock, std::chrono::seconds(WAIT_SECONDS), [this]()
                {
                    return quit_ || !tasks_.empty();
                });
                
                --idleThreads_;

                if (tasks_.empty())
                {
                    // FIXME: this code not for not quit
                    if (quit_ || !isNotTimedOut)
                    {
                        while (!finishThreadIDs_.empty())
                        {
                            auto id = std::move(finishThreadIDs_.front());
                            finishThreadIDs_.pop();

                            auto iter = threads_.find(id);
                            assert(iter != threads_.end());

                            if (iter->second.joinable())
                            {
                                iter->second.join();
                            }
                            threads_.erase(iter);
                        }
                        
                        --currentThreads_;
                        finishThreadIDs_.emplace(std::this_thread::get_id());
                        return;
                    }
                }
                task = std::move(tasks_.front());
                tasks_.pop();
                std::cout << "task executing..." << std::endl;
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

        if (idleThreads_ > 0)
        {
            cv_.notify_one();
        }
        else if (currentThreads_ < maxThreads_)
        {
            MutexGuard guard(mutex_);
            
            std::thread t(&ThreadPool::worker, this);
            
            assert(threads_.find(t.get_id()) == threads_.end());
            
            std::cout << "create thread: " << t.get_id() << std::endl;
            threads_.emplace(std::make_pair(t.get_id(), std::move(t)));
        }        

        return result;
    }
    
private:
    constexpr static size_t  WAIT_SECONDS = 2;
    
    bool                     quit_;
    size_t                   currentThreads_;     
    size_t                   idleThreads_;       
    size_t                   maxThreads_;

    std::unordered_map<std::thread::id, std::thread> threads_;
    std::queue<std::thread::id> finishThreadIDs_;
    std::mutex               mutex_;
    std::condition_variable  cv_;
    std::queue<TaskType>     tasks_;
};

constexpr size_t ThreadPool::WAIT_SECONDS;

#endif /* THREADPOOL_H */
