#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int)getpid());
    int rc = fork();
    if (rc < 0)
    {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        // child (new process)
        // now exec "wc"...
        char *path;
        char *myargs[2];
        char *envp[2];

        path = strdup("/bin/ls");

        myargs[0] = strdup("ls"); // program: "ls" (list files)
        myargs[1] = NULL;         // marks end of array

        envp[0] = "environment";
        envp[1] = NULL;

        // execl(path, myargs[0], myargs[1]); // runs /bin/ls
        // execle(path, myargs[0], myargs[1], envp); // runs /bin/ls
        // execlp(path, myargs[0], myargs[1]); // runs /bin/ls
        // execv(path, myargs); // runs /bin/ls
        execvp(myargs[0], myargs); // runs /bin/ls

        // NOT SUPPORTED ON MACOS
        // execvpe(myargs[0], myargs, envp); // runs /bin/ls

        printf("this shouldn't print out");
    }
    else
    {
        // parent goes down this path (original process)
        int wc = wait(NULL);
        printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",
               rc, wc, (int)getpid());
    }
    return 0;
}
