#include "unp.h"

#include <stdarg.h>
#include <syslog.h>

#define NO_ERRNO        0
#define PRINT_ERRNO     1

int daemon_proc;        /* set nonzero by daemon_init() */

static void err_doit(int, int, const char *, va_list);

/*
 * non-fatal error related to system call
 * print and return
 */

void err_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(NO_ERRNO, LOG_INFO, fmt, ap);
    return;
}

void err_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(NO_ERRNO, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

/*
 * fatal error - system call failed
 * print message and terminate
 * do not terminate from outside main otherwise
 */
void err_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(PRINT_ERRNO, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

static
void err_doit(int errnoflag, int loglevel, const char *fmt, va_list ap)
{
    int errno_save, n;
    char buf[LINE_MAX + 1];

    errno_save = errno;

    vsnprintf(buf, LINE_MAX, fmt, ap);

    n = strlen(buf);
    if (errnoflag)
        snprintf(buf + n, LINE_MAX - n, ": %s", strerror(errno_save));
    strncat(buf, "\n", 2);

    if (daemon_proc) {
        syslog(loglevel, "%s", buf);
    } else {
        fflush(stdout); /* in case stdout (1) and stderr (2) are same */
        fputs(buf, stderr);
        fflush(stderr);
    }
    return;
}





