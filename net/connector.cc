#include <swift/net/connector.h>

#include <errno.h>
#include <assert.h>

#include <swift/net/epoll_reactor.h>
#include <swift/net/sock_util.h>

namespace swift { namespace net
{

Connector::Connector(EpollReactor *reactor, const sockaddr_in &peer)
    : reactor_(reactor), peer_(peer), fd_(-1)
{
}

Connector::~Connector()
{
}

int Connector::Connect(int timeout)
{
    assert(on_succeed_ && on_failed_);

    fd_ = socket(PF_INET, SOCK_STREAM, 0);
    if(fd_ < 0)
    {
        if(on_failed_) on_failed_(this, errno);
        delete this;
        return kConnectFailed;
    }

    set_nonblocking(fd_);
    set_reuseaddr(fd_);

    int rtn = ::connect(fd_, reinterpret_cast<sockaddr *>(&peer_), sizeof(peer_));
    if(rtn == 0)
    {
        if(on_succeed_) on_succeed_(this);
        delete this;
        return kConnectSucceed;
    }

    if(rtn < 0 && errno != EINPROGRESS)
    {
        Failed(errno);
        return kConnectFailed;
    }

    reactor_->Attach(fd_, this, EPOLLOUT);
    return kConnectInProgress;
}

int Connector::HandleOutput(EpollReactor *reactor)
{
    int err = 0;
    socklen_t len = sizeof(err);
    int rtn = getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &len);
    if(rtn < 0 || err != 0)
    {
        Failed(rtn < 0 ? errno : err);
        return -1;
    }

    reactor->Detach(fd_);
    if(on_succeed_) on_succeed_(this);
    delete this;
    return -1;
}

int Connector::HandleError(EpollReactor *reactor)
{
    int err = 0;
    socklen_t len = sizeof(err);
    int rtn = getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &len);
    Failed(rtn < 0 ? errno : err);
    return -1;
}

int Connector::HandleHangup(EpollReactor *reactor)
{
    Failed(-1);
    return -1;
}

void Connector::Failed(int err)
{
    if(on_failed_) on_failed_(this, err);
    close(fd_);
    delete this;
}

}}
