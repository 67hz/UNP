#include "unp.h"

/* 
 * read n bytes from a descriptor
 */
static ssize_t
readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, n)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;   /* real error occured */
        } else if (nread == 0) {        /* EOF */
            break;
        }

        nleft -= nread;
        ptr += nread;       /* advance as many as read */
    }

    return (n - nleft);
}

ssize_t
Readn(int fd, void *ptr, size_t nbytes)
{
    ssize_t n;
    if ( (n = readn(fd, ptr, nbytes)) <= 0)
        err_sys("readn error");

    return n;
}
