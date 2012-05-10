// Author: Hongzhang Liu
// Date:

#ifndef LISTENER_H__
#define LISTENER_H__

#include "event_handler_impl.h"

class listener;

struct IWorkerFactory
{
    virtual void OnFdAccepted(epoll_reactor *reactor, int fd) = 0;
    virtual void OnAcceptError(listener *l) = 0;

    //return 0 if you can handle this situation
    virtual int OnFdExceeds(listener *l) = 0;
};

class listener : public event_handler_impl
{
public:
    listener(epoll_reactor *reactor, int fd, IWorkerFactory *factory);
    ~listener();

    int handle_input(epoll_reactor *reactor);
    int handle_output(epoll_reactor *reactor)
    {
        //you can't get here
        return 0;
    }

private:
    IWorkerFactory *m_factory;
    int m_nullFd;   //a backup fd to handle ENFILE or EMFILE
};

#endif

