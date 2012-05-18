#ifndef SWIFT_NET_EPOLL_REACTOR_H__
#define SWIFT_NET_EPOLL_REACTOR_H__

#include <sys/epoll.h>
#include <vector>
#include <string>
#include <tr1/functional>

#include <swift/base/noncopyable.h>

namespace swift { namespace net
{

struct EventHandler;

class EpollReactor : public noncopyable
{
public:
    explicit EpollReactor(const char *name);
    ~EpollReactor();

    const std::string &Name() const { return name_; }

    int Attach(int fd, EventHandler *eh, int events);
    int Modify(int fd, EventHandler *eh, int events);
    int Detach(int fd);

    void Run();
    int RunOnce();
    void Stop() { stop_ = 1; }
    bool Stoped() { return stop_ == 1; }

    typedef std::tr1::function<void ()> Task;
    void QueueTask(Task t);

private:
    std::string name_;
    int epoll_;
    volatile int stop_;
    std::vector<epoll_event> events_;
    std::vector<Task> tasks_;
};

}}
#endif
