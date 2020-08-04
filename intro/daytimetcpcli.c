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
    int sockfd;
    char recvline[LINE_MAX + 1];
    char sendline[LINE_MAX + 1];
    socklen_t cli_len;
    struct sockaddr cliaddr;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        perror("Usage: ./daytimetcpcli <server IPaddress>");
        exit(EXIT_FAILURE);
    }

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;  /* TCP */
    servaddr.sin_port = htons(SERV_PORT);      /* daytime server */

    /* presentation to network */
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) == 0) {
        fprintf(stderr, "inet_pton error for %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (connect (sockfd, (SA*) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }


    /* @TODO implement sock_ntop */
    char str[INET_ADDRSTRLEN];
    (void) getsockname(sockfd, &cliaddr, &cli_len);
    printf("getsockname: %d\n", cliaddr.sa_family);

    inet_ntop(AF_INET, &cliaddr, str, sizeof(str));
    printf("inet_ntop %s \n", str);

    /* write STDIN to server, block*/
    while (fgets(sendline, LINE_MAX, stdin) != NULL) {
        Writen(sockfd, sendline, 1); /* get RST */
        sleep(1);


        Writen(sockfd, sendline+1, strlen(sendline) - 1); /* gen SIGPIPE */
#if 0
        Writen(sockfd, sendline, strlen(sendline)); /* gen SIGPIPE */

#endif
        /* read back, readline null terminates */
        if (readline(sockfd, recvline, LINE_MAX) == 0) {
            printf("server terminated prematurely...\n");
            err_sys("server terminated prematurely");
        }

        fputs(recvline, stdout);
    }

    exit(EXIT_SUCCESS);
}
