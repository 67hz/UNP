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


int main(int argc, char *argv[])
{
    int sockfd;
    char recvline[LINE_MAX + 1];
    char sendline[LINE_MAX + 1];
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

    /* write STDIN to server, block*/
    while (fgets(sendline, LINE_MAX, stdin) != NULL) {

#ifdef MULTIWRITE
        Writen(sockfd, sendline, 1); /* get RST */
        sleep(1);
        Writen(sockfd, sendline+1, strlen(sendline) - 1); /* gen SIGPIPE */
#else
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
