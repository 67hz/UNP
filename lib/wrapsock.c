#include "unp.h"

void
Listen(int fd, int backlog)
{
    char *ptr;

    /* can override arg with env var */
    if ( (ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);

    if (listen(fd, backlog) < 0)
        err_sys("listen error");
}

int Socket(int family, int type, int protocol)
{
    int n;
    if ( (n = socket(family, type, protocol)) <  1) {
        perror("socket error");
        return -1;
    }
    return n;
}
