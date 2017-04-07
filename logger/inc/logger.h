#ifndef __LOGGER_H__
#define __LOGGER_H__

void print_time_str(void);

typedef enum logger_ {
    dbgInfo,
    dbgWarn,
    dbgErr,
    dbgCrit,
    dbgMax
} logger_e;

void
logger(logger_e lvl, const char *fmt, ...);

#endif /* __LOGGER_H__ */
