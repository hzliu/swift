#ifndef Condition_H__
#define Condition_H__

#include <pthread.h>

#include <swift/base/noncopyable.h>
#include <swift/base/mutex.h>

namespace swift
{

class Condition : noncopyable
{
public:
    explicit Condition(Mutex& mutex) : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
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
    Mutex &mutex_;
    pthread_cond_t pcond_;
};

}
#endif
