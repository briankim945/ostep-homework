#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: tlbc [number of pages to touch] [number of trials]\n");
        exit(1);
    }
    struct timeval start, end, tv;
    int s, *a;
    long PAGESIZE, NUMPAGES, j, i, reps, jump;
    // float time_dif;
    cpu_set_t cpuset;
    pthread_t thread;

    NUMPAGES = strtol(argv[1], NULL, 10);
    reps = strtol(argv[2], NULL, 10);

    // initialize thread
    thread = pthread_self();

    // Set affinity mask to include CPUs 0 to 7

    CPU_ZERO(&cpuset);
    for (j = 0; j < 8; j++)
        CPU_SET(j, &cpuset);

    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
    {
        printf("Incorrect thread masked\n");
        exit(1);
    }

    // Check the actual affinity mask assigned to the thread

    s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
    {
        printf("Incorrect thread masked\n");
        exit(1);
    }

    // printf("Set returned by pthread_getaffinity_np() contained:\n");
    // for (j = 0; j < CPU_SETSIZE; j++)
    //     if (CPU_ISSET(j, &cpuset))
    //         printf("    CPU %ld\n", j);

    // page size
    PAGESIZE = getpagesize();
    // printf("Page size: %ld\n", PAGESIZE);
    // printf("Total access operations: %ld\n", reps * NUMPAGES);

    // Iterations

    gettimeofday(&start, NULL);
    // printf("Start time: %ld\n", starttime);

    jump = PAGESIZE / sizeof(int);
    // printf("Jump size: %ld\n", jump);
    for (j = 0; j < reps; j++)
    {
        // printf("Starting loop %ld\n", j);
        a = (int *)calloc(NUMPAGES * jump, sizeof(int));
        // printf("Successfully allocated\n");
        for (i = 0; i < NUMPAGES * jump; i += jump)
        {
            a[i] += 1;
            // printf("Successfully touched page %ld\n", i);
        }
        // printf("Successfully touched all pages\n");
        free(a);
        // printf("Completed loop %ld\n", j);
    }

    // printf("Completed loops\n");

    gettimeofday(&end, NULL);

    timersub(&start, &end, &tv);

    // time_dif = ((double)1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)) / (reps * NUMPAGES);

    // printf("Total time took %ld microseconds\n", endtime - starttime);
    // printf("It took %f microseconds per access operation on average\n", (float) tv.tv_usec / (reps * NUMPAGES));
    printf("%ld %f\n", NUMPAGES, (float) tv.tv_usec / (reps * NUMPAGES));
    return 0;
}