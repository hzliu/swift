// Author: Hongzhang Liu
// Date:

#ifndef SWIFT_NET_LISTENER_H__
#define SWIFT_NET_LISTENER_H__

#include <assert.h>
#include <tr1/functional>

#include <swift/net/event_handler_impl.h>

namespace swift { namespace net
{

class Listener : public EventHandlerImpl
{
public:
    Listener(EpollReactor *reactor, int fd);
    ~Listener();

    typedef std::tr1::function<void (EpollReactor *, int fd)> ConnectionAcceptedCallback;
    void SetConnectionAcceptedCallback(ConnectionAcceptedCallback on_accept);

    typedef std::tr1::function<void ()> FdExceedCallback;
    void SetFdExceedCallback(FdExceedCallback on_fd_exceed);

    int HandleInput(EpollReactor *reactor);
    int HandleOutput(EpollReactor *reactor)
    {
        assert(false && "HandleOutput in Listener called");
        return 0;
    }

    static const int kMaxAcceptInOnLoop = 16;

private:
    int AcceptOnce();
    ConnectionAcceptedCallback on_connection_accept_;
    FdExceedCallback on_fd_exceed_;
};

}}
#endif

