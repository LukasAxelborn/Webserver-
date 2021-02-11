#include <stdio.h> /* This is the server code*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <time.h>

#define SERVER_PORT 12345 /* arbitary, but client & server must agree, std is 80, though it may req super-user */
#define TIME 2208988800

int main(int argc, char **argv)
{

    int sockfd;
    struct sockaddr_in serverAddr;
    uint32_t numbers;
    time_t servertime;
    socklen_t addr_size;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    sendto(sockfd, NULL, 0, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    printf("[+]Data send: %s\n", "nothing");

    addr_size = sizeof(serverAddr);

    recvfrom(sockfd, &numbers, sizeof(numbers), 0, (struct sockaddr *)&serverAddr, &addr_size);

    //servertime = ntohs(numbers-TIME);
    servertime = numbers-TIME;
    printf("size of numbers = %ld\n", sizeof(servertime));

    printf("[+]Data received: %s\n", ctime(&servertime));

    close(sockfd);
}