/* IPv4 server */
#include "unp.h"
#include <sys/select.h>

static void
sig_child(int signo)
{
    pid_t pid;
    int stat;

    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("%d terminated with [%s]\n", pid, sys_siglist[signo]);   /* unsafe to print */
    return;
}


int main(int argc, char *argv[])
{

    int index, max_client_in_use_index, maxfd, listenfd, connfd, sockfd, nready;
    int client[FD_SETSIZE];
    pid_t childpid;
    socklen_t clilen;
    ssize_t nread;
    fd_set allset, readset;
    struct sockaddr_in servaddr, cliaddr;
    char line_buf[LINE_MAX];

    if ( (listenfd = Socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(EXIT_FAILURE);

    memset (&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);


    if (bind(listenfd, (const struct sockaddr *) &servaddr,
                sizeof(servaddr)) == -1)
        err_sys("bind");

    listen(listenfd, LISTENQ);

    maxfd = listenfd;
    max_client_in_use_index = -1;

    for (index = 0; index < FD_SETSIZE; index++)
        client[index] = -1;         /* -1 -> available entry */

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for (;;) {
        readset = allset;
        if ( (nready = select(maxfd + 1, &readset, NULL, NULL, NULL)) == -1)
            err_sys("select");

        if (FD_ISSET(listenfd, &readset)) {                /* new client conn */
            clilen = sizeof(cliaddr);

            if ( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) == -1)
                err_sys("accept");

            for (index = 0; index < FD_SETSIZE; index++)
                if (client[index] < 0) {
                    client[index] = connfd;
                    break;
                }

            if (index == FD_SETSIZE)
                err_sys("too many connections");

            FD_SET(connfd, &allset);        /* add new connfd to set */
            if (connfd > maxfd)
                maxfd = connfd;             /* for select */

            if (index > max_client_in_use_index)
                max_client_in_use_index = index;

            if (--nready <= 0)
                continue;                   /* no more readable descriptors */

        }

        /* check all clients for data */
        for (index = 0; index <= max_client_in_use_index; index++) {
            if ( (sockfd = client[index]) < 0)
                continue;

            if (FD_ISSET(sockfd, &readset)) {

                if ( (nread = readline (sockfd, line_buf, LINE_MAX)) == 0) {
                    /* conn closed by client */
                    if (close(sockfd) == -1)
                        err_sys("close");

                    FD_CLR(sockfd, &allset);
                    client[index] = -1;
                } else {
                    Writen(sockfd, line_buf, strlen(line_buf));
                }

                if (--nready <= 0)
                    break;                  /* no more readable descripors */

            }
        }
    }


    exit(EXIT_SUCCESS);

}


