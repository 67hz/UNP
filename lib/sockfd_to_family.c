#include "unp.h"
#include <unistd.h>

static int
sockfd_to_family(int sockfd)
{
    struct sockaddr_storage ss;
    socklen_t len;

    if (getsockname(sockfd, (struct sockaddr *) &ss, &len) < 0)
        return -1;

    return ss.ss_family;
}

/* see <sys/socket.h for family values */
int
Sockfd_to_family(int sockfd)
{
    int rc;

    if ( (rc = sockfd_to_family(sockfd)) < 0)
        err_sys("sockfd_to_family error %d", rc);

    return rc;
}

