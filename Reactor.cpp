#include "Reactor.hpp"

#include <assert.h>

Reactor::Reactor()    
    : currentThreadId_(std::this_thread::get_id())
{    
}

Reactor::~Reactor()
{    
}

void Reactor::run()
{
    assert(std::this_thread::get_id() == currentThreadId_);
}


