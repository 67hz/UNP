
#include "unp.h"

#define LOG_INFO        0
#define LOG_WARNING     1
#define LOG_ERR         2

static
void err_doit(int loglevel, const char *fmt, va_list ap)
{
    char buf[LINE_MAX + 1];
    vsnprintf(buf, LINE_MAX, fmt, ap);
}

/*
 * non-fatal error related to system call
 * print and return
 */

void err_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(LOG_INFO, fmt, ap);
    return;
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
    err_doit(LOG_ERR, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}





