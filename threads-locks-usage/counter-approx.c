#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define THREAD_COUNT 10000
#define CEIL 1000000
#define NUMCPUS 8
#define THRESHOLD 50

typedef struct __counter_t
{
    int global;                     // global count
    pthread_mutex_t glock;          // global lock
    int local[NUMCPUS];             // per-CPU count
    pthread_mutex_t llock[NUMCPUS]; // ... and locks
    int threshold;                  // update freq
} counter_t;

typedef struct __counter_args
{
    counter_t *c;
    int threadID;
    int amt;
} counter_args;

// init: record threshold, init locks, init values
// of all local counts and global count
void init(counter_t *c, int threshold)
{
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    int i;
    for (i = 0; i < NUMCPUS; i++)
    {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

// update: usually, just grab local lock and update
// local amount; once it has risen ’threshold’,
// grab global lock and transfer local values to it
void update(counter_t *c, int threadID, int amt)
{
    int cpu = threadID % NUMCPUS;
    pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threshold)
    {
        // transfer to global (assumes amt>0)
        pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    pthread_mutex_unlock(&c->llock[cpu]);
}

// get: just return global amount (approximate)
int get(counter_t *c)
{
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val; // only approximate!
}

void *iterate(void *arg)
{
    counter_args *c = (counter_args *)arg;
    while (get(c->c) < CEIL)
    {
        update(c->c, c->threadID, c->amt);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    struct timeval start, end, tv;
    counter_t *c;
    pthread_t threads[THREAD_COUNT];
    int i;
    counter_args args[THREAD_COUNT];

    c = (counter_t *)malloc(sizeof(counter_t));

    init(c, THRESHOLD);

    gettimeofday(&start, NULL);

    for (i = 0; i < THREAD_COUNT; i++)
    {
        args[i].c = c;
        args[i].threadID = i;
        args[i].amt = 1;
        pthread_create(&threads[i], NULL, iterate, &args[i]);
    }

    for (i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&end, NULL);

    timersub(&start, &end, &tv);
    printf("%f\n", (float)tv.tv_usec);

    free(c);

    return 0;
}