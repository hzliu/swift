#include <swift/net/timer_queue.h>

#include <assert.h>

namespace swift { namespace net
{

TimerQueue::TimerQueue() : next_timer_id_(0)
{
}

TimerQueue::~TimerQueue()
{
    //timer_queue_.clear();
    for(TimerMapType::iterator iter = timers_.begin();
            iter != timers_.end(); ++iter)
    {
        delete iter->second;
    }
}

TimerId TimerQueue::ScheduleTimer(Timestamp when, TimerCallback callback)
{
    TimerId id = ++next_timer_id_;
    Timer *t = new Timer(id, when, callback);
    timers_.insert(std::make_pair(id, t));
    timer_queue_.push(t);

    return id;
}

bool TimerQueue::CancelTimer(TimerId id)
{
    TimerMapType::iterator iter = timers_.find(id);
    if(iter == timers_.end())
        return false;

    iter->second->canceled = true;
    return true;
}

void TimerQueue::RunUntil(Timestamp now)
{
    while(!timer_queue_.empty() && timer_queue_.top()->when <= now)
    {
        Timer *t = timer_queue_.top();
        if(!t->canceled)
            t->callback();
        timers_.erase(t->id);
        timer_queue_.pop();
        delete t;
    }
}

Timestamp TimerQueue::NextTimeout()
{
    assert(!timer_queue_.empty());
    return timer_queue_.top()->when;
}

}}
