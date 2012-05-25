#ifndef SWIFT_NET_TIMER_WHEEL__
#define SWIFT_NET_TIMER_WHEEL__

#include <tr1/functional>
#include <tr1/unordered_map>

#include <swift/base/noncopyable.h>
#include <swift/base/timestamp.h>
#include <swift/base/list.h>
#include <swift/net/timer_id.h>

namespace swift { namespace net
{

typedef std::tr1::function<void ()> TimerCallback;

class TimerWheel : noncopyable
{
public:
    TimerWheel();
    ~TimerWheel();

    TimerId ScheduleTimer(Timestamp when, TimerCallback callback);
    bool CancelTimer(TimerId id);
    void RunUntil(Timestamp now);
    Timestamp NextTimeout();

    static const int kMicrosecondsPerBucket = 5000;  //5 ms
    static const int kTimerBucketCount = 1024;
private:
    struct Timer
    {
        list_head link;
        TimerId id;
        int round;
        TimerCallback callback;

        Timer(TimerId i, TimerCallback c) : id(i), callback(c) {}
    };

    list_head timer_buckets_[kTimerBucketCount];
    int base_index_;
    Timestamp base_;

    typedef std::tr1::unordered_map<TimerId, Timer *> TimerMapType;
    TimerMapType timers_;
    TimerId next_timer_id_;
};

}}

#endif /* end of include guard: SWIFT_NET_TIMER_WHEEL__ */
