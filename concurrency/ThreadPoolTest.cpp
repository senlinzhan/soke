#include "ThreadPool.hpp"

#include <chrono>
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "starting..." << std::endl;
    ThreadPool pool(1000);

    std::vector<std::future<void>> futures;    

    for (int i = 0; i < 10; ++i)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));        
        futures.push_back(pool.submit([]()
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }));        
    }
    
    for (auto &ftu: futures)
    {
        ftu.get();
    }
    std::cout << "ending..." << std::endl;

    
    return 0;
}
