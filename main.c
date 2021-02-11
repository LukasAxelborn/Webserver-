#include <stdio.h> /* This is the server code*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>

#define SERVER_PORT 12345 /* arbitary, but client & server must agree, std is 80, though it may req super-user */
#define BUF_SIZE 4096     /* block transfer size */
#define QUEUE_SIZE 10

void fatal(char *string)
{
    printf("%s\n", string);
    exit(1);
}

char *parse_http_request(char *request)
{
  	char *key;
    char temp[strlen(request)];

    strcpy(temp, request);
    key = strtok(temp, " ");
    key = strtok(NULL, "/%22");

    char *ret = (char *)calloc(sizeof(char), strlen(key) - 1);

    for (int i = 0; i < strlen(key); i++)
    {
        ret[i] = key[i];
    }

    return ret;
}

void send_file(int sa, int fd)
{
    int bytes;
    char buf[BUF_SIZE];
    while (1)
    {
        bytes = read(fd, buf, BUF_SIZE);
        if (bytes <= 0)
            break;

        write(sa, buf, bytes);
    }
}

int get_file_size(int fd)
{
    struct stat buf;
    fstat(fd, &buf);
    return buf.st_size;
}

int main(int argc, char *argv[])
{
    int s, b, l, sa, fd, bytes, on = 1;
    char buf[BUF_SIZE], tmp[BUF_SIZE]; /* buffer for outging file */
    struct sockaddr_in channel;        /* holds IP address */

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
        char *get = parse_http_request(buf);

        if (!strcmp(get, " HTTP"))
        {
            fd = open("index.html", O_RDONLY); //open the file to be sent back
            if (fd < 0)
            {
                fatal("open the file failed");
            }

            char header[] = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html; charset=UTF-8; \r\n\r\n";


            write(sa, header, sizeof(header) - 1);
            send_file(sa, fd);
            close(fd);
        }
        else if (!strcmp(get, "my-server.jpg"))
        {

            fd = open(get, O_RDONLY); //open the file to be sent back
            if (fd < 0)
            {
                fatal("open the file failed");
            }
            int size = get_file_size(fd);

            char header[] = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: image/jpg\r\n"
                             "Content-Length: 104415\r\n\r\n";

            write(sa, header, sizeof(header) - 1);

            send_file(sa, fd); // Send the image to the server

            close(fd);
        }
          else if (!strcmp(get, "hallelu.gif"))
        {
            fd = open(get, O_RDONLY); //open the file to be sent back
            if (fd < 0)
            {
                fatal("open the file failed");
            }
            int size = get_file_size(fd);

            char picture[] = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: image/gif\r\n"
                             "Content-Length: 2434610\r\n\r\n";

            write(sa, picture, sizeof(picture) - 1);

            send_file(sa, fd); // Send the image to the server

            close(fd);
        } 
        else if (!strcmp(get, "hund.jpg"))
        {
            fd = open(get, O_RDONLY); //open the file to be sent back
            if (fd < 0)
            {
                fatal("open the file failed");
            }
            int size = get_file_size(fd);

            char picture[] = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: image/jpg\r\n"
                             "Content-Length: 360319\r\n\r\n";

            write(sa, picture, sizeof(picture) - 1);

            send_file(sa, fd); // Send the image to the server

            close(fd);
        }
        else
        {
            fd = open("error.html", O_RDONLY); //open the file to be sent back
            if (fd < 0)
            {
                fatal("open the file failed");
            }

            char header[] = "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html; charset=UTF-8; \r\n\r\n";


            write(sa, header, sizeof(header) - 1);
            send_file(sa, fd);
            close(fd);
        }

        printf("%s", buf);

        free(get);
        close(sa); //close connection
    }
}
