#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct __ns_mutex_t
{
    sem_t s1;
    sem_t s2;
    sem_t mutex;
    int q1;
    int q2;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m)
{
    m->q1 = 0;
    m->q2 = 0;
    sem_init(&m->mutex, 1);
    sem_init(&m->s1, 1);
    sem_init(&m->s2, 0);
}

void ns_mutex_acquire(ns_mutex_t *m)
{
    sem_wait(&m->mutex);
    m->q1++;
    sem_post(&m->mutex);

    sem_wait(&m->s1);
    m->q2++;

    sem_wait(&m->mutex);
    m->q1--;

    if (m->q1 == 0)
    {
        sem_post(&m->mutex);
        sem_post(&m->s2);
    }
    else
    {
        sem_post(&m->mutex);
        sem_post(&m->s1);
    }

    sem_wait(&m->s2);
    m->q2--;
}

void ns_mutex_release(ns_mutex_t *m)
{
    if (m->q2 == 0)
    {
        sem_post(&m->s1);
    }
    else
    {
        sem_post(&m->s2);
    }
}

ns_mutex_t mut;
int loops;

void *worker(void *arg)
{
    int i, w;
    w = (int)arg;
    for (i = 0; i < loops; i++)
    {
        ns_mutex_acquire(&mut);
        printf("worker %d on loop %d\n", w, i);
        ns_mutex_release(&mut);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    assert(argc == 3);
    int num_workers = atoi(argv[1]);
    loops = atoi(argv[2]);

    pthread_t pr[num_workers];

    ns_mutex_init(&mut);

    printf("parent: begin\n");

    int i;
    for (i = 0; i < num_workers; i++)
        Pthread_create(&pr[i], NULL, worker, (void *)i);

    for (i = 0; i < num_workers; i++)
        Pthread_join(pr[i], NULL);

    printf("parent: end\n");
    return 0;
}
