#include <unistd.h>
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

int Socket(int family, int type, int protocol)
{
    int n;
    if ( (n = socket(family, type, protocol)) <  1) {
        perror("socket error");
        return -1;
    }
    return n;
}

int main(int argc, char *argv[])
{

    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buf[LINE_MAX];
    time_t ticks;

    if ( (listenfd = Socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(EXIT_FAILURE);

    memset (&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);  /* dayime server */

    if (bind (listenfd,
                (const struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* backlog set to 20 */
    listen(listenfd, 20);

    for (;;) {
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

        ticks = time(NULL);
        
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        if (write(connfd, buf, strlen(buf)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        if (close(connfd) == -1) {
            perror("close connfd");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);


}


