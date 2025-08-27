
#include "client.h"

#include "log.h"
#include "nsaeipc.h"
#include "nsaecmd.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void vsend_cmd (uint8_t cmd, va_list args);

static void send_byte (uint8_t b);
static void send_string (char *s);
static void send_u32 (uint32_t u);


void
send_cmd (uint8_t cmd, ...)
{
    va_list args;
    va_start (args, cmd);
    vsend_cmd (cmd, args);
    va_end (args);
}

static void
vsend_cmd (uint8_t cmd, va_list args)
{
    switch (cmd)
    {
        /* cmd addr */
        case NSAE_CMD_BREAKPOINT:
            send_byte (cmd);
            send_u32 (va_arg (args, uint32_t));
            break;

        /* cmd fd_num */
        case NSAE_CMD_EJECT_FD:
            send_byte (cmd);
            send_byte ((uint8_t)va_arg (args, int));
            break;

        /* cmd fd_num len filename */
        case NSAE_CMD_LOAD_FD:
        case NSAE_CMD_SAVE_FD:
            send_byte (cmd);
            send_byte ((uint8_t)va_arg (args, int));
            send_string (va_arg (args, char *));
            break;

        /* cmd len filename */
        case NSAE_CMD_LOAD_HD:
        case NSAE_CMD_SAVE_HD:
            send_byte (cmd);
            send_string (va_arg (args, char *));
            break;

        /* cmd */
        case NSAE_CMD_CONTINUE:
        case NSAE_CMD_EJECT_HD:
        case NSAE_CMD_EXIT:
        case NSAE_CMD_PAUSE:
        case NSAE_CMD_RESTART:
        case NSAE_CMD_RUN:
        case NSAE_CMD_STATUS:
        case NSAE_CMD_STEP:
            send_byte (cmd);
            break;

        /* error */
        default: 
            log_error ("nsaectl: error: unknown command\n");
            abort ();
    }
}

static void
send_byte (uint8_t b)
{
    log_verbose ("nsaectl: send: (u8)%u\n", b);
    nsae_ipc_send (&b, sizeof (uint8_t));
}

static void
send_string (char *s)
{
    size_t len = strlen (s);
    log_verbose ("nsaectl: send: (size_t)%zu '%s'\n", len, s);
    nsae_ipc_send ((uint8_t *)&len, sizeof (size_t));
    nsae_ipc_send ((uint8_t *)s, len);
}

static void
send_u32 (uint32_t u)
{
    log_verbose ("nsaectl: send: (u32)%u\n", u);
    nsae_ipc_send ((uint8_t *)&u, sizeof (uint32_t));
}

/* end of file */
