/* IPv4 server */
#include "unp.h"

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

    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    ssize_t nread;
    struct sockaddr_in servaddr, cliaddr;
    struct sigaction sa;
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

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags |= SA_RESTART;
    sa.sa_handler = sig_child;

#if 0
    /* catch SIGCHILD (child terminated) to avoid zombie children */
    /* old, obsoleted, undefined way */
    if (signal(SIGCHLD, sig_child) == SIG_ERR)
        err_sys("signal error");
#endif

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        err_sys("signal error");

    for (;;) {
        clilen = sizeof(cliaddr);

        if ( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
            if (errno == EINTR)
                continue;
            err_ret("accept error");
        }

        int client_port = ntohs(cliaddr.sin_port);

/* CHILD */
        if ( (childpid = fork()) == 0) {
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
/* end CHILD */


        if (close(connfd) == -1) {      /* parent closes conn */
            perror("close connfd");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);

}


