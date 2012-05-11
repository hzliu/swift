#ifndef SWIFT_BASE_MUTEX_H__
#define SWIFT_BASE_MUTEX_H__

#include <pthread.h>

#include <swift/base/noncopyable.h>

namespace swift
{

class Mutex : noncopyable
{
public:
    Mutex()
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&mutex_);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* get_pthread_mutex() /* non-const */
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
};

class MutexGuard : noncopyable
{
public:
    MutexGuard(Mutex &mutex) : mutex_(mutex) { mutex_.lock(); }
    ~MutexGuard() { mutex_.unlock(); }

private:
    Mutex &mutex_;
};

}
#endif
