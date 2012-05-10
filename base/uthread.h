// Author: Hongzhang Liu
// Date:

#ifndef UTHREAD_H__
#define UTHREAD_H__

struct uthread_t;

typedef void (*uthread_entry)(void *);

void uthread_init();

void uthread_start();

uthread_t *uthread_create(uthread_entry entry, void *arg, long ssize);

void uthread_sleep(int seconds);

void uthread_yield();

uthread_t *uthread_self();

struct uthread_event_t;
uthread_event_t *uthread_event_create();
void uthread_event_wait(uthread_event_t *event);
void uthread_event_signal(uthread_event_t *event);
void uthread_event_signal_all(uthread_event_t *event);
void uthread_event_destroy(uthread_event_t *event);

void uthread_wait(uthread_t *thread);


#endif

