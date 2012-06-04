#ifndef SWIFT_BASE_EVENT_HANDLER_H__
#define SWIFT_BASE_EVENT_HANDLER_H__

namespace swift { namespace net
{

class EpollReactor;

struct EventHandler
{
    virtual int HandleInput(EpollReactor *reactor) = 0;
    virtual int HandleOutput(EpollReactor *reactor) = 0;
    virtual int HandleHangup(EpollReactor *reactor) = 0;
    virtual int HandleError(EpollReactor *reactor) = 0;

    virtual ~EventHandler() {}
};

}}

#endif
