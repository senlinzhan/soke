#ifndef THREADRAII_H
#define THREADRAII_H

#include <thread>

class ThreadRAII
{
public:
    enum class DtorAction {join, detach};
    
    template<typename... Ts>
    ThreadRAII(DtorAction a, Ts&&... params)
        : action_(a),
          thread_(std::forward<Ts>(params)...)
    {
    }

    ThreadRAII(ThreadRAII &&) = default;
    ThreadRAII &operator=(ThreadRAII &&) = default;
    
    ~ThreadRAII()
    {
        if (thread_.joinable())
        {
            if (action_ == DtorAction::join)
            {
                thread_.join();
            }
            else
            {
                thread_.detach();
            }
        }
    }
	
    std::thread &get()
    {
        return thread_;
    }
    
private:
    DtorAction action_;
    std::thread thread_;
};


#endif /* THREADRAII_H */
