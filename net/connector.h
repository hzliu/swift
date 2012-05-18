#ifndef SWIFT_NET_CONNECTOR_H__
#define SWIFT_NET_CONNECTOR_H__

#include <netinet/in.h>

#include <tr1/functional>

#include <swift/base/noncopyable.h>
#include <swift/net/event_handler.h>

struct sockaddr_in;

namespace swift { namespace net
{

class EpollReactor;

class Connector : public EventHandler, public noncopyable
{
public:
    typedef std::tr1::function<void (Connector *c)> SucceedCallback;
    typedef std::tr1::function<void (Connector *c, int error)> FailedCallback;

    Connector(EpollReactor *reactor, const sockaddr_in &peer);
    ~Connector();

    void SetCallback(SucceedCallback succeed, FailedCallback failed)
    {
        on_succeed_ = succeed;
        on_failed_ = failed;
    }

    const static int kConnectFailed = -1;
    const static int kConnectInProgress = 0;
    const static int kConnectSucceed = 1;

    int Connect(int timeout);

    int Fd() { return fd_; }
    EpollReactor *Reactor() { return reactor_; }

    virtual int HandleInput(EpollReactor *)
    {
        //should not happen
        return 0;
    }

    virtual int HandleOutput(EpollReactor *reactor);
    virtual int HandleError(EpollReactor *reactor);
    virtual int HandleHangup(EpollReactor *reactor);

private:
    void Failed(int err);

    EpollReactor *reactor_;
    sockaddr_in peer_;
    int fd_;
    SucceedCallback on_succeed_;
    FailedCallback on_failed_;
};

}}

#endif
