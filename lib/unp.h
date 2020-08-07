#ifndef _UNP_H
#define _UNP_H 1
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
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/stat.h>
#include <stdarg.h>


#define SERV_PORT           9877
#define SERV_PORT_STR       "9877"


#define LISTENQ             20

/* Error Handling */
void err_msg(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_ret(const char *fmt, ...);

/* IO */
ssize_t
readline(int fd, void *vptr, size_t maxlen);

ssize_t
Readn(int fd, void *ptr, size_t n);

void
Writen(int fd, void *ptr, size_t nbytes);

/* type checking */
int
isfdtype(int fd, int fdtype);

/* sockets */
void
Listen(int fd, int backlog);

int
Socket(int family, int type, int protocol);

int Sockfd_to_family(int sockfd);

/* comms */
void str_echo (int sockfd);


#define MAX(a,b) \
    ({ __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b; })


#endif /* unp.h */
