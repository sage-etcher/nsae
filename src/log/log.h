
#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

enum {
    LOG_DEBUG,
    LOG_VERBOSE,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
    LOG_COUNT,
};

extern FILE *g_log_file_stream;
extern uint8_t *g_log_categories;
extern size_t   g_log_categories_count;

void log_init (size_t n);
void log_quit (void);
void log_set (uint8_t minimum_severity);
void log_set_cat (uint8_t category, uint8_t minimum_severity);
void log_fn (uint8_t category, uint8_t severity, const char *fmt, ...);


#if !defined(LOG_CATEGORY) && !defined(LOG_NOWARN)
#   warning "log: LOG_CATEGORY macro undefined, cannot provide logging macros"

#else
#define log_debug(...)   log_fn (LOG_CATEGORY, LOG_DEBUG,   __VA_ARGS__)
#define log_verbose(...) log_fn (LOG_CATEGORY, LOG_VERBOSE, __VA_ARGS__)
#define log_info(...)    log_fn (LOG_CATEGORY, LOG_INFO,    __VA_ARGS__)
#define log_warning(...) log_fn (LOG_CATEGORY, LOG_WARNING, __VA_ARGS__)
#define log_error(...)   log_fn (LOG_CATEGORY, LOG_ERROR,   __VA_ARGS__)
#define log_fatal(...)   log_fn (LOG_CATEGORY, LOG_FATAL,   __VA_ARGS__)

#endif /* defined(LOG_CATEGORY) */

#endif /* LOG_H */
/* end of file */
