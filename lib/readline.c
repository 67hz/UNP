#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include "unp.h"

static int read_cnt; /* static int inits to 0 */
static char *read_ptr;
static char read_buf[LINE_MAX];

static size_t
my_read(int fd, char *ptr)
{
    while (read_cnt <= 0) {
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        } else if (read_cnt == 0)
            return 0;
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}


ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;      /* store NL, like fgets() */
        } else if (rc == 0) {
            if (n == 1)     /* no data read */
                return 0;
            else
                break;      /* EOF, some data read */
        } else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }
    *ptr = 0;               /* null terminate, like fgets() */
    return n;
}
