#ifndef CALLBACK_H
#define CALLBACK_H

#include <memory>
#include <functional>

namespace soke
{
    class TCPConnection;
    
    using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
    using ConnectionCallback = std::function<void (TCPConnectionPtr)>;
    using MessageCallback = std::function<void (TCPConnectionPtr, const char *, ssize_t len)>;
};

#endif /* CALLBACK_H */
