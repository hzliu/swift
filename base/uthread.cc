#include <cstdlib>
#include <cstring>

#include "list.h"
#include "uthread.h"

struct uthread_context
{
    void **sp;
};


//uthred_t object live in the bottom of stack
struct uthread_t
{
    list_head_t link;
    list_head_t waiting;
    uthread_context context;
};

extern "C"
void __attribute__ ((__noinline__, __regparm__(2)))
uthread_transfer(uthread_context *prev, uthread_context *next);

asm (
     "\t.text\n"
     "\t.globl uthread_transfer\n"
     "uthread_transfer:\n"
     #if __amd64
         #define NUM_SAVED 6
         "\tpushq %rbp\n"
         "\tpushq %rbx\n"
         "\tpushq %r12\n"
         "\tpushq %r13\n"
         "\tpushq %r14\n"
         "\tpushq %r15\n"
         "\tmovq %rsp, (%rdi)\n"
         "\tmovq (%rsi), %rsp\n"
         "\tpopq %r15\n"
         "\tpopq %r14\n"
         "\tpopq %r13\n"
         "\tpopq %r12\n"
         "\tpopq %rbx\n"
         "\tpopq %rbp\n"
     #elif __i386
       #define NUM_SAVED 4
       "\tpushl %ebp\n"
       "\tpushl %ebx\n"
       "\tpushl %esi\n"
       "\tpushl %edi\n"
       "\tmovl %esp, (%eax)\n"
       "\tmovl (%edx), %esp\n"
       "\tpopl %edi\n"
       "\tpopl %esi\n"
       "\tpopl %ebx\n"
       "\tpopl %ebp\n"
     #else
       #error unsupported architecture
     #endif
     "\tret\n"
     );


__thread uthread_context realworld;
__thread uthread_t *uthread_current, *uthread_main;
__thread list_head run_queue;
__thread list_head to_be_destroyed;
__thread list_head timer_queue;


static
void uthread_destroy(uthread_t *t)
{
    list_add(&t->link, &to_be_destroyed);
}

static void uthread_common_entry(
#if __amd64     //these arguments are passed in register
        void *unused1,
        void *unused2,
        void *unused3,
        void *unused4,
        void *unused5,
        void *unused6,
#endif
        uthread_entry entry, void *arg)
{
    entry(arg);
    uthread_t *self = uthread_self();

    list_splice(&self->waiting, run_queue.prev);

    uthread_destroy(self);
    uthread_transfer(&self->context, &uthread_main->context);
}


static
uthread_t *__uthread_create(uthread_entry entry, void *arg, long ssize)
{
    uthread_t *t = (uthread_t *)malloc(ssize);
    t->link.next = t->link.prev = 0;
    INIT_LIST_HEAD(&t->waiting);
    t->context.sp = (void **)(((char *)t) + ssize);
    *--t->context.sp = arg;
    *--t->context.sp = (void *)entry;
    *--t->context.sp = 0;
    *--t->context.sp = (void *)uthread_common_entry;
    t->context.sp -= NUM_SAVED;
    memset(t->context.sp, 0, sizeof(*t->context.sp) * NUM_SAVED);

    return t;
}

uthread_t *uthread_create(uthread_entry entry, void *arg, long ssize)
{
    uthread_t *t = __uthread_create(entry, arg, ssize);
    list_add_tail(&t->link, &run_queue);
    return t;
}

static void uthread_schedule(void *);

void uthread_init()
{
    INIT_LIST_HEAD(&run_queue);
    INIT_LIST_HEAD(&timer_queue);
    INIT_LIST_HEAD(&to_be_destroyed);

    uthread_t *t = (uthread_t *)malloc(4096 * 2);
    t->link.next = t->link.prev = 0;
    t->context.sp = (void **)(((char *)t) + 4096 * 2);
    *--t->context.sp = 0;
    *--t->context.sp = (void *)uthread_schedule;
    t->context.sp -= NUM_SAVED;
    memset(t->context.sp, 0, sizeof(*t->context.sp) * NUM_SAVED);

    uthread_main = t;
    uthread_current = NULL;
}

void uthread_yield()
{
    list_add_tail(&uthread_current->link, &run_queue);
    uthread_transfer(&uthread_current->context, &uthread_main->context);
}


void uthread_start()
{
    uthread_transfer(&realworld, &uthread_main->context);
}

static void clean_up_destroyed()
{
    while(!list_empty(&to_be_destroyed))
    {
        uthread_t *t = list_entry(to_be_destroyed.next, uthread_t, link);
        list_del(to_be_destroyed.next);
        free(t);
    }
}

static void uthread_schedule(void *)
{
    while(!list_empty(&run_queue))
    {
        uthread_current = list_entry(run_queue.next, uthread_t, link);
        list_del(run_queue.next);
        uthread_transfer(&uthread_main->context, &uthread_current->context);
        clean_up_destroyed();
    }

    uthread_transfer(&uthread_main->context, &realworld);
}

uthread_t *uthread_self()
{
    return uthread_current;
}

void uthread_wait(uthread_t *thread)
{
    list_add(&uthread_current->link, &thread->waiting);
    uthread_transfer(&uthread_current->context, &uthread_main->context);
}


