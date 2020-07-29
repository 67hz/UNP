#ifndef _UNP_H
#define _UNP_H 1
#include <unistd.h>
#include <sys/stat.h>

ssize_t
readline(int fd, void *vptr, size_t maxlen);

int
isfdtype(int fd, int fdtype);



#endif /* unp.h */
