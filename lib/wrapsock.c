#include "unp.h"
#include <asm-generic/errno.h>

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

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int accepted_fd;
    
    while ( (accepted_fd = accept(fd, sa, salenptr)) < 0) {
#ifdef EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
            continue;
        else
            err_sys("accept error");
    }
    return accepted_fd;
}
