#include <swift/net/timer_wheel.h>

#include <assert.h>

namespace swift { namespace net
{

TimerWheel::TimerWheel()
    : base_index_(0), base_(Timestamp::Now()), next_timer_id_(0)
{
    for(int i = 0; i < kTimerBucketCount; ++i)
    {
        INIT_LIST_HEAD(&timer_buckets_[i]);
    }
}

TimerWheel::~TimerWheel()
{
    for(TimerMapType::iterator iter = timers_.begin();
            iter != timers_.end(); ++iter)
    {
        delete iter->second;
    }
}

TimerId TimerWheel::ScheduleTimer(Timestamp when, TimerCallback callback)
{
    int64_t microseconds =
        when.MicrosecondsSinceEpoch() - base_.MicrosecondsSinceEpoch();
    int time_in_bucket = static_cast<int>(
        (microseconds + kMicrosecondsPerBucket - 1) / kMicrosecondsPerBucket);

    TimerId id = ++next_timer_id_;
    Timer *t = new Timer(id, callback);
    t->round = time_in_bucket / kTimerBucketCount;
    int index = (time_in_bucket + base_index_) % kTimerBucketCount;
    list_add_tail(&t->link, &timer_buckets_[index]);
    timers_.insert(std::make_pair(id, t));

    return id;
}

bool TimerWheel::CancelTimer(TimerId id)
{
    TimerMapType::iterator iter = timers_.find(id);
    if(iter == timers_.end())
        return false;

    Timer *t = iter->second;
    timers_.erase(iter);
    list_del(&t->link);
    delete t;

    return true;
}

void TimerWheel::RunUntil(Timestamp now)
{
    int64_t microseconds =
        now.MicrosecondsSinceEpoch() - base_.MicrosecondsSinceEpoch();
    int buckets = static_cast<int>(microseconds / kMicrosecondsPerBucket);
    base_ = Timestamp(
            base_.MicrosecondsSinceEpoch() + buckets * kMicrosecondsPerBucket);
    for(int i = 0; i < buckets; ++i)
    {
        ++base_index_;
        if(base_index_ >= kTimerBucketCount)
            base_index_ = 0;

        list_head *pos;
        list_head *n;
        list_for_each_safe(pos, n, &timer_buckets_[base_index_])
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
            Timer *t = list_entry(pos, Timer, link);
#pragma GCC diagnostic pop
            if(t->round > 0)
            {
                --t->round;
                continue;
            }

            list_del(pos);
            t->callback();
            timers_.erase(t->id);
            delete t;
        }
    }
}

Timestamp TimerWheel::NextTimeout()
{
    if(timers_.empty())
        return Timestamp::SecondsLater(3);

    for(int i = 1; i <= kTimerBucketCount; ++i)
    {
        int index = (base_index_ + i) % kTimerBucketCount;
        if(!list_empty(&timer_buckets_[index]))
            return Timestamp(
                    base_.MicrosecondsSinceEpoch() + kMicrosecondsPerBucket * i);
    }

    assert(false && "non-empty timer wheel should not got here");
    return Timestamp::SecondsLater(3);
}

}}
