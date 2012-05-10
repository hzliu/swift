#ifndef POSIX_MUTEX_H__
#define POSIX_MUTEX_H__

#include <pthread.h>

#include "noncopyable.h"

class posix_mutex : noncopyable
{
public:
    posix_mutex()
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~posix_mutex()
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

class mutex_guard : noncopyable
{
public:
    mutex_guard(posix_mutex &mutex) : mutex_(mutex) { mutex_.lock(); }
    ~mutex_guard() { mutex_.unlock(); }

private:
    posix_mutex &mutex_;
};


#endif
