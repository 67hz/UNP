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
    pid_t childpid;
    socklen_t clilen;
    ssize_t nread;
    struct sockaddr_in servaddr, cliaddr;

    char buf[LINE_MAX];
    char chat_buf[LINE_MAX];

    time_t ticks;
    ssize_t ret;

    if ( (listenfd = Socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(EXIT_FAILURE);

    memset (&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* host to network long */
    servaddr.sin_port = htons(SERV_PORT);  /* host to network short - dayime server */

    if (bind (listenfd,
                (const struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* backlog set to 20 */
    listen(listenfd, 20);

    for (;;) {
        clilen = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) == -1)
            err_sys("accept error");
        int client_port = ntohs(cliaddr.sin_port);

        if ( (childpid = fork()) == 0) {                /* child */
            if (close(listenfd) == -1)        /* close listener in child */
                err_sys("child close error");

            /*printf("Family: %d\n", Sockfd_to_family(listenfd));*/
            printf("%s, port %d has entered the room\n",
                    inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)),
                    client_port);



            ticks = time(NULL);
            snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));

            for (;;) {
                /* blocking until line sent from client*/
                if ( (nread = readline(connfd, chat_buf, LINE_MAX)) == 0) { /* conn closed by other end */
                    printf("%d has left @ %s\n", client_port, buf);
                    break;
                }

                printf("%s:%d: %s\n", buf, client_port, chat_buf);

                Writen(connfd, chat_buf, nread);
            }

            exit(EXIT_SUCCESS);         /* exit child process */
        }

        if (close(connfd) == -1) {      /* parent closes conn */
            perror("close connfd");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);

}


