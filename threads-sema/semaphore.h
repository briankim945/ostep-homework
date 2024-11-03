#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

typedef struct __sem_t
{
    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} sem_t;

void sem_init(sem_t *s, int value);

void sem_wait(sem_t *s);

void sem_post(sem_t *s);

#endif