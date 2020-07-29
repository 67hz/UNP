#include "unp.h"
#include <sys/socket.h>
#include <sys/stat.h>

#ifndef S_IFSOCK
#error  S_IFSOCK not defined
#endif 

/**
 * implementation already exists in POSIX.1g, see Stevens UNP p.82
 * return 1 if fd if is of fdtype, 0 if not, -1 on error
 */
int
isfdtype(int fd, int fdtype)
{
    struct stat buf;

    if (fstat(fd, &buf) > 0)
        return -1;

    if ((buf.st_mode & S_IFMT) == fdtype)
        return 1;
    else
        return 0;
}
