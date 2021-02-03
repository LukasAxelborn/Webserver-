#include <stdio.h> /* This is the server code*/
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

#define SERVER_PORT 12345 /* arbitary, but client & server must agree, std is 80, though it may req super-user */
#define BUF_SIZE 4096     /* block transfer size */
#define QUEUE_SIZE 10

void fatal(char *string)
{
    printf("%s\n", string);
    exit(1);
}

int main(int argc, char *argv[])
{
    int s, b, l, fd, sa, bytes, on = 1;
    char buf[BUF_SIZE];         /* buffer for outging file */
    struct sockaddr_in channel; /* holds IP address */

    /* Build address structure to bind to socket */
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons(SERVER_PORT);

    /* Passive open. Wait for connection */

    /* create socket */
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0)
        fatal("socket failed");
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

    /* bind and listen */
    b = bind(s, (struct sockaddr *)&channel, sizeof(channel));
    if (b < 0)
        fatal("bind failed");

    l = listen(s, QUEUE_SIZE); //specify queue size
    if (l < 0)
        fatal("listen failed");

    /* socket is now set up and bound. Wait for connection and process it */
    /* inf loop that accepts and handles connections */
    while (1)
    {
        printf("wating for connection on port %d\n", SERVER_PORT);
        fflush(stdout);
        sa = accept(s, 0, 0); //block for connection request, last 2 arg is to get address of whoever connected
        /* sa will be used to interact with the connected client */
        /* meanwhile the socket is still listening */
        if (sa < 0)
            fatal("accept failed");

        char buff[100];
        snprintf((char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");
        write(sa, (char *)buff, strlen((char *)buff));

        read(sa, buf, BUF_SIZE); //read file name from socket

        close(fd); //close file
        close(sa); //close connection
    }
}
