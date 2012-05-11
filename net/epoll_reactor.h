#ifndef SWIFT_NET_EPOLL_REACTOR_H__
#define SWIFT_NET_EPOLL_REACTOR_H__

#include <sys/epoll.h>
#include <vector>
#include <string>

#include <swift/base/noncopyable.h>
#include <swift/net/event_handler.h>

typedef void (*pfn_action)(void *arg);

class epoll_reactor : public noncopyable
{
public:
    int attach(int fd, event_handler *eh, int events);
    int modify(int fd, event_handler *eh, int events);
    int detach(int fd);
    void run();
    int run_once();
    void stop() { m_stop = 1; }
    bool stoped() { return m_stop == 1; }
    void push_delay_task(pfn_action task, void *arg);

    const std::string &name() const { return m_name; }

    epoll_reactor(const char *name, int size);
    ~epoll_reactor();

private:
    int m_epoll;
    volatile int m_stop;
    std::vector<epoll_event> m_events;

    struct delay_task
    {
        pfn_action function;
        void *arg;
    };

    std::vector<delay_task> m_tasks;
    std::string m_name;
};

#endif
