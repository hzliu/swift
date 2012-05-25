#ifndef SWIFT_NET_TIMER_QUEUE_H__
#define SWIFT_NET_TIMER_QUEUE_H__

#include <stdint.h>
#include <queue>
#include <vector>
#include <tr1/functional>
#include <tr1/unordered_map>

#include <swift/base/noncopyable.h>
#include <swift/base/timestamp.h>
#include <swift/net/timer_id.h>

namespace swift { namespace net
{

typedef std::tr1::function<void ()> TimerCallback;

class TimerQueue : public noncopyable
{
public:
    TimerQueue();
    ~TimerQueue();

    /**
     * add a timer to TimerQueue
     * @param callback
     * @param when
     * @return id
     */
    TimerId ScheduleTimer(Timestamp when, TimerCallback callback);
    bool CancelTimer(TimerId id);
    void RunUntil(Timestamp now);
    Timestamp NextTimeout();

private:
    TimerId next_timer_id_;

    struct Timer
    {
        TimerId id;
        Timestamp when;
        TimerCallback callback;
        bool canceled;

        Timer(TimerId i, Timestamp w, TimerCallback c)
            : id(i), when(w), callback(c), canceled(false)
        {}
    };

    struct TimerComparer
    {
        bool operator()(Timer *lhs, Timer *rhs) const
        {
            return rhs->when < lhs->when;
        }
    };

    typedef std::tr1::unordered_map<TimerId, Timer *> TimerMapType;
    TimerMapType timers_;
    std::priority_queue<Timer *, std::vector<Timer *>, TimerComparer> timer_queue_;
};

}}

#endif
