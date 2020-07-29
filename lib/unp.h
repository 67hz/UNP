#ifndef _UNP_H
#define _UNP_H 1
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <stdlib.h>

/* Error Handling */
void err_sys(const char *fmt, ...);

/* IO */
ssize_t
readline(int fd, void *vptr, size_t maxlen);

/* type checking */
int
isfdtype(int fd, int fdtype);

/* sockets */
void
Listen(int fd, int backlog);

int
Socket(int family, int type, int protocol);

int Sockfd_to_family(int sockfd);

#endif /* unp.h */
