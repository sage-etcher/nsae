
#define LOG_NOWARN
#include "log.h"

#include "darray.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef DARRAY_T (char) log_string_t;


#define LOG_BUF_MAX 2048

FILE *g_log_file_stream = NULL;
uint8_t *g_log_categories = NULL;
size_t g_log_categories_count = 0;

int s_log_output_string_enable = 0;
log_string_t s_log_output_string = {
    .elem_size = sizeof (char),
    .alloc = 0,
    .count = 0,
    .m = NULL,
};


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

    /* log to string aswell */
    if (s_log_output_string_enable)
    {
        log_string_append (buf);
    }

}

void
log_string_set_state (int state)
{
    s_log_output_string_enable = state;
}

void
log_string_destroy (void)
{
    free (s_log_output_string.m);
    s_log_output_string.m = NULL;
    s_log_output_string.alloc = 0;
    s_log_output_string.count = 0;
}

void
log_string_append (char *msg)
{
    void *tmp = NULL;
    char *string_end = NULL;
    size_t msg_length = 0;

    if (msg == NULL) { return; }

    msg_length = strlen (msg);
    tmp = da_resize ((da_void_t *)&s_log_output_string, msg_length);
    assert (tmp != NULL);

    string_end = &s_log_output_string.m[s_log_output_string.count];
    memcpy (string_end, msg, msg_length);

    s_log_output_string.count += msg_length;
}

char *
log_string_get (void)
{
    size_t length = 0;
    char *output = NULL;

    if ((s_log_output_string.m == NULL) ||
        (s_log_output_string.count == 0))
    {
        return NULL;
    }

    length = s_log_output_string.count;
    output = malloc (length + 1);
    assert (output != NULL);

    memcpy (output, s_log_output_string.m, length);
    output[length] = '\0';

    return output;
}

/* end of file */
