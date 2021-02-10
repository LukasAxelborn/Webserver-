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
    key = strtok(NULL, " ");

    char *ret = (char *)calloc(sizeof(char), strlen(key) - 1);

    for (int i = 0; i < strlen(key) - 1; i++)
    {
        ret[i] = key[i + 1];
    }

    //ret[sizeof(ret)] = '\0';
    return ret;
}

char page[] =

    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8; \r\n\r\n"
    "<!DOCTYPE html>\r\n"
    "<html><head><title>Cool-website</title>\r\n"
    "<style>body { background-color: #fc5e03 }</style></head>\r\n"
    "<body><center><h1>Time to party!</h1><br>\r\n"
    "<img src=\"my-server.jpg\"></center></body></html>\r\n";

char error[] =

    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html; charset=UTF-8; \r\n\r\n"
    "<!DOCTYPE html>\r\n"
    "<html><head><title>404 Not Found</title>\r\n"
    "<style>body { background-color: #fc5e03 }</style></head>\r\n"
    "<body><h1>404 File not found</h1><br>\r\n";

void send_file(int sa, int fd)
{
    int bytes;
    char buf[BUF_SIZE];
    while (1)
    {
        printf("line: %d\n", __LINE__);
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

void server()
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

        if (!strcmp(get, "my-server.jpg"))
        {
            printf("line: %d\n", __LINE__);

            fd = open("my-server.jpg", O_RDONLY); //open the file to be sent back
            if (fd < 0)
            {
                fatal("open the file failed");
            }
            int size = get_file_size(fd);

            char picture[] = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: image/jpg\r\n"
                              "Content-Length: 104415\r\n\r\n";

            write(sa, picture, sizeof(picture) - 1);

            send_file(sa, fd); // Send the image to the server

            close(fd);
        }
        else if(!strcmp(get, ""))
        {
            printf("line: %d\n", __LINE__);

            write(sa, page, sizeof(page) - 1);
        }
        else
        {
            write(sa, error, sizeof(error) - 1);
        }
        

        printf("line: %d\n", __LINE__);
        //test(sa);

        printf("%s", buf);

        close(sa); //close connection
    }
}

int main(int argc, char *argv[])
{
    server();
}
