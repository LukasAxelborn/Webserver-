#include <stdio.h>				/* This is the server code*/
#include<sys/types.h>
#include<sys/fcntl.h>
#include <netinet/in.h>
#include<netdb.h>

#define SERVER_PORT 12345 		/* arbitary, but client & server must agree */
#define BUF_SIZE 4096 			/* block transfer size */
#define QUEUE_SIZE 10

int main(int argc, char * argv[]){
	int s, b, l, sa, bytes, on = 1;
	char buf[BUF_SIZE];			/* buffer for outging file */ 
	struct sockaddr_in channel;	/* holds IP address */

	/* Build address structure to bind to socket */
	memset(&channel, 0, sizeof(channel));
	channel.sin_family = AF_INET;
	channel.sin_addr.s_addr = htonl(INADDER_ANY);
	channel.sin_port = htons(SERVER_PORT);

	/* Passive open. Wait for connection */

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s < 0) fatal("socket failed");
	setsockopt(s, SOL_SOCKET, SO_REUSEADDER, (char *)&on,sizeof(on));

    b = bind(s, (struct socketaddr *) &channel, sizeof(channel));
    if(b < 0) fatal("bind failed");

    l = listen(s, QUEUE_SIZE);              //specify queue size
    if(l < 0) fatal("listen failed");

    /* socket is now set up and bound. Wait for connection and process it */
    while(1)
    {
        sa = accept(s, 0, 0);               //block for connection request
        if(sa < 0) fatal("accept failed");

        read(sa, buf, BUF_SIZE);            //read file name from socket

        /* get and return the file */
        fd = open(buf, O_RDONLY);           //open the file to be sent back
        if(fd < 0) fatal("open failed");

        while(1)
        {
            bytes = read(fd, buf, BUF_SIZE);    //read from file
            if(bytes <= 0) break;               //check for eof
            write(sa, buf, bytes);              //write bytes to socket
        }
        close(fd);                              //close file
        close(sa);                              //close connection
    }


