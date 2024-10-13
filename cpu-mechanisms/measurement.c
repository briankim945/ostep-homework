#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct timeval tv;
    char *buf;
    int starttime, endtime, reps;

    reps = 10000000;

    gettimeofday(&tv, NULL);
    starttime = tv.tv_usec;

    for (int i = 0; i < reps; i++)
    {
        read(STDIN_FILENO, &buf, 0);
    }

    gettimeofday(&tv, NULL);
    endtime = tv.tv_usec;

    printf("It took %f microseconds per read operation on average\n",
           ((double)endtime - starttime) / (double)reps);
    return 0;
}