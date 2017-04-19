#ifndef CALLBACK_H
#define CALLBACK_H

#include <memory>
#include <functional>

namespace soke
{
    class TCPConnection;
    class Buffer;
    
    using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
    using ConnectionCallback = std::function<void (TCPConnectionPtr)>;
    using MessageCallback = std::function<void (TCPConnectionPtr, Buffer *buf)>;
};

#endif /* CALLBACK_H */
