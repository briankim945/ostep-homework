#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define THREAD_COUNT 100
#define CEIL 1000

// basic node structure
typedef struct __node_t
{
    int key;
    struct __node_t *next;
} node_t;

// basic list structure (one used per list)
typedef struct __list_t
{
    node_t *head;
    pthread_mutex_t lock;
} list_t;

typedef struct __list_args
{
    list_t *l;
    int key;
} list_args;

void List_Init(list_t *L)
{
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

int List_Insert(list_t *L, int key)
{
    // synchronization not needed
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL)
    {
        perror("malloc");
        return -1;
    }
    new->key = key;
    // just lock critical section
    pthread_mutex_lock(&L->lock);
    new->next = L->head;
    L->head = new;
    pthread_mutex_unlock(&L->lock);
    return 0; // success
}

int List_Lookup(list_t *L, int key)
{
    int rv = -1;
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while (curr)
    {
        if (curr->key == key)
        {
            rv = 0;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return rv; // now both success and failure
}

int List_Count(list_t *L)
{
    int count = 0;
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    count = 0;
    while (curr)
    {
        count++;
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return count;
}

void *iterate(void *arg)
{
    list_args *l_args = (list_args *)arg;
    list_t *l = l_args->l;
    int key = l_args->key;
    while (List_Count(l) < CEIL)
    {
        if (List_Lookup(l, key) == -1)
        {
            List_Insert(l, key);
        }
        key++;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    struct timeval start, end, tv;
    list_t *l;
    pthread_t threads[THREAD_COUNT];
    int i;
    list_args args[THREAD_COUNT];

    l = (list_t *)malloc(sizeof(list_t));

    List_Init(l);

    gettimeofday(&start, NULL);

    for (i = 0; i < THREAD_COUNT; i++)
    {
        args[i].l = l;
        args[i].key = 0;
        pthread_create(&threads[i], NULL, iterate, &args[i]);
    }

    for (i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&end, NULL);

    timersub(&start, &end, &tv);
    printf("%f\n", (float)tv.tv_usec);

    free(l);

    return 0;
}