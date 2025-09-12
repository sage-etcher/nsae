
#define LOG_NOWARN
#include "log.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LOG_BUF_MAX 2048

FILE *g_log_file_stream = NULL;
uint8_t *g_log_categories = NULL;
size_t g_log_categories_count = 0;


void
log_init (size_t n)
{
    g_log_file_stream = NULL;
    g_log_categories = malloc (n * sizeof (uint8_t));
    g_log_categories_count = n;

    log_set (LOG_INFO);
}

void
log_quit (void)
{
    free (g_log_categories);
    g_log_categories = NULL;
    g_log_categories_count = 0;
}

void
log_set (uint8_t minimum_severity)
{
    for (size_t i = 0; i < g_log_categories_count; i++)
    {
        g_log_categories[i] = minimum_severity;
    }
}

void
log_set_cat (uint8_t category, uint8_t minimum_severity)
{
    g_log_categories[category] = minimum_severity;
}

void
log_fn (uint8_t category, uint8_t severity, const char *fmt, ...)
{
    /* exit early if severity is insufficient */
    unsigned minimum_severity = g_log_categories[category];

    if (minimum_severity > severity) return;

    /* generate log message */
    char buf[LOG_BUF_MAX+1];

    va_list args;
    va_start (args, fmt);
    int i = vsnprintf (buf, LOG_BUF_MAX, fmt, args);
    va_end (args);

    if (i < 0) return;

    /* log to primary stream */
    FILE *stream = (severity >= LOG_WARNING) ? stderr : stdout;
    fprintf (stream, "%s", buf);

    /* duplicate logs to file aswell */
    if (g_log_file_stream != NULL)
    {
        fprintf (g_log_file_stream, "%s", buf);
    }
}

/* end of file */
