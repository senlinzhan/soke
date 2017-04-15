#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

namespace soke
{
    class EventLoop;
    
    class Channel
    {
    public:
        using EventCallback = std::function<void ()>;
        
        Channel(EventLoop *loop, int fd);
        ~Channel();

        Channel(const Channel &) = delete;
        Channel &operator=(const Channel &) = delete;

        void setReadCallback(EventCallback callback);
        void setWriteCallback(EventCallback callback);
        void setErrorCallback(EventCallback callback);
        void setCloseCallback(EventCallback callback);
        
        void setReadyEvents(int revents);
        
        EventLoop *ownerLoop() const;

        void enableReading();
        void enableWriting();

        void disableWriting();
        void disableAll();
        
        int fd() const;

        bool isNoneEvent() const;
        int events() const;

        void handleEvent();
        
    private:
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;
        
        EventLoop       *loop_;
        const int        fd_;
        int              events_;
        int              revents_;
        EventCallback    readCallback_;
        EventCallback    writeCallback_;
        EventCallback    errorCallback_;
        EventCallback    closeCallback_;
    };    
};

#endif /* CHANNEL_H */
