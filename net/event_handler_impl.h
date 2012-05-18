// Author: Hongzhang Liu
// Date:

#ifndef SWIFT_NET_EPOLL_HANDLER_IMPL_H__
#define SWIFT_NET_EPOLL_HANDLER_IMPL_H__

#include <unistd.h>

#include <swift/base/noncopyable.h>
#include <swift/net/event_handler.h>
#include <swift/net/epoll_reactor.h>

namespace swift { namespace net
{

class EventHandlerImpl : public EventHandler, public noncopyable
{
public:
    EventHandlerImpl(EpollReactor *reactor, int fd, int event)
        : reactor_(reactor), mask_(event), fd_(fd)
    {
        reactor_->Attach(fd_, this, mask_);
    }

    virtual ~EventHandlerImpl()
    {
        close(fd_);
    }

    int Fd() { return fd_; }

    EpollReactor *Reactor()
    {
        return reactor_;
    }

    static void Delete_(EventHandlerImpl *self)
    {
        delete self;
    }

    virtual int HandleError(EpollReactor *reactor)
    {
        reactor->QueueTask(std::tr1::bind(Delete_, this));
        return -1;
    }

    virtual int HandleHangup(EpollReactor *reactor)
    {
        reactor->QueueTask(std::tr1::bind(Delete_, this));
        return -1;
    }

    void EnableInputNotify()
    {
        EnableNotify(EPOLLIN);
    }

    void DisableInputNotify()
    {
        DisableNotify(EPOLLIN);
    }

    void EnableOutputNotify()
    {
        EnableNotify(EPOLLOUT);
    }

    void DisableOutputNotify()
    {
        DisableNotify(EPOLLOUT);
    }

    void EnableNotify(int event)
    {
        if((mask_ & event) == event)
            return;

        mask_ |= event;
        reactor_->Modify(fd_, this, mask_);
    }

    void DisableNotify(int event)
    {
        if((mask_ & event) == 0)
            return;

        mask_ &= ~event;
        reactor_->Modify(fd_, this, mask_);
    }

private:
    EpollReactor *reactor_;
    int mask_; //event mask
    int fd_;
};

}}
#endif

