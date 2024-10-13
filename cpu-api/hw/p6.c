#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int)getpid());
    int rc = fork(), status;
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
        waitpid(rc, &status, 0);
        // parent goes down this path (original process)
        printf("goodbye, I am parent of %d (wc: %d) (pid:%d)\n",
               rc, status, (int)getpid());
    }
    return 0;
}
