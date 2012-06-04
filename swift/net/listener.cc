#include <swift/net/listener.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstring>

namespace swift { namespace net
{

Listener::Listener(EpollReactor *reactor, int fd)
    : EventHandlerImpl(reactor, fd, EPOLLIN)
{
}

Listener::~Listener()
{
}

int Listener::HandleInput(EpollReactor *reactor)
{
    for(int i = 0; i < kMaxAcceptInOnLoop; ++i)
    {
        if(AcceptOnce() < 0)
            break;
    }

    return 0;
}

int Listener::AcceptOnce()
{
    int f = accept(Fd(), NULL, NULL);
    if(f < 0)
    {
        if(errno == EMFILE || errno == ENFILE)
        {
            if(on_fd_exceed_) on_fd_exceed_();
            return -1;
        }

        if(errno != EAGAIN)
        {
            //what can we do for this?
            //TODO: log it
            return -1;
        }

        return -1;
    }

    if(on_connection_accept_)
        on_connection_accept_(Reactor(), f);
    else
        close(f);

    return 0;
}


}}
