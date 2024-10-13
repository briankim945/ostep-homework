#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int)getpid());
    int rc1, rc2;
    int pipefd[2];
    char c;
    char *buf;

    if (pipe(pipefd) == -1)
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
        close(pipefd[0]); /* Close unused read end */

        // child: redirect standard output to a file
        if (dup2(pipefd[1], STDOUT_FILENO) < 0)
        {
            fprintf(stderr, "dup stdout failed\n");
            exit(1);
        }

        printf("7 chars\n");

        close(pipefd[1]); /* Reader will see EOF */

        _exit(EXIT_SUCCESS);
    }
    else
    {
        // parent goes down this path (original process)
        printf("BEFORE NULL\n");

        int wc = wait(NULL);

        printf("PAST NULL\n");

        // new child
        rc2 = fork();

        if (rc2 < 0)
        {
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        }
        else if (rc2 == 0)
        {
            // child (new process)

            /* Wait for first child */

            close(pipefd[1]); /* Close unused write end */

            buf = (char *)calloc(100, sizeof(char));

            int i = 0;
            while (read(pipefd[0], &c, 1) > 0 && c != '\n')
                buf[i++] = c;

            printf("hello, I am child (pid:%d) and I was given a string of length %d\n",
                   (int)getpid(), (int)strlen(buf));
            close(pipefd[0]); /* Close unused read end */
        }
        else
        {
            // parent goes down this path (original process)

            wc = wait(NULL);
            printf("goodbye, I am parent of %d and %d (wc: %d) (pid:%d)\n",
                   rc1, rc2, wc, (int)getpid());
        }
    }
    return 0;
}
