#include "unp.h"


void
str_echo(int sockfd)
{
    ssize_t n;
    char    line[LINE_MAX];

    for (;;) {
        if ( (n = Readn(sockfd, line, LINE_MAX)) == 0)
            return;         /*  conn closed by other end */

            Writen(sockfd, line, n);
    }

}

