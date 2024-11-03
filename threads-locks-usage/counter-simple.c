#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define THREAD_COUNT 10000
#define CEIL 1000000

typedef struct __counter_t
{
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c)
{
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c)
{
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c)
{
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c)
{
    pthread_mutex_lock(&c->lock);
    int rc = c->value;
    pthread_mutex_unlock(&c->lock);
    return rc;
}

void *iterate(void *arg)
{
    counter_t *c = (counter_t *)arg;
    while (get(c) < CEIL)
    {
        increment(c);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    struct timeval start, end, tv;
    counter_t *c;
    pthread_t threads[THREAD_COUNT];
    int i;

    c = (counter_t *)malloc(sizeof(counter_t));

    init(c);

    gettimeofday(&start, NULL);

    for (i = 0; i < THREAD_COUNT; i++)
        pthread_create(&threads[i], NULL, iterate, c);

    for (i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&end, NULL);

    timersub(&start, &end, &tv);
    printf("%f\n", (float)tv.tv_usec);

    free(c);

    return 0;
}