#ifndef SWIFT_NET_CONNECTOR_H__
#define SWIFT_NET_CONNECTOR_H__

#include <swift/base/noncopyable.h>
#include <swift/net/epoll_reactor.h>

struct sockaddr_in;

namespace swift { namespace net
{

class Connector;
struct IConnectorNotifier
{
    virtual void ConnectFailed(Connector *c, int error) = 0;
    virtual void ConnectSucceed(Connector *c) = 0;
    virtual ~IConnectorNotifier() {}
};

class Connector : public event_handler, public noncopyable
{
public:
    Connector(epoll_reactor *reactor, IConnectorNotifier *notifier);

    ~Connector();

    const static int CONNECT_FAILED = -1;
    const static int CONNECT_IN_PROGRESS = 0;
    const static int CONNECT_SUCCEED = 1;

    int connect(const sockaddr_in &remoteAddr);

    int fd() { return m_fd; }
    epoll_reactor *reactor() { return m_reactor; }

    int handle_input(epoll_reactor *)
    {
        //should not happen
        return 0;
    }

    int handle_output(epoll_reactor *reactor);
    int handle_error(epoll_reactor *reactor);
    int handle_hangup(epoll_reactor *reactor);

private:
    epoll_reactor *m_reactor;
    int m_fd;
    IConnectorNotifier *m_notifier;
};


}}

#endif
