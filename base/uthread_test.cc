#include <cstdio>

#include "uthread.h"

void test_entry(void *arg)
{
    printf("hello, from %s\n", arg);
    uthread_yield();
    printf("hello, i am back: %s\n", arg);
}

void test_entry2(void *arg)
{
    uthread_t *t = (uthread_t *)arg;
    printf("ok, i am waiting for u1 ...\n");
    uthread_wait(t);
    printf("wait ok !\n");
}

int main()
{
    uthread_init();
    uthread_t *u1 = uthread_create(test_entry, (void *)"u1", 4096);
    uthread_create(test_entry2, u1, 4096);
    uthread_create(test_entry, (void *)"u2", 4096);
    uthread_create(test_entry, (void *)"u3", 4096);
    uthread_start();

    return 0;
}
