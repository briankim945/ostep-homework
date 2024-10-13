#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#define REPS 1e8
#define MILLION 1e6

// sched_setaffinity does not exist on MacOS

int main(int argc, char *argv[])
{
    struct timeval start_tv, end_tv;
    char *buf;
    int rc1, rc2, sc;
    int pipefd_1[2], pipefd_2[2];
    char c1, c2;

    // cpu_set_t my_set;                                      /* Define your cpu_set bit mask. */
    // CPU_ZERO(&my_set);                                     /* Initialize it all to 0, i.e. no CPUs selected. */
    // CPU_SET(7, &my_set);                                   /* set the bit that represents core 7. */
    // sc = sched_setaffinity(0, sizeof(cpu_set_t), &my_set); /* Set affinity of tihs process to */
    /* the defined mask, i.e. only 7. */
    /*if (sc < 0)
    {
        perror("cpu set");
        exit(EXIT_FAILURE);
    }*/

    if (pipe(pipefd_1) < 0)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd_2) < 0)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    rc1 = fork();

    if (rc1 < 0)
    {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc1 == 0)
    {
        // child (new process)

        close(pipefd_1[0]); /* Close unused read end */
        close(pipefd_2[1]); /* Close unused write end */

        for (int i = 0; i < REPS; ++i)
        {
            while (read(pipefd_2[0], &c1, 1) <= 0)
            {
            } /* read to the first pipe */
            write(pipefd_1[1], &c1, 1); /* write to the first pipe */
        }

        close(pipefd_2[0]);
        close(pipefd_1[1]);

        exit(EXIT_SUCCESS);
    }
    else
    {
        // parent goes down this path (original process)

        close(pipefd_2[0]); /* Close unused read end */
        close(pipefd_1[1]); /* Close unused write end */

        gettimeofday(&end_tv, NULL);

        for (int i = 0; i < REPS; i++)
        {
            read(STDIN_FILENO, &buf, 0);
        }

        gettimeofday(&start_tv, NULL);

        close(pipefd_1[0]);
        close(pipefd_2[1]);

        printf("It took %f microseconds on average to context switch\n",
               (float)(start_tv.tv_sec * MILLION + start_tv.tv_usec - start_tv.tv_sec * MILLION -
                       start_tv.tv_usec) /
                   REPS / 2);

        exit(EXIT_SUCCESS);
    }
    return 0;
}