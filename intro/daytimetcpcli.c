/* simple tcp client */

#include "unp.h"

#define SA                   const struct sockaddr

static void
sig_pipe(int signo)
{
    pid_t pid;
    int stat;

    pid = getpid();
    printf("%d terminated with [%s]\n", pid, sys_siglist[signo]);
    return;
}

static void
str_cli(FILE *fp, int sockfd)
{
    int maxfdpl;
    fd_set rset;
    char recvline[LINE_MAX + 1], sendline[LINE_MAX + 1];

    FD_ZERO(&rset);

    for (;;) {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdpl = MAX(fileno(fp), sockfd) + 1;

        if (select(maxfdpl, &rset, NULL, NULL, NULL) == -1)
            err_sys("select");

        if (FD_ISSET(sockfd, &rset)) {  /* socket is readable */

            if (readline(sockfd, recvline, LINE_MAX) == 0)
                err_sys("server terminated prematurely");

            if (fputs(recvline, stdout) == EOF)
                err_sys("fputs errored");
        }

        if (FD_ISSET(fileno(fp), &rset)) { /* stdin is readable */
            if (fgets(sendline, LINE_MAX, fp) != NULL) {
#ifdef MULTIWRITE
                Writen(sockfd, sendline, 1); /* get RST */
                sleep(3);
                Writen(sockfd, sendline+1, strlen(sendline) - 1); /* gen SIGPIPE */
#else
                Writen(sockfd, sendline, strlen(sendline)); /* gen SIGPIPE */
#endif
            }
        }
    }
}


int main(int argc, char *argv[])
{
    int sockfd;

    socklen_t cli_len;
    struct sigaction sa;
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

    sigemptyset(&sa.sa_mask);
    sa.sa_flags |= SA_INTERRUPT;
    sa.sa_handler = sig_pipe;

    if (sigaction(SIGPIPE, &sa, NULL) == -1)
        err_sys("signal error");

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

    str_cli(stdin, sockfd);

    exit(EXIT_SUCCESS);
}
