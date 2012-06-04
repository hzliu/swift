#ifndef SWIFT_NET_EPOLL_REACTOR_H__
#define SWIFT_NET_EPOLL_REACTOR_H__

#include <sys/epoll.h>
#include <vector>
#include <string>
#include <tr1/functional>

#include <swift/base/noncopyable.h>
#include <swift/net/timer_queue.h>

namespace swift { namespace net
{

struct EventHandler;
typedef std::tr1::function<void ()> TimerCallback;

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
    int RunOnce(int milliseconds);
    void Stop() { stop_ = 1; }
    bool Stoped() { return stop_ == 1; }

    TimerId RunAfter(int milliseconds, TimerCallback callback);
    TimerId RunAt(Timestamp when, TimerCallback callback);
    bool CancelTimer(TimerId id);

    typedef std::tr1::function<void ()> Task;
    void QueueTask(Task t);

private:
    std::string name_;
    int epoll_;
    volatile int stop_;
    std::vector<epoll_event> events_;
    std::vector<Task> tasks_;

    TimerQueue timer_queue_;
};

}}
#endif
