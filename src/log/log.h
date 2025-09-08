
#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

extern FILE *g_log_fp;

extern bool g_log_info;
extern bool g_log_verbose;
extern bool g_log_debug;
extern bool g_log_warning;
extern bool g_log_error;

#define log_conditional(cond, fp, ...)                          \
    do                                                          \
    {                                                           \
        if (cond)                                               \
        {                                                       \
            fprintf (fp, __VA_ARGS__);                          \
                                                                \
            if (g_log_fp != NULL)                               \
            {                                                   \
                fprintf (g_log_fp, __VA_ARGS__);                \
                fflush (g_log_fp);                              \
            }                                                   \
        }                                                       \
    }                                                           \
    while (0)

#define log_info(...)    log_conditional (g_log_info,    stdout, __VA_ARGS__)
#define log_verbose(...) log_conditional (g_log_verbose, stdout, __VA_ARGS__)
#define log_debug(...)   log_conditional (g_log_debug,   stderr, __VA_ARGS__)
#define log_warning(...) log_conditional (g_log_warning, stderr, __VA_ARGS__)
#define log_error(...)   log_conditional (g_log_error,   stderr, __VA_ARGS__)

#endif /* LOG_H */
/* end of file */
