#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int)getpid());
    char *buf;
    int sz, fd = open("./message.txt", O_APPEND | O_RDWR);
    if (fd < 0)
    {
        perror("r1");
        exit(1);
    }
    int bound = 8;
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
        buf = (char *)calloc(100, sizeof(char));
        sz = read(fd, buf, bound);
        if (sz < 0)
        {
            perror("r1");
            exit(1);
        }
        write(fd, " child ", 7);
        printf("hello, I am child (pid:%d), the file pointer is %d, %d bytes were read, and the file content is %s\n",
               (int)getpid(), fd, sz, buf);
        close(fd);
    }
    else
    {
        // parent goes down this path (original process)
        buf = (char *)calloc(100, sizeof(char));
        sz = read(fd, buf, bound);
        if (sz < 0)
        {
            perror("r1");
            exit(1);
        }
        write(fd, " parent ", 8);
        printf("hello, I am parent of %d (pid:%d), the file pointer is %d, %d bytes were read, and the file content is %s\n",
               rc, (int)getpid(), fd, sz, buf);
        close(fd);
    }
    return 0;
}
