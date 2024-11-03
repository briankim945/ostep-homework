#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "3490"
#define BACKLOG 10
#define EVENT_COUNT 8

int main(int argc, char *argv[])
{
    int status, sock_fd, recv_fd, len, bytes_sent;
    struct addrinfo hints;
    struct addrinfo *servinfo; // will point to the results
    struct sockaddr_storage recv_addr;
    socklen_t addr_size;
    struct timeval tv;
    struct timezone tz;
    struct tm *today;
    char msg[64], buffer[32];

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // servinfo now points to a linked list of 1 or more struct addrinfos

    // ... do everything until you don't need servinfo anymore ....

    if ((sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        exit(1);
    }

    if (bind(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        exit(1);
    }

    if (listen(sock_fd, BACKLOG) == -1)
    {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        exit(1);
    }

    addr_size = sizeof(recv_addr);
    if ((recv_fd = accept(sock_fd, (struct sockaddr *)&recv_addr, &addr_size)) == -1)
    {
        fprintf(stderr, "accept error: %s\n", strerror(errno));
        exit(1);
    }

    while (recv(recv_fd, buffer, sizeof buffer, 0) > 0)
    {
        printf("%d\n", sizeof(buffer));
        gettimeofday(&tv, &tz);
        today = localtime(&tv.tv_sec);
        snprintf(
            msg,
            sizeof msg,
            "Hello, world! The time is %d:%0d:%0d.%d\n",
            today->tm_hour,
            today->tm_min,
            today->tm_sec,
            tv.tv_usec);

        len = strlen(msg);
        if ((bytes_sent = send(recv_fd, msg, len, 0)) == -1)
        {
            fprintf(stderr, "send error: %s\n", strerror(errno));
            exit(1);
        }
    }

    close(sock_fd);

    freeaddrinfo(servinfo); // free the linked-list

    return 0;
}
