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

char* parse_http_request(char* request){

    char *key;
    char temp[strlen(request)];

    strcpy(temp, request);
    key = strtok(temp, " ");
    key = strtok(NULL, " ");

    char *ret = (char*)malloc(sizeof(key)-1);

    for (int i = 0; i < strlen(key); i++)
    {
        ret[i] = key[i+1];  
    }

    //ret[sizeof(ret)] = '\0';
    return ret;
}

unsigned long fsize(char* file)
{
    FILE * f = fopen(file, "r");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}

int main(int argc, char *argv[])
{
    int s, b, l, fd, sa, bytes, on = 1;
    char buf[BUF_SIZE], tmp[BUF_SIZE];         /* buffer for outging file */
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
        
        memset(buf, 0, BUF_SIZE);
        read(sa, buf, BUF_SIZE); //read file name from socket


        // funktionen retunerar en sträng om addrese till filen den vill läsa      
        char* filename = parse_http_request(buf);


        /* get and return the file */
        fd = open(filename, O_RDONLY); //open the file to be sent back
        if (fd < 0)
            fatal("open failed");

        long filesize = fsize(filename);

        while (1)
        {
            bytes = read(fd, filename, filesize); //read from file
            if (bytes <= 0)
                break;             //check for eof
            write(sa, filename, filesize); //write bytes to socket
        }

        char buff[100];
        snprintf((char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nLukas Invest AB");
        write(sa, (char *)buff, strlen((char *)buff));

        printf("%s", buf);


        close(sa); //close connection
    }
}
