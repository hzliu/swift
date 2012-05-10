#ifndef POSIX_CONDITION_H__
#define POSIX_CONDITION_H__

#include <pthread.h>

#include "mutex.h"
#include "noncopyable.h"

class posix_condition : noncopyable
{
public:
    explicit posix_condition(posix_mutex& mutex) : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~posix_condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.get_pthread_mutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notify_all()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    posix_mutex &mutex_;
    pthread_cond_t pcond_;
};


#endif
