#include <stdio.h> /* This is the server code*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <time.h>

#define SERVER_PORT 12345 /* arbitary, but client & server must agree, std is 80, though it may req super-user */
#define TIME 2208988800

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server, client; /* holds IP address */
    socklen_t addr_size;

    /* create socket */

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    /* Build address structure to bind to socket */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    /* Passive open. Wait for connection */

    /* bind and listen */
    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    addr_size = sizeof(client);

    recvfrom(sockfd, NULL, 0, 0, (struct sockaddr *)&client, &addr_size);

    printf("[+]Data received: %s\n", "nothing");

    time_t servertime;
    time(&servertime);
    //uint32_t time = htonl((uint32_t)(servertime + TIME ));

    uint32_t time =servertime + TIME;
    sendto(sockfd, &time, sizeof(time), 0, (struct sockaddr *)&client, addr_size);

    printf("[+]Data send: %u\n", time);

    close(sockfd);
}