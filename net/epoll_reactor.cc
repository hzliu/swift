#include <sys/epoll.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <stdexcept>
#include <sstream>

#include "epoll_reactor.h"

epoll_reactor::epoll_reactor(const char *name, int size) :
    m_stop(0), m_name(name)
{
    m_epoll = epoll_create(size);
    if(m_epoll < 0)
    {
        //what should we do here?
        std::ostringstream ss;
        ss << "create epoll fd error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }

    //should be enough
    m_tasks.reserve(1024);
    m_events.resize(1024);
}

epoll_reactor::~epoll_reactor()
{
    close(m_epoll);
}

int epoll_reactor::attach(int fd, event_handler *eh, int events)
{
    epoll_event e;
    e.events = events;
    e.data.ptr = eh;

    return epoll_ctl(m_epoll, EPOLL_CTL_ADD, fd, &e);
}

int epoll_reactor::modify(int fd, event_handler *eh, int events)
{
    epoll_event e;
    e.events = events;
    e.data.ptr = eh;

    return epoll_ctl(m_epoll, EPOLL_CTL_MOD, fd, &e);
}

int epoll_reactor::detach(int fd)
{
    static epoll_event e;
    return epoll_ctl(m_epoll, EPOLL_CTL_DEL, fd, &e);
}

int epoll_reactor::run_once()
{
    epoll_event *events = m_events.data();
    int rtn = epoll_wait(m_epoll, events, m_events.size(), -1);
    if(rtn < 0)
    {
        if(errno == EINTR)
            return 0;

        //error_log("epoll_wait error, %s", strerror(errno));
        return -1;
    }

    for (int i = 0; i < rtn; ++i)
    {
        event_handler *eh = (event_handler *)events[i].data.ptr;
        if(events[i].events & EPOLLERR)
            if(eh->handle_error(this) != 0)
                continue;
        if(events[i].events & EPOLLIN)
            if(eh->handle_input(this) != 0)
                continue;
        if(events[i].events & EPOLLOUT)
            if(eh->handle_output(this) != 0)
                continue;
        //we handle hangup at the last
        if(events[i].events & EPOLLHUP)
            if(eh->handle_hangup(this) != 0)
                continue;
    }

    if(rtn == m_events.size())
        m_events.resize(2 * rtn);

    for(std::vector<delay_task>::iterator iter = m_tasks.begin();
            iter != m_tasks.end(); ++iter)
    {
        iter->function(iter->arg);
    }

    m_tasks.clear();

    return 0;
}

void epoll_reactor::run()
{
    while(!m_stop)
    {
        if(run_once() < 0)
            return;
    }
}

void epoll_reactor::push_delay_task(pfn_action task, void *arg)
{
    delay_task t = { task, arg };
    m_tasks.push_back(t);
}

