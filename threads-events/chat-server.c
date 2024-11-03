// https://beej.us/guide/bgnet/source/examples/selectserver.c

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "3490"
#define BACKLOG 10
#define EVENT_COUNT 8

int main(int argc, char *argv[])
{
    fd_set master;   // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    int fdmax;       // maximum file descriptor number

    int recv_fd;                         // Receiving socket
    int acc_fd;                          // New accepted socket
    struct sockaddr_storage remote_addr; // Remote address
    socklen_t addrlen;

    char buf[256]; // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1; // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *addr_info, *p;

    void *tmp_addr;

    FD_ZERO(&master); // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &addr_info)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = addr_info; p != NULL; p = p->ai_next)
    {
        recv_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (recv_fd < 0)
        {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(recv_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(recv_fd, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(recv_fd);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL)
    {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(addr_info);

    // listen
    if (listen(recv_fd, 10) == -1)
    {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(recv_fd, &master);

    // keep track of the biggest file descriptor
    fdmax = recv_fd; // so far, it's this one

    while (1)
    {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }

        // loop through existing connections for readable data
        for (i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            { // readable data
                if (i == recv_fd)
                { // handle new connections
                    addrlen = sizeof(remote_addr);
                    if ((acc_fd = accept(recv_fd, (struct sockaddr *)&remote_addr, &addrlen)) == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        FD_SET(acc_fd, &master);
                        if (acc_fd > fdmax)
                        { // reset max fd
                            fdmax = acc_fd;
                        }
                        if (((struct sockaddr *)&remote_addr)->sa_family == AF_INET)
                        { // If IPV4
                            tmp_addr = &(((struct sockaddr_in *)&remote_addr)->sin_addr);
                        }
                        else
                        { // If IPV6
                            tmp_addr = &(((struct sockaddr_in6 *)&remote_addr)->sin6_addr);
                        }
                        printf(
                            "[SERVER] New connection from %s on socket %d\n",
                            inet_ntop(remote_addr.ss_family, tmp_addr, remoteIP, INET6_ADDRSTRLEN),
                            acc_fd);
                    }
                }
                else
                {
                    // handle from existing client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
                    {
                        // got error or connection closed by client
                        if (nbytes == 0)
                        {
                            // connection closed
                            printf("[SERVER] socket %d hung up\n", i);
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(i);           // bye!
                        FD_CLR(i, &master); // remove from master set
                    }
                    else
                    {
                        // we got some data from a client
                        for (j = 0; j <= fdmax; j++)
                        {
                            // send to everyone!
                            if (FD_ISSET(j, &master))
                            {
                                // except the listener and ourselves
                                if (j != recv_fd && j != i)
                                {
                                    if (send(j, buf, nbytes, 0) == -1)
                                    {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}