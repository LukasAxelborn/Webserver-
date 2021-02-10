#include <stdio.h> /* This is the server code*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 12345 /* arbitary, but client & server must agree, std is 80, though it may req super-user */
#define BUF_SIZE 1024     /* block transfer size */
#define QUEUE_SIZE 10

void fatal(char *string)
{
    printf("%s\n", string);
    exit(1);
}

int main(int argc, char *argv[])
{
    int s, b, l, sa, on = 1;
    char buf[BUF_SIZE], tmp[BUF_SIZE]; /* buffer for outging file */
    struct sockaddr_in server, client; /* holds IP address */
    socklen_t client_size;

    /* create socket */

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        fatal("socket failed");

    /* Build address structure to bind to socket */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_PORT);

    /* Passive open. Wait for connection */

    //setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

    /* bind and listen */
    b = bind(s, (struct sockaddr *)&server, sizeof(server));
    if (b < 0)
        fatal("bind failed");

    client_size = sizeof(client);

    recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &client_size);
    printf("[+]Data received: %s", buf);
    sendto(s, buf, BUF_SIZE, 0, (struct sockaddr *)&client, client_size);
    printf("[+]Data received: %s", buf);
}