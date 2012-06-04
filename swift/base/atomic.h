#ifndef SWIFT_BASE_ATOMIC_H__
#define SWIFT_BASE_ATOMIC_H__

#include <stdint.h>
#include <swift/base/noncopyable.hpp>

namespace swift
{

namespace detail
{
template<typename T>
    class AtomicIntegerT : noncopyable
{
public:
    AtomicIntegerT()
        : value_(0)
    {
    }

    T Get()
    {
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T GetAndAdd(T x)
    {
        return __sync_fetch_and_add(&value_, x);
    }

    T AddAndGet(T x)
    {
        return __sync_add_and_fetch(&value_, x);
    }

    T IncrementAndGet()
    {
        return AddAndGet(1);
    }

    T DecrementAndGet()
    {
        return AddAndGet(-1);
    }

    void Add(T x)
    {
        GetAndAdd(x);
    }

    void Increment()
    {
        IncrementAndGet();
    }

    void Decrement()
    {
        DecrementAndGet();
    }

    T GetAndSet(T x)
    {
        return __sync_lock_test_and_set(&value_, x);
    }

private:
    volatile T value_;
};
}

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;
}

#endif
