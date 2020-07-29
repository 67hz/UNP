/* simple tcp client */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include "unp.h"

#define SA                   const struct sockaddr



int main(int argc, char *argv[])
{
    int sockfd, n;
    int counter = 0;
    char recvline[LINE_MAX + 1];
    struct sockaddr_in servaddr;

    if (argc != 2) {
        perror("Usage: ./daytimetcpcli <IPaddress>");
        exit(EXIT_FAILURE);
    }

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;  /* TCP */
    servaddr.sin_port = htons(9999);      /* daytime server */

    /* presentation to network */
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) == 0) {
        fprintf(stderr, "inet_pton error for %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (connect (sockfd, (SA*) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }
        

    while ( (n = readline(sockfd, recvline, LINE_MAX)) > 0) {
        counter++;
        recvline[n] = 0;         /* null terminate */
        if (fputs(recvline, stdout) == EOF)
            exit(EXIT_FAILURE);
    }

    if (n < 0) {
        perror("Read error");
        exit(EXIT_FAILURE);
    }
        
    printf("Counter: %d\n", counter);


    exit(EXIT_SUCCESS);
}
