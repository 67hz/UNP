#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include "unp.h"


int main(int argc, char *argv[])
{

    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    char buf[LINE_MAX];
    time_t ticks;
    ssize_t ret;
    ssize_t count = 0;

    if ( (listenfd = Socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(EXIT_FAILURE);

    memset (&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* host to network long */
    servaddr.sin_port = htons(9999);  /* host to network short - dayime server */

    if (bind (listenfd,
                (const struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* backlog set to 20 */
    listen(listenfd, 20);

    for (;;) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
        printf("Connection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)),
                ntohs(cliaddr.sin_port));

        printf("Family: %d\n", Sockfd_to_family(listenfd));

        ticks = time(NULL);
        
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));

        for (int i = 0; i < strlen(buf); i++) {

            ret = write(connfd, &buf[i], 1);
            if (ret == -1) {
                /* if (errno == EINTR) */
                /*     continue; */

                perror("write");
                break;
            }
             
            count += ret;
        }

        if (close(connfd) == -1) {
            perror("close connfd");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);


}


