#include <netinet/in.h>
#include <errno.h>

#include "connector.h"
#include "sock_util.h"

connector::connector(epoll_reactor *reactor, IConnectorNotifier *notifier) :
    m_reactor(reactor), m_fd(-1), m_notifier(notifier)
{
}

connector::~connector()
{
    delete m_notifier;
}

int connector::connect(const sockaddr_in &remoteAddr)
{
    m_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(m_fd < 0)
        return CONNECT_FAILED;

    set_nonblocking(m_fd);
    set_reuseaddr(m_fd);

    int rtn = ::connect(m_fd, (const sockaddr *)&remoteAddr,
            sizeof(remoteAddr));
    if(rtn == 0)
    {
        m_notifier->ConnectSucceed(this);
        delete this;
        return CONNECT_SUCCEED;
    }

    if(rtn < 0 && errno != EINPROGRESS)
    {
        close(m_fd);
        m_fd = -1;
        m_notifier->ConnectFailed(this, errno);
        delete this;
        return CONNECT_FAILED;
    }

    m_reactor->attach(m_fd, this, EPOLLOUT);
    return CONNECT_IN_PROGRESS;
}

int connector::handle_output(epoll_reactor *reactor)
{
    int err = 0;
    socklen_t len = sizeof(err);
    int rtn = getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &err, &len);
    if(rtn < 0 || err != 0)
    {
        m_notifier->ConnectFailed(this, err);
        close(m_fd);
        delete this;
        return -1;
    }

    reactor->detach(m_fd);
    m_notifier->ConnectSucceed(this);
    delete this;
    return -1;
}

int connector::handle_error(epoll_reactor *reactor)
{
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &err, &len);
    m_notifier->ConnectFailed(this, err);

    close(m_fd);
    delete this;
    return -1;
}

int connector::handle_hangup(epoll_reactor *reactor)
{
    m_notifier->ConnectFailed(this, -1);
    close(m_fd);
    delete this;
    return -1;
}



