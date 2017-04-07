#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "logger.h"

/**
 * Helper to dump timestamp including ms
 *
 * @param buffer (i/o) buffer in which to store timestamp
 * @return void
 */
void
print_time_str(void)
{
    char buffer[100];
    int millisec;
    struct tm* tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
    if (millisec>=1000) { // Allow for rounding up to nearest second
        millisec -=1000;
        tv.tv_sec++;
    }
    tm_info = localtime(&tv.tv_sec);

    /* format final output */
    strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
    printf("%s.%03d: ", buffer, millisec);
}

/**
 * Which strings get printed for a debug lvl 
 */
const char *log_str[] =
{
    "INFO",
    "WARN",
    " ERR",
    "CRIT",
};

/**
 * Helper to get a log_str corresponding to a given log_lvl
 *
 * @param lvl (i) log level
 * @return char* for the log_str
 */
static const char*
get_log_str(logger_e lvl)
{
    if (lvl >= dbgMax) {
        printf("Error: unknown debug lvl '%d', defaulting to INFO\n", lvl);
        return log_str[0];
    }

    return log_str[lvl];
}

/**
 * Logger utility
 *
 * Prints log messages at the given log level
 * Note, callers do not have to provide a newline in the format,
 * this API will append a newline automatically
 *
 * @param lvl (i) requested logging level
 * @parap fmt (i) format specifier
 * @param ... (i) string to print
 * @return void
 */
void
logger(logger_e lvl, const char *fmt, ...)
{
    va_list arglist;
    
    /* dump the timestamp */
    print_time_str();

    /* dump the log str */
    printf("%s: ", get_log_str(lvl));

    /* format the rest of the output */
    va_start(arglist, fmt);
    vprintf(fmt, arglist);
    va_end(arglist);
    
    /* tack on a newline */
    printf("\n");
}

