#ifndef SWIFT_BASE_BLOCKING_QUEUE_H_
#define SWIFT_BASE_BLOCKING_QUEUE_H_

#include <deque>

#include <swift/base/noncopyable.h>
#include <swift/base/condition.h>
#include <swift/base/mutex.h>

namespace swift
{

template<typename T>
class BlockingQueue : noncopyable
{
public:
    BlockingQueue()
        : mutex_(),
        not_empty_(mutex_),
        queue_()
    {
    }

    void put(const T &x)
    {
        mutex_guard lock(mutex_);
        queue_.push_back(x);
        not_empty_.notify(); // TODO: move outside of lock
    }

    T take()
    {
        mutex_guard lock(mutex_);
        while (queue_.empty())
        {
            not_empty_.wait();
        }
        T front(queue_.front());
        queue_.pop_front();
        return front;
    }

    size_t size() const
    {
        mutex_guard lock(mutex_);
        return queue_.size();
    }

private:
    mutable posix_mutex mutex_;
    posix_condition not_empty_;
    std::deque<T>     queue_;
};

}

#endif
