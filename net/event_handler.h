#ifndef EVENT_HANDLER_H__
#define EVENT_HANDLER_H__

class epoll_reactor;

struct event_handler
{
    virtual int handle_input(epoll_reactor *reactor) = 0;
    virtual int handle_output(epoll_reactor *reactor) = 0;
    virtual int handle_hangup(epoll_reactor *reactor) = 0;
    virtual int handle_error(epoll_reactor *reactor) = 0;
};

#endif
