#include <swift/net/epoll_reactor.h>

#include <sys/epoll.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <stdexcept>
#include <sstream>

#include <swift/net/event_handler.h>

namespace swift { namespace net
{

EpollReactor::EpollReactor(const char *name)
    :name_(name), stop_(0)
{
    epoll_ = epoll_create(16);  //16 is unused actually; see manpage
    if(epoll_ < 0)
    {
        //what should we do here?
        std::ostringstream ss;
        ss << "create epoll fd error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }

    //should be enough
    tasks_.reserve(1024);
    events_.resize(1024);
}

EpollReactor::~EpollReactor()
{
    close(epoll_);
}

int EpollReactor::Attach(int fd, EventHandler *eh, int events)
{
    epoll_event e;
    e.events = events;
    e.data.ptr = eh;

    return epoll_ctl(epoll_, EPOLL_CTL_ADD, fd, &e);
}

int EpollReactor::Modify(int fd, EventHandler *eh, int events)
{
    epoll_event e;
    e.events = events;
    e.data.ptr = eh;

    return epoll_ctl(epoll_, EPOLL_CTL_MOD, fd, &e);
}

int EpollReactor::Detach(int fd)
{
    static epoll_event e;
    return epoll_ctl(epoll_, EPOLL_CTL_DEL, fd, &e);
}

int EpollReactor::RunOnce(int milliseconds)
{
    epoll_event *ev = events_.data();
    int rtn = epoll_wait(epoll_, ev, events_.size(), milliseconds);

    Timestamp now = Timestamp::Now();

    if(rtn < 0)
    {
        if(errno == EINTR)
            return 0;

        //error_log("epoll_wait error, %s", strerror(errno));
        return -1;
    }

    for (int i = 0; i < rtn; ++i)
    {
        EventHandler *eh = static_cast<EventHandler *>(ev[i].data.ptr);
        if(ev[i].events & EPOLLERR)
            if(eh->HandleError(this) != 0)
                continue;
        if(ev[i].events & EPOLLIN)
            if(eh->HandleInput(this) != 0)
                continue;
        if(ev[i].events & EPOLLOUT)
            if(eh->HandleOutput(this) != 0)
                continue;
        //we handle hangup at the last
        if(ev[i].events & EPOLLHUP)
            if(eh->HandleHangup(this) != 0)
                continue;
    }

    if(static_cast<size_t>(rtn) == events_.size())
        events_.resize(2 * rtn);

    for(std::vector<Task>::iterator iter = tasks_.begin();
            iter != tasks_.end(); ++iter)
    {
        (*iter)();
    }

    tasks_.clear();

    timer_queue_.RunUntil(now);

    return 0;
}

void EpollReactor::Run()
{
    const int kMicrosecondsPerMillisecond = 1e3;
    while(!stop_)
    {
        Timestamp next_timeout = timer_queue_.NextTimeout();
        int milliseconds = static_cast<int>(
                (next_timeout.MicrosecondsSinceEpoch() -
                 Timestamp::MicrosecondsUntilNow()) / kMicrosecondsPerMillisecond);
        if(milliseconds < 0)    //this is possible
            milliseconds = 0;
        if(RunOnce(milliseconds) < 0)
            return;
    }
}

void EpollReactor::QueueTask(Task t)
{
    tasks_.push_back(t);
}

TimerId EpollReactor::RunAfter(int milliseconds, TimerCallback callback)
{
    return timer_queue_.ScheduleTimer(
            Timestamp::MillisecondsLater(milliseconds), callback);
}

TimerId EpollReactor::RunAt(Timestamp when, TimerCallback callback)
{
    //FIXME: if when < now?
    return timer_queue_.ScheduleTimer(when, callback);
}

}}
