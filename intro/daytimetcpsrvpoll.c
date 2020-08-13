#include "unp.h"
#include <sys/poll.h>
#include <unistd.h>

#ifdef OPEN_MAX
static int openmax = OPEN_MAX;
#else
static int openmax = 0;
#endif

#define OPEN_MAX_GUESS          256

static int open_max(void)
{
    if (openmax == 0) {                         /* first time through */
        errno = 0;
        if ( (openmax = sysconf(_SC_OPEN_MAX)) < 0) {
            if (errno == 0)
                openmax = OPEN_MAX_GUESS;       /* indeterminate */
            else
                err_sys("sysconf error for _SC_OPEN_MAX");
        }
    }
    return openmax;
}

int main (int argc, char **argv)
{
    int i, listenfd, connfd, sockfd;
    nfds_t max_client_index;
    int nready;
    ssize_t n;
    char line[LINE_MAX];
    socklen_t clilen;
    int openmax = open_max();
    struct pollfd client[openmax];
    struct sockaddr_in cliaddr, servaddr;



    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
        err_sys("bind");

    Listen(listenfd, LISTENQ);

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;

    /* make all clients available */
    for (i = 1; i < openmax; i++) {
        client[i].fd = -1;  /* -1 <- available entry */
    }

    max_client_index = 0;   /* max index into client array */

    for (;;) {
        printf("\nfor.. %d\n", i);
        if ( (nready = poll(client, max_client_index + 1, -1)) < 0)
            err_sys("poll");

        if (client[0].revents & POLLRDNORM) {  /* new client conn */
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);


            for (i = 1; i < openmax; i++)
                if (client[i].fd < 0) {     /* fill 1st available fd */
                    client[i].fd = connfd;
                    break;
                }

            if (i == openmax)
                err_quit("too many clients");


            client[i].events = POLLRDNORM;
            if (i > max_client_index)
                max_client_index = i;

            if (--nready <= 0)
                continue;   /* no more readable descriptors */

        }


        for (i = 1; i <= max_client_index; i++) {  /* check clients for data */
            printf("checking %d\n", i);
            if ( (sockfd = client[i].fd) < 0)
                continue;

            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                if ( (n = readline(sockfd, line, LINE_MAX)) < 0) {
                    if (errno == ECONNRESET) {       /* conn reset by client */
                        if (close(sockfd) == -1)
                            err_sys("close error");
                        printf("conn reset");
                        client[i].fd = -1;
                    } else
                        err_sys("readline error");
                } else if (n == 0) {                /* conn closed by client */
                    if (close(sockfd) == -1)
                        err_sys("close error");

                    printf("conn closed");

                    client[i].fd = -1;
                } else {
                    Writen(sockfd, line, n);
                }

                if (--nready <= 0)
                    break;                   /* no more readable descriptors */
            }
        }
    }

}




