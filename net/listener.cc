#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstring>

#include "listener.h"

listener::listener(epoll_reactor *reactor, int fd, IWorkerFactory *factory) :
    event_handler_impl(reactor, fd, EPOLLIN), m_factory(factory)
{
    m_nullFd = open("/dev/null", O_RDONLY);
}

listener::~listener()
{
    close(m_nullFd);
}

int listener::handle_input(epoll_reactor *reactor)
{
    bool fdExceeds = false;
    for(int i = 0; i < 16; ++i)
    {
        int f = accept(fd(), NULL, NULL);
        if(f < 0)
        {
            if(errno == EMFILE || errno == ENFILE)
            {
                if(m_factory->OnFdExceeds(this) == 0)
                    continue;

                close(m_nullFd);
                fdExceeds = true;
                continue;
            }

            if(errno != EAGAIN)
            {
                m_factory->OnAcceptError(this);
                return -1;
            }

            return 0;
        }

        if(fdExceeds)
        {
            //log_error("fd limit exceeds, close connections ...");
            close(f);
            continue;
        }

        m_factory->OnFdAccepted(reactor, f);
    }

    if(fdExceeds)
    {
        m_nullFd = open("/dev/null", O_RDONLY);
    }

    return 0;
}

