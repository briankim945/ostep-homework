#include <assert.h>
#include <pthread.h>
#include "semaphore.h"

// only one thread can call this
void sem_init(sem_t *s, int value)
{
    s->value = value;
    assert(pthread_cond_init(&s->cond, NULL) == 0);
    assert(pthread_mutex_init(&s->lock, NULL) == 0);
}

void sem_wait(sem_t *s)
{
    assert(pthread_mutex_lock(&s->lock) == 0);
    while (s->value <= 0)
        assert(pthread_cond_wait(&s->cond, &s->lock) == 0);
    s->value--;
    assert(pthread_mutex_unlock(&s->lock) == 0);
}

void sem_post(sem_t *s)
{
    assert(pthread_mutex_lock(&s->lock) == 0);
    s->value++;
    assert(pthread_cond_signal(&s->cond) == 0);
    assert(pthread_mutex_unlock(&s->lock) == 0);
}
