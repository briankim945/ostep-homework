#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile int child_status = 0;

void sigchild_handler(int sig)
{
    child_status = 1;
}

int main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int)getpid());
    int rc = fork(), status = 1;

    // Triggers when child status changes
    signal(SIGCHLD, sigchild_handler);
    if (rc < 0)
    {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int)getpid());
    }
    else
    {
        // Waiting without wait()
        while (child_status != 1)
        {
        };
        // parent goes down this path (original process)
        printf("goodbye, I am parent of %d (pid:%d)\n",
               rc, (int)getpid());
    }
    return 0;
}
