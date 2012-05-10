// Author: Hongzhang Liu
// Date:

#ifndef EPOLL_HANDLER_IMPL_H__
#define EPOLL_HANDLER_IMPL_H__

#include <unistd.h>

#include "../base/noncopyable.h"

#include "event_handler.h"
#include "epoll_reactor.h"

class event_handler_impl : public event_handler, public noncopyable
{
public:
    event_handler_impl(epoll_reactor *reactor, int fd, int event) :
        m_reactor(reactor), m_mask(event), m_fd(fd)
    {
        m_reactor->attach(m_fd, this, m_mask);
    }

    virtual ~event_handler_impl()
    {
        close(m_fd);
    }

    int fd() { return m_fd; }

    epoll_reactor *reactor()
    {
        return m_reactor;
    }

    static void delete_(void *arg)
    {
        event_handler_impl *me = (event_handler_impl *)arg;
        delete me;
    }

    virtual int handle_error(epoll_reactor *reactor)
    {
        reactor->push_delay_task(delete_, this);
        return -1;
    }

    virtual int handle_hangup(epoll_reactor *reactor)
    {
        reactor->push_delay_task(delete_, this);
        return -1;
    }

    void enable_input_notify()
    {
        enable_notify(EPOLLIN);
    }

    void disable_input_notify()
    {
        disable_notify(EPOLLIN);
    }

    void enable_output_notify()
    {
        enable_notify(EPOLLOUT);
    }

    void disable_output_notify()
    {
        disable_notify(EPOLLOUT);
    }

    void enable_notify(int event)
    {
        if((m_mask & event) == event)
            return;

        m_mask |= event;
        m_reactor->modify(m_fd, this, m_mask);
    }

    void disable_notify(int event)
    {
        if((m_mask & event) == 0)
            return;

        m_mask &= ~event;
        m_reactor->modify(m_fd, this, m_mask);
    }

private:
    epoll_reactor *m_reactor;
    int m_mask; //event mask
    int m_fd;
};

#endif

