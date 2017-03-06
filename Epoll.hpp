#ifndef EPOLL_H
#define EPOLL_H


class Epoll
{
public:
    Epoll();
    ~Epoll();

    void addRead(int sockfd);
    
    Epoll(const Epoll &) = delete;
    Epoll &operator=(const Epoll &) = delete;
    
private:
    int epfd_;
};



#endif /* EPOLL_H */
